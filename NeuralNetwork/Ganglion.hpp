#ifndef TOOLBOX_NEURALNETWORK_GANGLION
#define TOOLBOX_NEURALNETWORK_GANGLION

/*
 * Toolbox/NeuralNetwork/Ganglion.hpp
 *
 * A highly generic, structureless, recurency-capable, neural network that
 * serves as the basis for our "higher order" neural networks.
 */


/****************************************************************************
 * Notes:
 *
 * - A ganglion is a generic name for a cluster of neurons.  A neural network
 *   is simply a ganglion with a desginated purpose.
 *
 * - The Ganglion class provides a generic interface for manipulating a
 *   neural network as a whole unit.
 *
 ****************************************************************************
 * TODO:
 *
 * - Add backprop training functions/interface
 *
 ****************************************************************************
	typedef Toolbox::NeuralNetwork::Ganglion		Ganglion; // a.k.a tGanglion< tNeuron<tNucleus<double>> >

	// Generate a quick XOR-capable network
	Ganglion MyGanglion;
	MyGanglion->AddInput( "Input 1" );
	MyGanglion->AddInput( "Input 2" );
	MyGanglion->AddHiddenLayer( 4 );
	MyGanglion->AddHiddenLayer( 3 );
	MyGanglion->AddOutput( "Output" );
	MyGanglion->ConnectNetwork();

	// ... Train the network here ...

	// Once trained, execute by setting the input values
	MyGanglion->SetInput( "Input 1", 1.0 );
	MyGanglion->SetInput( "Input 2", 0.0 );

	// And then processing the network
	MyGanglion->Process();

	// Finally, get our output from the network
	double Output = MyGanglion->GetOutput( "Output" );

 ****************************************************************************/
/****************************************************************************/


#include <Toolbox/NeuralNetwork/Neuron.hpp>


namespace Toolbox
{
	namespace NeuralNetwork
	{
		template <typename _tNeuron = Neuron>
		class tGanglion : public std::enable_shared_from_this< tGanglion<_tNeuron> >
		{
		public:
			typedef std::shared_ptr< tGanglion<_tNeuron> >			Ptr;
			typedef std::weak_ptr< tGanglion<_tNeuron> >			wPtr;

			typedef typename _tNeuron::tNeurotransmitter			tNeurotransmitter;
			typedef typename _tNeuron::tNucleus						tNucleus;
			typedef tLabeledNeuron< tNucleus >						ttLabeledNeuron;

			typedef unsigned int									tNeuronIndex;
			typedef std::map< tNeuronIndex, std::map<tNeuronIndex, typename _tNeuron::Ptr> >	tHiddenLayers;
			typedef std::map< std::string, typename ttLabeledNeuron::Ptr >	tIOLayer;
			typedef std::list< typename _Neuron<tNeurotransmitter>::Ptr >	tNeuronList;

		public:
			tGanglion()
			{
			}

			virtual ~tGanglion()
			{
			}

			tIOLayer &Inputs()
			{
				return _Inputs;
			}

			const tIOLayer &Inputs() const
			{
				return _Inputs;
			}

			tIOLayer &Outputs()
			{
				return _Outputs;
			}

			const tIOLayer &Outputs() const
			{
				return _Outputs;
			}

			tHiddenLayers &Hidden()
			{
				return _Hidden;
			}

			const tHiddenLayers &Hidden() const
			{
				return _Hidden;
			}

			void AddInput( const std::string &label )
			{
				_Inputs[ label ] = std::make_shared< ttLabeledNeuron >( label );
			}

			void SetInput( const std::string &label, tNeurotransmitter value = tNeurotransmitter(0) )
			{
				auto Input = _Inputs.find( label );

				if ( Input == _Inputs.end() )
					throw std::runtime_error( std::string("Toolbox::NeuralNetwork::Ganglion::SetInput(): Input '") + label + std::string("' not found.") );

				_Inputs[ label ]->SetValue( value );
			}

			void AddOutput( const std::string &label )
			{
				_Outputs[ label ] = std::make_shared< ttLabeledNeuron >( label );
			}

			tNeurotransmitter GetOutput( const std::string &label )
			{
				auto Output = _Outputs.find( label );

				if ( Output == _Outputs.end() )
					throw std::runtime_error( std::string("Toolbox::NeuralNetwork::Ganglion::GetOutput(): Output '") + label + std::string("' not found.") );

				return Output->second->Value();
			}

			void AddHiddenLayer( unsigned int numNeurons )
			{
				tNeuronIndex CurLayer = _Hidden.size();
				++CurLayer;

				for ( unsigned int i = 0; i < numNeurons; ++i )
					_Hidden[ CurLayer ][ i ] = std::make_shared< _tNeuron >();
			}

			// By default, we connect as a fully-connected, feed-forward network
			virtual void ConnectNetwork()
			{
				for ( auto i = _Inputs.begin(), i_end = _Inputs.end(); i != i_end; ++i )
				{
					auto Input = i->second;

					if ( !_Hidden.empty() )
					{
						auto FirstHiddenLayer = *(_Hidden.begin());

						// Connect the inputs to the first hidden layer
						for ( auto h = FirstHiddenLayer.second.begin(), h_end = FirstHiddenLayer.second.end(); h != h_end; ++h )
						{
							auto Hidden = h->second;
							Input->AddAxon( Hidden );
						}
					}
					else
					{
						// Connect the inputs directly to the outputs
						for ( auto o = _Outputs.begin(), o_end = _Outputs.end(); o != o_end; ++o )
						{
							auto Output = o->second;
							Input->AddAxon( Output );
						}
					}
				}

				if ( !_Hidden.empty() )
				{
					// Connect the hidden layers together -- go to the second-to-last element since we won't be connecting the last to the outputs here
					for ( auto l1 = _Hidden.begin(), l_end = _Hidden.end(); l1 != l_end; ++l1 )
					{
						// Make sure we have a "next hidden layer" to connect to
						auto l2 = l1;
						if ( ++l2 == l_end )
							break;

						auto CurLayer = l1->second;
						auto NextLayer = l2->second;

						for ( auto h1 = CurLayer.begin(), h1_end = CurLayer.end(); h1 != h1_end; ++h1 )
						{
							auto Hidden1 = h1->second;

							for ( auto h2 = NextLayer.begin(), h2_end = NextLayer.end(); h2 != h2_end; ++h2 )
							{
								auto Hidden2 = h2->second;

								Hidden1->AddAxon( Hidden2 );
							}
						}
					}

					// Connect the last hidden layer to the outputs
					for ( auto o = _Outputs.begin(), o_end = _Outputs.end(); o != o_end; ++o )
					{
						auto Output = o->second;

						auto LastHiddenLayer = *(_Hidden.rbegin());

						for ( auto h = LastHiddenLayer.second.begin(), h_end = LastHiddenLayer.second.end(); h != h_end; ++h )
						{
							auto Hidden = h->second;
							Output->AddDendrite( Hidden );
						}
					}
				}
			}

			// All input values should be set prior to processing the network -- will stop after maxProcessingCycles or when no more neurons need processing (typically when the network settles and "generates output")
			void Process( unsigned int maxProcessingCycles = 10000 )
			{
				tNeuronList ProcessingList, NextList;

				// Start by adding the input neurons to our processing list
				for ( auto i = _Inputs.begin(), i_end = _Inputs.end(); i != i_end; ++i )
				{
					auto Input = i->second;
					Input->NeedsProcessing();
					ProcessingList.push_back( Input );
				}

				unsigned int LoopCount = 0;
				// While we still have anything in it, lets go through our processing list
				while ( !ProcessingList.empty() )
				{
					if ( maxProcessingCycles != 0 && LoopCount++ >= maxProcessingCycles )
						break;

					NextList.clear();

					for ( auto n = ProcessingList.begin(), n_end = ProcessingList.end(); n != n_end; ++n )
					{
						// If neurons fire, add their axons to the next processing list
						if ( (*n)->Process() )
						{
							for ( auto a = (*n)->Axons().begin(), a_end = (*n)->Axons().end(); a != a_end; ++a )
							{
								auto Axon = a->lock();

								if ( Axon )
								{
									Axon->NeedsProcessing();
									NextList.push_back( Axon );
								}
							}
						}
					}

					// Copy our processing list to prepare for the next round
					ProcessingList = NextList;
				}
			}

		protected:
			tIOLayer		_Inputs;
			tIOLayer		_Outputs;
			tHiddenLayers	_Hidden;
		};


		// Then define our default Ganglion type
		typedef tGanglion<>	Ganglion;
	}
}


#endif // TOOLBOX_NEURALNETWORK_NEURALNETWORK


// vim: tabstop=4 shiftwidth=4



