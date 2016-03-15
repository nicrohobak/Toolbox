#ifndef TOOLBOX_NEURALNETWORK_NEURON_HPP
#define TOOLBOX_NEURALNETWORK_NEURON_HPP

/*
 * Toolbox/NeuralNetwork/Neuron.hpp
 *
 * The basis of neurons for our networks.
 */

/****************************************************************************
 * Notes:
 *
 * - Neurons themselves have some functions that a user of a neural network
 *   needs to have some control over (activation functions, etc.), so this
 *   structure has been developed to accomodate that need, while providing
 *   all of the other basic functionality of a neuron abstracted away.
 *
 * - If default values are used, these neurons will form the basis of a
 *   standard multi-layer, feed-forward network with a sigmoid (tanh)
 *   activation and the ability to learn via backpropagation.
 *
 * - The _Neuron<> class is the pure-virtual base class for all Neurons.  It
 *   accepts a tNeurotransmitter as its template parameter (that must
 *   ultimately match the tNeurotransmitter of its Nucleus<>).
 *
 * - The tNeuron<> class is the specific Neuron implementation that accepts a
 *   tNucleus<> type as its template parameter.  Neurons cannot be instanced
 *   without using tNeuron<> and assigning a tNucleus<>.
 *
 * - The default tNucleus<> uses a 'double' type tNeurotransmitter (this sets
 *   the "basic unit" for the Neuron [classically float or double]) and has
 *   default functions that will operate as a network with sigmoid (tanh)
 *   activation and backpropagation learning.  The Nucleus<> can set this
 *   unit as a template parameter for future expansion possibilities
 *   relating to neural networks that can process different basic units of
 *   information, or even of multiple kinds of data simultaneously (with the
 *   help of a special custom datatype).
 *
 * - Custom nuclei are created by deriving tNucleus<> (with your preferred
 *   base tNeurotransmitter type) and overriding the functions provided
 *   via the tNucleus<> parent class.  A custom tNucleus<> MUST define the
 *   Activation function at minimum.  Any tNucleus<> that uses something other
 *   than float/double for its tNeurotransmitter MUST define a custom
 *   tNucleus<> instance.
 *
 * - Outside of determining the basic unit, a tNucleus<> also contains all of
 *   functions a programmer may want to customize for any particular Neuron
 *   (summation/activation/etc.)
 *
 *****************************************************************************
#if USE_DEFAULT_NEURON
    typedef Toolbox::NeuralNetwork::Neuron	Neuron;	// a.k.a. tNeuron< tNucleus<double> >
#else
	typedef int 							tNetworkDataType;

	// Defines custom neuron functions and the core network data unit (int, in this case)
	class CustomNucleus : public Toolbox::NeuralNetwork::tNucleus< tNetworkDataType >
	{
	public:
		virtual tNeurotransmitter Activation( tNetworkDataType value )
		{
			// Custom activation function calculation goes here
			return 1;
		}
	};

	typedef CustomNucleus								Nucleus;
	typedef Toolbox::NeuralNetwork::tNeuron< Nucleus >	Neuron;
#endif // USE_DEFAULT_NEURON

	Neuron::Ptr MyNeuron = std::make_shared< Neuron >();
	MyNeuron->AddAxon( MyNeuron );						// Creates a loopback connection to itself

 ****************************************************************************/
/****************************************************************************/


#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>

#include <cmath>

#include <Toolbox/Defines.h>


namespace Toolbox
{
	namespace NeuralNetwork
	{
		// Forward declarations
		template <typename tNucleus> class _Neuron;

		// Defaults
		namespace Default
		{
			typedef double		tNeurotransmitter;
		}


		//
		// Default Activation Functions
		//
		namespace Activation
		{
			template <typename tType>
			tType Linear( tType value )
			{
				return value;
			}

			template <typename tType>
			tType Sigmoid( tType value )
			{
				return tType(1) / (tType(1) + exp(-value));
			}

			template <typename tType>
			tType FastSigmoid( tType value )
			{
				return value / (tType(1) + std::abs(value));
			}

			template <typename tType>
			tType TanH( tType value )
			{
				return tanh( value );
			}
		}


		//
		// Default Derivation Functions (Activation counterparts, typically used during network training via backpropagation)
		//
		namespace Derivation
		{
			template <typename tType>
			tType Linear( tType value )
			{
				return value;
			}

			template <typename tType>
			tType Sigmoid( tType value )
			{
				return value * (tType(1) - value);
			}

			template <typename tType>
			tType FastSigmoid( tType value )
			{
				return Sigmoid( value );
			}

			template <typename tType>
			tType TanH( tType value )
			{
				// Thanks to Mike Seymour on StackOverflow for this snippet
				//  - http://stackoverflow.com/users/204847/mike-seymour
				tType sh = tType(1) / std::cosh( value );		// sech(x) = 1/cosh(x)
				return sh * sh;									// sech^2(x)
			}
		}


		// Our interface to customize our neurons.  Passed as a template parameter to our Neuron.
		// tNeurotransmitter is built with the assumption to be of type 'double', but is templated for expansion possibilities -- this is true for all related classes with this template parameter
		template <typename _tNeurotransmitter = Default::tNeurotransmitter>
		class tNucleus
		{
		public:
			typedef _tNeurotransmitter				tNeurotransmitter;
			typedef _Neuron< tNeurotransmitter >	tNeuron;			// Base class!
			typedef std::shared_ptr< tNeuron >		tNeuron_Ptr;

		public:
			virtual ~tNucleus()
			{
			}

			// NOTE: Default implementation exists below Neuron class definition
			virtual tNeurotransmitter Accumulator( tNeuron_Ptr self );

			virtual tNeurotransmitter Activation( tNeurotransmitter value )
			{
				return Activation::Sigmoid( value );
				//return Activation::TanH( value );
			}

			// The inverse of our activation function (used during backprop learning)
			virtual tNeurotransmitter Derivation( tNeurotransmitter value )
			{
				return Derivation::Sigmoid( value );
				//return Derivation::TanH( value );
			}
		};


		// Then define our default Nucleus
		typedef tNucleus<>	Nucleus;


		// Neurons maintain both dendrites and axons to assist algorithms (like backprop) with neighbor neuron checking
		template <typename _tNeurotransmitter = Default::tNeurotransmitter>
		class _Neuron : public std::enable_shared_from_this< _Neuron<_tNeurotransmitter> >
		{
		public:
			typedef _tNeurotransmitter					tNeurotransmitter;
			typedef _Neuron< tNeurotransmitter >		tNeuron;

			TOOLBOX_POINTERS( tNeuron )

			typedef std::map< wPtr, tNeurotransmitter,
						std::owner_less<wPtr> >			tDendrites;	// Neuron inputs & weights
			typedef std::list< wPtr >					tAxons;		// Neuron outputs

		public:
			tDendrites									Dendrites;
			tAxons										Axons;

			tNeurotransmitter							Threshold;		// Only fire if it is '>=' this value

		public:
			_Neuron():
				Threshold( tNeurotransmitter() ),
				_Processed( false ),
				_Activated( false ),
				_CurValue( tNeurotransmitter() ),
				_PrevValue( tNeurotransmitter() )
			{
			}

			_Neuron( const tNeurotransmitter &threshold ):
				Threshold( threshold ),
				_Processed( false ),
				_Activated( false ),
				_CurValue( tNeurotransmitter() ),
				_PrevValue( tNeurotransmitter() )
			{
			}

			virtual ~_Neuron() = 0;

			virtual std::string Label() const
			{
				std::stringstream label("");
				label << this;
				return label.str();
			}

			bool Processed() const
			{
				return _Processed;
			}

			bool Activated() const
			{
				return _Activated;
			}

			const tNeurotransmitter &Value() const
			{
				return _CurValue;
			}

			const tNeurotransmitter &PrevValue() const
			{
				return _PrevValue;
			}

			void SetValue( tNeurotransmitter value )
			{
				_CurValue = value;
				_PrevValue = value;
				_Processed = false;
			}

			void AddDendrite( Ptr neuron )
			{
				AddDendrite( neuron, _generateRandomWeight() );
			}

			void AddDendrite( Ptr neuron, tNeurotransmitter initialWeight )
			{
				_addDendrite( neuron, initialWeight );
				neuron->_addAxon( this->shared_from_this() );
			}

			void RemoveDendrite( Ptr neuron )
			{
				_removeDendrite( neuron );
				neuron->_removeAxon( this->shared_from_this() );
			}

			void AddAxon( Ptr neuron )
			{
				AddAxon( neuron, _generateRandomWeight() );
			}

			void AddAxon( Ptr neuron, tNeurotransmitter initialWeight )
			{
				_addAxon( neuron );
				neuron->_addDendrite( this->shared_from_this(), initialWeight );
			}

			void RemoveAxon( Ptr neuron )
			{
				_removeAxon( neuron );
				neuron->_removeDendrite( this->shared_from_this() );
			}

			tNeurotransmitter GetWeight( Ptr neuron ) const
			{
				if ( !neuron )
					throw std::runtime_error( "Toolbox::NeuralNetwork::RemoveDendrite(): No neuron provided." );

				auto d = Dendrites.find( neuron );

				if ( d == Dendrites.end() )
					throw std::runtime_error( "Toolbox::NeuralNetwork::GetWeight(): Neuron not found." );

				return d->second;
			}

			// Signals that we need to be processed
			void NeedsProcessing()
			{
				_Processed = false;
				_Activated = false;
			}

			// Should be called from children even if return value is ignored -- Tracking this is faster than tracking unique additions to processing lists
			virtual bool Process( bool useThreshold = true )
			{
				bool NeedsProcessing = !_Processed;
				_Processed = true;
				return NeedsProcessing;
			}

		protected:
			bool				_Processed;
			bool				_Activated;

			tNeurotransmitter	_CurValue;
			tNeurotransmitter	_PrevValue;

		protected:
			// The private versions only act on 'this', but public versions maintain both sides
			void _addDendrite( Ptr neuron, tNeurotransmitter initialWeight )
			{
				if ( !neuron )
					throw std::runtime_error( "Toolbox::NeuralNetwork::_tNeuron<>::AddDendrite(): No neuron provided." );

				Dendrites.insert( std::pair<wPtr, tNeurotransmitter>(wPtr(neuron), initialWeight) );
			}

			void _removeDendrite( Ptr neuron )
			{
				if ( !neuron )
					throw std::runtime_error( "Toolbox::NeuralNetwork::_tNeuron<>::RemoveDendrite(): No neuron provided." );

				auto d = Dendrites.find( neuron );

				if ( d == Dendrites.end() )
					throw std::runtime_error( "Toolbox::NeuralNetwork::_tNeuron<>::RemoveDendrite(): Neuron not found." );

				Dendrites.erase( d );
			}

			void _addAxon( Ptr neuron )
			{
				if ( !neuron )
					throw std::runtime_error( "Toolbox::NeuralNetwork::_tNeuron<>::AddAxon(): No neuron provided." );

				Axons.push_back( neuron );
			}

			void _removeAxon( Ptr neuron )
			{
				if ( !neuron )
					throw std::runtime_error( "Toolbox::NeuralNetwork::_tNeuron<>::RemoveAxon(): No neuron provided." );

				for ( auto a = Axons.begin(), a_end = Axons.end(); a != a_end; ++a )
				{
					if ( a->lock() == neuron )
					{
						Axons.erase( a );
						return;
					}
				}

				throw std::runtime_error( "Toolbox::NeuralNetwork::_tNeuron<>::RemoveAxon(): Neuron not found." );
			}

			static tNeurotransmitter _generateRandomWeight()
			{
				static std::uniform_real_distribution< float >	d{ -0.75, 0.75 };	// Generate within this range
				static std::random_device						rd;
				static std::default_random_engine 				e( rd() );

				return d( e );
			}
		};

		template <typename tNeurotransmitter>
		_Neuron<tNeurotransmitter>::~_Neuron<tNeurotransmitter>()
		{
		}

		// Now that the neuron has been defined, we can define our default summation function
		// NOTE: The _Neuron<> used here is the base neuron class
		template <typename tNeurotransmitter>
		tNeurotransmitter tNucleus<tNeurotransmitter>::Accumulator( std::shared_ptr< _Neuron<tNeurotransmitter> > self )
		{
			if ( !self )
				throw std::runtime_error( "Toolbox::NeuralNetwork::tNucleus<>::Accumulator(): No 'self' neuron provided!" );

			tNeurotransmitter Value = tNeurotransmitter();

			// Check to see if we're an input/bias neuron
			if ( self->Dendrites.empty() )
			{
				// If we are, then the value is its previous value since it should have been set already (but the current could be manipulated before we get called)
				Value = self->PrevValue();
			}
			else
			{
				// Sum up the weighted inputs
				for ( auto d = self->Dendrites.begin(), d_end = self->Dendrites.end(); d != d_end; ++d )
				{
					auto Dendrite = (d->first).lock();

					if ( Dendrite && (Dendrite->Activated() || Dendrite->Dendrites.empty()) )
					{
						// Handle recursive neurons
						if ( Dendrite == self )
							Value += Dendrite->PrevValue() * d->second;
						else
							Value += Dendrite->Value() * d->second;
					}
				}
			}

			return Value;
		}


		// The tNeuron class allows us to combine any Nucleus with our Neurons
		template <typename _tNucleus = Nucleus>
		class tNeuron : public _Neuron< typename _tNucleus::tNeurotransmitter >
		{
		public:
			typedef _tNucleus								tNucleus;
			typedef tNeuron< tNucleus >						ttNeuron;
			typedef typename tNucleus::tNeurotransmitter	tNeurotransmitter;

			TOOLBOX_POINTERS( ttNeuron )

			typedef _Neuron< tNeurotransmitter >			Parent;

			// An interface/container for our activation functions, and related items
			// All neurons of this type will always use the same type of nucleus (hence static)
			static tNucleus								 	Nucleus;

		public:
			tNeuron()
			{
			}

			tNeuron( const tNeurotransmitter &threshold ):
				Parent( threshold )
			{
			}

			virtual ~tNeuron()
			{
			}

			// Returns 'true' if the neuron fires during procesing
			virtual bool Process( bool useThreshold = true )
			{
				// Check with our parent about processing (also sets internal state...should be called even if the return value is ignored)
				if ( !Parent::Process(useThreshold) )
					return false;

				this->_PrevValue = this->_CurValue;
				this->_CurValue = tNeurotransmitter();		// Zero out our value to prepare for our accumulator

				// Update our value
				this->_CurValue = this->Nucleus.Accumulator( this->shared_from_this() );

				// If we have no dendrites, assume we're an input/bias neuron and activate 100% of the time
				if ( this->Dendrites.empty() )
					this->_Activated = true;
				else
				{
					// Otherwise, run it through our activation function/check the threshold
					this->_CurValue = this->Nucleus.Activation( this->_CurValue );

					// If we're not using a threshold (bias instead), or if we exceed the set threshold
					if ( !useThreshold || this->_CurValue >= this->Threshold )
						this->_Activated = true;
				}

				return this->_Activated;
			}
		};


		// Make sure to declare our static nucleus
		template <typename tNucleus>
		tNucleus tNeuron< tNucleus >::Nucleus;


		// Then define our default Neuron type
		typedef tNeuron<>	Neuron;


		// Just like a standard neuron, but with an attached label for human-readability
		template <typename _tNucleus = Nucleus>
		class tLabeledNeuron : public tNeuron< _tNucleus >
		{
		public:
			typedef _tNucleus								tNucleus;
			typedef typename tNucleus::tNeurotransmitter	tNeurotransmitter;
			typedef tLabeledNeuron< tNucleus >				ttLabeledNeuron;
			typedef tNeuron< tNucleus >						Parent;

			TOOLBOX_POINTERS( tLabeledNeuron )

		public:
			tLabeledNeuron():
				_Label( "Neuron" )
			{
			}

			tLabeledNeuron( const tNeurotransmitter &threshold ):
				Parent( threshold )
			{
			}

			tLabeledNeuron( const std::string &label, const tNeurotransmitter &threshold = tNeurotransmitter() ):
				Parent( threshold ),
				_Label( label )
			{
			}

			virtual ~tLabeledNeuron()
			{
			}

			virtual std::string Label() const
			{
				return _Label;
			}

			void SetLabel( const std::string &label )
			{
				_Label = label;
			}

		protected:
			std::string	_Label;
		};


		// Then define our default LabeledNeuron type
		typedef tLabeledNeuron<>	LabeledNeuron;
	}
}


#endif // TOOLBOX_NEURALNETWORK_NEURON_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper


