#ifndef TOOLBOX_NEURALNETWORK_TRAINER_HPP
#define TOOLBOX_NEURALNETWORK_TRAINER_HPP

/*
 * Toolbox/NeuralNetwork/Trainer.hpp
 *
 * The helper that trains our ganglia (via backpropagation, by default).
 */


/****************************************************************************
 * Notes:
 *
 * - The Trainer class implements a backpropagation learning algorithm by
 *   default to train our Ganglia.
 *
 * - A TrainingSet is a collection of network inputs and desired outputs used
 *   to train the network.
 *
 * - TrainingData provides an easy interface to work with a single data point
 *   from a TrainingSet.
 ****************************************************************************

 ****************************************************************************/
/****************************************************************************/

#include <Toolbox/NeuralNetwork/Ganglion.hpp>


namespace Toolbox
{
	namespace NeuralNetwork
	{
		namespace Default
		{
			const double LearningRate		= 0.3;				// The learning rate for the network (Should be -gt 0 && -lt 1)
			const double Momentum			= 0.2;				// The learning momentum for the network (Should be -gt 0 && -lt 1)
			const double AllowedError		= 0.001;			// The allowed margin of error (0.10 = 10%, 0.001 = 0.1%)
			const size_t MaxTrainingCycles	= 100000;			// How many training cycles to run through before giving up

			// Different activation functions define default ON/OFF values
			namespace OFF
			{
				const int		Linear		=  0;
				const double 	Sigmoid		=  0.001;
				const double	TanH		= -0.999;
			}

			namespace ON
			{
				const int		Linear		= 1;
				const double	Sigmoid		= 0.999;
				const double	TanH		= 0.999;
			}
		}


		template <typename _tNeurotransmitter = Default::tNeurotransmitter>
		class tTrainingData : public std::enable_shared_from_this< tTrainingData<_tNeurotransmitter> >
		{
		public:
			typedef _tNeurotransmitter						tNeurotransmitter;

			TOOLBOX_MEMORY_POINTERS( tTrainingData<tNeurotransmitter> )

		public:
			std::map< std::string, tNeurotransmitter >		Input;
			std::map< std::string, tNeurotransmitter >		Output;

		public:
			tTrainingData()
			{
			}

			~tTrainingData()
			{
			}

			void Clear()
			{
				Input.clear();
				Output.clear();
			}
		};

		typedef tTrainingData<>		TrainingData;



		template <typename _tGanglion = Ganglion>
		class tTrainingSet : public std::enable_shared_from_this< tTrainingSet<_tGanglion> >
		{
		public:
			typedef _tGanglion								ttGanglion;
			typedef typename ttGanglion::tNeurotransmitter	tNeurotransmitter;
			typedef tTrainingData< tNeurotransmitter >		ttTrainingData;
			typedef tTrainingSet< ttGanglion >				ttTrainingSet;

			TOOLBOX_MEMORY_POINTERS( tTrainingData<ttGanglion> )

		public:
			tTrainingSet()
			{
			}

			tTrainingSet( const ttGanglion &copyIO )
			{
				CopyIOFrom( copyIO );
			}

			~tTrainingSet()
			{
			}

			size_t Size() const
			{
				if ( _Inputs.empty() )
					return 0;

				auto SizeCheck = _Inputs.begin();	// Should be valid if we're not empty

				return SizeCheck->second.size();
			}
	
			void Clear()
			{
				_Inputs.clear();
				_Outputs.clear();
			}

			// Add inputs/outputs manually...
			void AddInput( const std::string &input )
			{
				if ( input.empty() )
					throw std::runtime_error( "Toolbox::NeuralNetwork::tTrainer<>::AddInput(): No input name provided." );

				_Inputs[ input ] = std::vector<tNeurotransmitter>();
			}

			void AddOutput( const std::string &output )
			{
				if ( output.empty() )
					throw std::runtime_error( "Toolbox::NeuralNetwork::tTrainer<>::AddOutput(): No output name provided." );

				_Outputs[ output ] = std::vector<tNeurotransmitter>();
			}

			// ...Or add them all at once by copying from a similar source
			void CopyIOFrom( const ttTrainingData &data )
			{
				for ( auto i = data.Input.begin(), i_end = data.Input.end(); i != i_end; ++i )
					_Inputs[ i->first ] = std::vector<tNeurotransmitter>();

				for ( auto o = data.Output.begin(), o_end = data.Output.end(); o != o_end; ++o )
					_Outputs[ o->first ] = std::vector<tNeurotransmitter>();
			}

			void CopyIOFrom( const ttTrainingSet &set )
			{
				for ( auto i = set._Inputs.begin(), i_end = set._Inputs.end(); i != i_end; ++i )
					_Inputs[ i->first ] = std::vector<tNeurotransmitter>();

				for ( auto o = set._Outputs.begin(), o_end = set._Outputs.end(); o != o_end; ++o )
					_Outputs[ o->first ] = std::vector<tNeurotransmitter>();
			}

			void CopyIOFrom( const ttGanglion &network )
			{
				for ( auto i = network.Input.begin(), i_end = network.Input.end(); i != i_end; ++i )
					_Inputs[ i->first ] = std::vector<tNeurotransmitter>();

				for ( auto o = network.Output.begin(), o_end = network.Output.end(); o != o_end; ++o )
					_Outputs[ o->first ] = std::vector<tNeurotransmitter>();
			}

			// Once data has been added, we can get individual records with this
			ttTrainingData GetRecord( size_t index ) const
			{
				ttTrainingData ReturnData;

				for ( auto i = _Inputs.begin(), i_end = _Inputs.end(); i != i_end; ++i )
					ReturnData.Input[ i->first ] = i->second[ index ];

				for ( auto o = _Outputs.begin(), o_end = _Outputs.end(); o != o_end; ++o )
					ReturnData.Output[ o->first ] = o->second[ index ];

				return ReturnData;
			}

			// Essentially the push_back() of this class
			void AddRecord( ttTrainingData &data )
			{
				for ( auto i = _Inputs.begin(), i_end = _Inputs.end(); i != i_end; ++i )
				{
					auto NewInput = data.Input.find( i->first );

					if ( NewInput != data.Input.end() )
						i->second.push_back( data.Input[NewInput->first] );
					else
						i->second.push_back( tNeurotransmitter() );
				}

				for ( auto o = _Outputs.begin(), o_end = _Outputs.end(); o != o_end; ++o )
				{
					auto NewOutput = data.Output.find( o->first );

					if ( NewOutput != data.Output.end() )
						o->second.push_back( data.Output[NewOutput->first] );
					else
						o->second.push_back( tNeurotransmitter() );
				}
			}

		protected:
			std::map< std::string, std::vector<tNeurotransmitter> >		_Inputs;
			std::map< std::string, std::vector<tNeurotransmitter> >		_Outputs;
		};

		typedef tTrainingSet<>		TrainingSet;


		namespace Default
		{
			namespace ErrorFunc
			{
				template <typename tType>
				tType MeanSquared( const tType &value )
				{
					tNeurotransmitter Error = tNeurotransmitter(0.5) * (value * value);
					return Error;
				}

				template <typename tType>
				tType ArcTan( const tType &value )
				{
					tNeurotransmitter Error = atan( value );
					return Error * Error;
				}
			}
		}


		//
		// The default trainer trains multi-layer backprop by default
		//
		template <typename _tGanglion = Ganglion>
		class tTrainer : public std::enable_shared_from_this< tTrainer<typename _tGanglion::tNeurotransmitter> >
		{
		public:
			typedef _tGanglion										ttGanglion;
			typedef typename ttGanglion::tNeurotransmitter			tNeurotransmitter;
			typedef tNeuron< tNeurotransmitter >					ttNeuron;
			typedef tLabeledNeuron< typename ttGanglion::tNucleus >	ttLabeledNeuron;
			typedef tTrainingData< tNeurotransmitter >				ttTrainingData;
			typedef tTrainingSet< ttGanglion >						ttTrainingSet;

			TOOLBOX_MEMORY_POINTERS( tTrainer<tNeurotransmitter> )

		public:
			tNeurotransmitter										LearningRate;
			tNeurotransmitter										Momentum;
			tNeurotransmitter										AllowedError;			// The allowed margin of error to be considered "trained"
			size_t													MaxTrainingCycles;		// Max overall training cycles -- 0 allows infinite
			size_t													MaxProcessingCycles;	// Max network processing cycles per each forward pass -- 0 allows infinite

		public:
			tTrainer():
				LearningRate( tNeurotransmitter(Default::LearningRate) ),
				Momentum( tNeurotransmitter(Default::Momentum) ),
				AllowedError( tNeurotransmitter(Default::AllowedError) ),
				MaxTrainingCycles( Default::MaxTrainingCycles ),
				MaxProcessingCycles( Default::MaxProcessingCycles )
			{
			}

			tTrainer( const tNeurotransmitter &learningRate, const tNeurotransmitter &momentum = tNeurotransmitter(Default::Momentum), const tNeurotransmitter &marginOfError = tNeurotransmitter(Default::AllowedError), size_t maxTrainingCycles = Default::MaxTrainingCycles, size_t maxProcessingCycles = Default::MaxProcessingCycles ):
				LearningRate( learningRate ),
				Momentum( momentum ),
				AllowedError( marginOfError ),
				MaxTrainingCycles( maxTrainingCycles ),
				MaxProcessingCycles( maxProcessingCycles )
			{
			}

			virtual ~tTrainer()
			{
			}

			// The value here is typically (target - output) of an output neuron, or the weighted error sum of a hidden neuron
			virtual tNeurotransmitter CalculateError( const tNeurotransmitter &value )
			{
				return Default::ErrorFunc::MeanSquared( value );
			}

			// Incremental training updates the weights after each run of the network, whereas batch training only updates after the entire set has been seen
			// TODO: Add momentum
			virtual bool Train( ttGanglion &network, const ttTrainingSet &set, tNeurotransmitter *networkError = NULL, bool incrementalTraining = true )
			{
				std::map< typename _Neuron<tNeurotransmitter>::Ptr, tNeurotransmitter >		_Error;
				std::map< typename _Neuron<tNeurotransmitter>::Ptr,
					std::map<typename _Neuron<tNeurotransmitter>::Ptr, tNeurotransmitter >>	_WeightUpdates, _PrevWeightUpdates;

				tNeurotransmitter SetError = tNeurotransmitter();				// The total error for a set, and ultimately our return value
				size_t TrainingSetSize = set.Size();
				size_t NumOutputs = network.Output.size();
				bool Trained = false;

				if ( TrainingSetSize <= 0 )
					throw std::runtime_error("Toolbox::NeuralNetwork::Trainer::Train(): Training set is empty.");

				// Until our margin of error is low enough...
				for ( unsigned int CurCycle = 0; ; ++CurCycle )
				{
					// ...or until we cap out
					if ( MaxTrainingCycles != 0 && CurCycle >= MaxTrainingCycles )
						break;

					SetError = tNeurotransmitter();
					_PrevWeightUpdates.clear();

					// Loop through our training set
					for ( size_t CurRecord = 0; CurRecord < TrainingSetSize; ++CurRecord )
					{
						tTrainingData< tNeurotransmitter > Record = set.GetRecord( CurRecord );

						// Clear the errors from the previous round
						tNeurotransmitter NetworkError = tNeurotransmitter();		// Calculated and compared each cycle
						_Error.clear();

						// Prepare the network with the inputs
						for ( auto i = Record.Input.begin(), i_end = Record.Input.end(); i != i_end; ++i )
							network.SetInput( i->first, i->second );

						// Process the network
						network.Process( MaxProcessingCycles );

						// Calculate the error of our output neurons
						// Probably TODO: Multithread this loop (threadpool to process neurons?)
						for ( auto o = Record.Output.begin(), o_end = Record.Output.end(); o != o_end; ++o )
						{
							auto OutputNeuron = network.GetOutputNeuron( o->first );

							if ( !OutputNeuron )
								continue;

							auto DesiredValue = o->second;

							// First calculate the error
							tNeurotransmitter Error = OutputNeuron->Value() - DesiredValue;
							_Error[ OutputNeuron ] = Error;
							NetworkError += this->CalculateError( Error );

							// Then update the weights
							size_t NumOutputs = 0;
							for ( auto d = OutputNeuron->Dendrites.begin(), d_end = OutputNeuron->Dendrites.end(); d != d_end; ++d )
							{
								auto DendriteNeuron = (d->first).lock();

								if ( !DendriteNeuron )
									continue;

								tNeurotransmitter CurMomentum = tNeurotransmitter();

								// Verify we have a previous weight to work with for our momentum
								if ( !_PrevWeightUpdates.empty() )
									CurMomentum = _PrevWeightUpdates[ OutputNeuron ][ DendriteNeuron ];

								tNeurotransmitter WeightUpdate = -LearningRate * Error * OutputNeuron->Nucleus.Derivation(OutputNeuron->Value()) * DendriteNeuron->Value();
								_WeightUpdates[ OutputNeuron ][ DendriteNeuron ] += WeightUpdate + CurMomentum;
								++NumOutputs;
							}

							// Root MSE? -- Harder to genericize
							//NetworkError = sqrt( NetworkError );
						}

						SetError += NetworkError;

						// Now, work backwards through the hidden layers calculating errors and updating weights
						// TODO: Multithread this loop (threadpool to process neurons?)
						// For each hidden layer (in reverse order)...
						for ( auto l = network.Hidden.rbegin(), l_end = network.Hidden.rend(); l != l_end; ++l )
						{
							// We look at each neuron...
							for ( auto h = l->second.begin(), h_end = l->second.end(); h != h_end; ++h )
							{
								auto HiddenNeuron = h->second;

								// If it wasn't activated, we can ignore it and move on to the next
								if ( !HiddenNeuron->Activated() )
									continue;

								tNeurotransmitter Error = tNeurotransmitter();
								tNeurotransmitter HiddenDerivation = HiddenNeuron->Nucleus.Derivation( HiddenNeuron->Value() );

								// And gather the total weighted error for this neuron from each of its axons
								for ( auto a = HiddenNeuron->Axons.begin(), a_end = HiddenNeuron->Axons.end(); a != a_end; ++a )
								{
									auto Axon = a->lock();

									if ( Axon )
									{
										// We should technically use the axon's derivation, but it should be the same as our hidden, so we're going to cheat here since we don't have the Axon nucleus...
										tNeurotransmitter CurError = _Error[ Axon ] * HiddenNeuron->Nucleus.Derivation(Axon->Value()) * Axon->Dendrites[ HiddenNeuron ];
										Error += CurError;
									}
								}

								_Error[ HiddenNeuron ] = Error;

								// Now that we know our error, calculate the weight updates
								for ( auto d = HiddenNeuron->Dendrites.begin(), d_end = HiddenNeuron->Dendrites.end(); d != d_end; ++d )
								{
									auto DendriteNeuron = (d->first).lock();

									if ( !DendriteNeuron )
										continue;

									tNeurotransmitter CurMomentum = tNeurotransmitter();

									// Verify we have a previous weight to work with for our momentum
									if ( !_PrevWeightUpdates.empty() )
										CurMomentum = _PrevWeightUpdates[ HiddenNeuron ][ DendriteNeuron ];

									tNeurotransmitter WeightUpdate = -LearningRate * Error * HiddenDerivation * DendriteNeuron->Value();
									_WeightUpdates[ HiddenNeuron ][ DendriteNeuron ] += WeightUpdate + CurMomentum;
								}
							}
						}

						// If we're doing incremental training, then update the weights immediately after each record we've calculated them for
						if ( incrementalTraining )
						{
							size_t WeightCount = 0;
							for ( auto from = _WeightUpdates.begin(), from_end = _WeightUpdates.end(); from != from_end; ++from, ++WeightCount )
							{
								auto FromNeuron = from->first;

								for ( auto to = from->second.begin(), to_end = from->second.end(); to != to_end; ++to )
								{
									auto ToNeuron = to->first;
									auto WeightUpdate = to->second;
									FromNeuron->Dendrites[ ToNeuron ] = FromNeuron->Dendrites[ ToNeuron ] + WeightUpdate;
								}
							}

							_PrevWeightUpdates = _WeightUpdates;
							_WeightUpdates.clear();
						}
					}

					// Sometimes interesting/helpful to see, but not something that should be "on" by default...candidate for a DEBUG flag or similar
					std::cout << "Cur Cycle: " << CurCycle << "  Error: " << SetError << "  (Allowed: " << AllowedError << ")" << std::endl;

					// If we get through the training set and have no errors, we're "trained" and should head to the validation set (if one was provided)
					if ( SetError <= AllowedError )
					{
						Trained = true;
						break;
					}

					// Batch training -- update weights after each entire set
					if ( !incrementalTraining )
					{
						size_t WeightCount = 0;
						for ( auto from = _WeightUpdates.begin(), from_end = _WeightUpdates.end(); from != from_end; ++from, ++WeightCount )
						{
							auto FromNeuron = from->first;

							for ( auto to = from->second.begin(), to_end = from->second.end(); to != to_end; ++to )
							{
								auto ToNeuron = to->first;
								auto WeightUpdate = to->second;
								FromNeuron->Dendrites[ ToNeuron ] = FromNeuron->Dendrites[ ToNeuron ] + WeightUpdate;
							}
						}

						_PrevWeightUpdates = _WeightUpdates;
						_WeightUpdates.clear();
					}
				}

				if ( networkError )
					*networkError = SetError;

				return Trained;
			}

			bool BatchTrain( ttGanglion &network, const ttTrainingSet &set, tNeurotransmitter *networkError = NULL )
			{
				return this->Train( network, set, networkError, false );
			}

			// Checks a data set and returns the network error for the set
			bool Validate( ttGanglion &network, const ttTrainingSet &set = ttTrainingSet(), tNeurotransmitter *error = NULL )
			{
				tNeurotransmitter SetError = tNeurotransmitter();				// The total error for a set, and ultimately our return value
				size_t TrainingSetSize = set.Size();
				size_t NumOutputs = network.Output.size();
				bool Trained = false;

				// Loop through our validation set
				for ( size_t CurRecord = 0; CurRecord < TrainingSetSize; ++CurRecord )
				{
					tTrainingData< tNeurotransmitter > Record = set.GetRecord( CurRecord );

					// Clear the errors from the previous round
					tNeurotransmitter NetworkError = tNeurotransmitter();		// Calculated and compared each cycle

					// Prepare the network with the inputs
					for ( auto i = Record.Input.begin(), i_end = Record.Input.end(); i != i_end; ++i )
						network.SetInput( i->first, i->second );

					// Process the network
					network.Process( MaxProcessingCycles );

					// Calculate the error of our output neurons
					// Probably TODO: Multithread this loop (threadpool to process neurons?)
					for ( auto o = Record.Output.begin(), o_end = Record.Output.end(); o != o_end; ++o )
					{
						auto OutputNeuron = network.GetOutputNeuron( o->first );

						if ( !OutputNeuron )
							continue;

						auto DesiredValue = o->second;

						// First calculate the error
						tNeurotransmitter Error = OutputNeuron->Value() - DesiredValue;
						NetworkError += this->CalculateError( Error );

						// Root MSE? -- Harder to genericize
						//NetworkError = sqrt( NetworkError );
					}

					SetError += NetworkError;
				}

				if ( error != NULL )
					*error = SetError;

				return SetError <= AllowedError;
			}
		};

		typedef tTrainer<>			Trainer;
	}
}


#endif // TOOLBOX_NEURALNETWORK_TRAINER_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper



