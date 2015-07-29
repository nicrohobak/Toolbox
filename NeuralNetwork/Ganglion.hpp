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
	typedef Toolbox::NeuralNetwork::Ganglion		Ganglion; // a.k.a tGanglion< tNeuron<tNucleus<double>> >

	// Generate a quick XOR-capable network
	Ganglion MyGanglion;
	MyGanglion->NewInput( "Input 1" );
	MyGanglion->NewInput( "Input 2" );
	MyGanglion->NewHiddenLayer( 4 );
	MyGanglion->NewHiddenLayer( 3 );
	MyGanglion->NewOutput( "Output" );
	MyGanglion->ConnectNetwork();

	// ... Train the network here ...
	// See <Toolbox/NeuralNetwork/Trainer.hpp> for more info

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
		namespace Default
		{
			const size_t MaxProcessingCycles	= 100000;
		}


		template <typename _tNeuron = Neuron>
		class tGanglion : public std::enable_shared_from_this< tGanglion<_tNeuron> >
		{
		public:
			typedef _tNeuron																	ttNeuron;

			TOOLBOX_MEMORY_POINTERS( tGanglion<ttNeuron> )

			typedef typename ttNeuron::tNeurotransmitter										tNeurotransmitter;
			typedef typename ttNeuron::tNucleus													tNucleus;
			typedef tLabeledNeuron< tNucleus >													ttLabeledNeuron;

			typedef unsigned int																tNeuronIndex;
			typedef std::map< tNeuronIndex, std::map<tNeuronIndex, typename ttNeuron::Ptr> >	tHiddenLayers;
			typedef std::map< std::string, typename ttLabeledNeuron::Ptr >						tIOLayer;
			typedef std::list< typename _Neuron<tNeurotransmitter>::Ptr >						tNeuronList;

		public:
			tIOLayer				Input;
			tIOLayer				Output;
			tHiddenLayers			Hidden;

			bool					UseBias;
			typename ttNeuron::Ptr	BiasNeuron;
			tNeurotransmitter		DefaultThreshold;

		public:
			tGanglion():
				UseBias( true ),
				DefaultThreshold( tNeurotransmitter() )
			{
				_CreateBias();
			}

			tGanglion( bool useThreshold, const tNeurotransmitter &value = tNeurotransmitter(1) ):
				UseBias( !useThreshold ),
				DefaultThreshold( value )
			{
				_CreateBias();
			}

			virtual ~tGanglion()
			{
			}

			void NewInput( const std::string &label )
			{
				Input[ label ] = std::make_shared< ttLabeledNeuron >( label, DefaultThreshold );
			}

			void SetInput( const std::string &label, tNeurotransmitter value = tNeurotransmitter() )
			{
				auto CurInput = Input.find( label );

				if ( CurInput == Input.end() )
					throw std::runtime_error( std::string("Toolbox::NeuralNetwork::Ganglion::SetInput(): Input '") + label + std::string("' not found.") );

				Input[ label ]->SetValue( value );
			}

			void NewOutput( const std::string &label )
			{
				Output[ label ] = std::make_shared< ttLabeledNeuron >( label, DefaultThreshold );
			}

			tNeurotransmitter GetOutput( const std::string &label )
			{
				auto CurOutput = Output.find( label );

				if ( CurOutput == Output.end() )
					throw std::runtime_error( std::string("Toolbox::NeuralNetwork::Ganglion::GetOutput(): Output '") + label + std::string("' not found.") );

				return CurOutput->second->Value();
			}

			typename ttNeuron::Ptr GetOutputNeuron( const std::string &label )
			{
				auto CurOutput = Output.find( label );

				if ( CurOutput == Output.end() )
					throw std::runtime_error( std::string("Toolbox::NeuralNetwork::Ganglion::GetOutputNeuron(): Output '") + label + std::string("' not found.") );

				return CurOutput->second;
			}

			void NewHiddenLayer( unsigned int numNeurons )
			{
				tNeuronIndex CurLayer = Hidden.size();

				if ( CurLayer != 0 )
					++CurLayer;

				for ( unsigned int i = 0; i < numNeurons; ++i )
					Hidden[ CurLayer ][ i ] = std::make_shared< ttNeuron >( DefaultThreshold );
			}

			// By default, we connect as a fully-connected, feed-forward network
			virtual void ConnectNetwork()
			{
				for ( auto i = Input.begin(), i_end = Input.end(); i != i_end; ++i )
				{
					auto CurInput = i->second;

					if ( !Hidden.empty() )
					{
						auto FirstHiddenLayer = *(Hidden.begin());

						// Connect the inputs to the first hidden layer
						for ( auto h = FirstHiddenLayer.second.begin(), h_end = FirstHiddenLayer.second.end(); h != h_end; ++h )
						{
							auto CurHidden = h->second;
							CurInput->AddAxon( CurHidden );
						}
					}
					else
					{
						// Connect the inputs directly to the outputs
						for ( auto o = Output.begin(), o_end = Output.end(); o != o_end; ++o )
						{
							auto CurOutput = o->second;
							CurInput->AddAxon( CurOutput );
						}
					}
				}

				if ( !Hidden.empty() )
				{
					// Connect the hidden layers together -- go to the second-to-last element since we won't be connecting the last to the outputs here
					for ( auto l1 = Hidden.begin(), l_end = Hidden.end(); l1 != l_end; ++l1 )
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

							if ( UseBias )
								BiasNeuron->AddAxon( Hidden1 );

							for ( auto h2 = NextLayer.begin(), h2_end = NextLayer.end(); h2 != h2_end; ++h2 )
							{
								auto Hidden2 = h2->second;
								Hidden1->AddAxon( Hidden2 );
							}
						}
					}

					// Connect the last hidden layer to the outputs
					for ( auto o = Output.begin(), o_end = Output.end(); o != o_end; ++o )
					{
						auto CurOutput = o->second;

						if ( UseBias )
							BiasNeuron->AddAxon( CurOutput );

						auto LastHiddenLayer = *(Hidden.rbegin());

						for ( auto h = LastHiddenLayer.second.begin(), h_end = LastHiddenLayer.second.end(); h != h_end; ++h )
						{
							auto HiddenNeuron = h->second;

							if ( UseBias )
								BiasNeuron->AddAxon( HiddenNeuron );

							CurOutput->AddDendrite( HiddenNeuron );
						}
					}
				}
			}

			// All input values should be set prior to processing the network -- will stop after maxProcessingCycles or when no more neurons need processing (typically when the network settles and "generates output")
			void Process( size_t maxProcessingCycles = Default::MaxProcessingCycles )
			{
				tNeuronList ProcessingList, NextList;

				// Start by adding the input neurons to our processing list
				for ( auto i = Input.begin(), i_end = Input.end(); i != i_end; ++i )
				{
					auto Input = i->second;
					Input->NeedsProcessing();
					ProcessingList.push_back( Input );
				}

				unsigned int CurCycle = 0;

				// While we still have anything in it, lets go through our processing list
				while ( !ProcessingList.empty() )
				{
					if ( maxProcessingCycles != 0 && CurCycle++ >= maxProcessingCycles )
						break;

					NextList.clear();

					for ( auto n = ProcessingList.begin(), n_end = ProcessingList.end(); n != n_end; ++n )
					{
						// If neurons fire, add their axons to the next processing list
						if ( (*n)->Process(!UseBias) )
						{
							for ( auto a = (*n)->Axons.begin(), a_end = (*n)->Axons.end(); a != a_end; ++a )
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
			void _CreateBias()
			{
				BiasNeuron = std::make_shared< ttNeuron >();
				BiasNeuron->SetValue( tNeurotransmitter(1) );
			}
		};


		// Then define our default Ganglion type
		typedef tGanglion<>	Ganglion;
	}
}


#endif // TOOLBOX_NEURALNETWORK_GANGLION


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


