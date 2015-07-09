#ifndef TOOLBOX_NEURALNETWORK_NEURON
#define TOOLBOX_NEURALNETWORK_NEURON

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
    typedef Toolbox::NeuralNetwork::Neuron				Neuron;	// a.k.a. tNeuron< tNucleus<double> >
#else
	typedef int 										tNetworkDataType;

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
	MyNeuron->AddAxon( MyNeuron );						// Creates a recurrent connection to itself

 ****************************************************************************/
/****************************************************************************/


#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <random>
#include <stdexcept>

#include <cmath>

#include <Toolbox/Memory.hpp>


namespace Toolbox
{
	namespace NeuralNetwork
	{
		// Forward declarations and typedefs
		typedef double tNeurotransmitter_Default;
		template <typename tNucleus> class _Neuron;


		// Our interface to customize our neurons.  Passed as a template parameter to our Neuron.
		// tNeurotransmitter is built with the assumption to be of type 'double', but is templated for expansion possibilities -- this is true for all related classes with this template parameter
		template <typename _tNeurotransmitter = tNeurotransmitter_Default>
		class tNucleus
		{
		public:
			typedef _tNeurotransmitter				tNeurotransmitter;
			typedef _Neuron< tNeurotransmitter >	tNeuron;			// Base class!
			typedef std::shared_ptr< tNeuron >		tNeuron_Ptr;		// Ptr to base Neuron class!

		public:
			virtual ~tNucleus()
			{
			}

			// The input summation function
			virtual tNeurotransmitter Summation( tNeuron_Ptr self );	// NOTE: Default implementation exists below Neuron class definition

			virtual tNeurotransmitter Activation( tNeurotransmitter value )
			{
				return tanh( value );
			}

			// The inverse of our activation function (used during backprop learning)
			virtual tNeurotransmitter Derivation( tNeurotransmitter value )
			{
			}
		};


		// Then define our default Nucleus
		typedef tNucleus<>	Nucleus;


		// Neurons maintain both dendrites and axons to assist algorithms (like backprop) with neighbor neuron checking -- tNeurotransmitter type MUST match Nucleus tNeurotransmitter
		template <typename _tNeurotransmitter = tNeurotransmitter_Default>
		class _Neuron : public std::enable_shared_from_this< _Neuron<_tNeurotransmitter> >
		{
		public:
			typedef _tNeurotransmitter				tNeurotransmitter;
			typedef _Neuron< tNeurotransmitter >	tNeuron;

			TOOLBOX_MEMORY_POINTERS( tNeuron )

			typedef std::map< wPtr, tNeurotransmitter, std::owner_less<wPtr> >	tDendrites;	// Neuron inputs & weights
			typedef std::list< wPtr >			tAxons;		// Neuron outputs

		public:
			_Neuron():
				_Processed( false ),
				_CurValue( tNeurotransmitter(0) ),
				_PrevValue( tNeurotransmitter(0) ),
				_Threshold( tNeurotransmitter(0.5) )
			{
			}

			virtual ~_Neuron() = 0;

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

			const tNeurotransmitter &Threshold() const
			{
				return _Threshold;
			}

			void SetThreshold( tNeurotransmitter threshold )
			{
				_Threshold = threshold;
			}

			const tDendrites &Dendrites() const
			{
				return _Dendrites;
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

			const tAxons &Axons() const
			{
				return _Axons;
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

				auto d = _Dendrites.find( neuron );

				if ( d == _Dendrites.end() )
					throw std::runtime_error( "Toolbox::NeuralNetwork::GetWeight(): Neuron not found." );

				return d->second;
			}

			// Signals that we need to be processed
			void NeedsProcessing()
			{
				_Processed = false;
			}

			// Should be called from children even if return value is ignored -- Tracking this is faster than tracking unique additions to processing lists
			virtual bool Process()
			{
				bool NeedsProcessing = !_Processed;
				_Processed = true;
				return NeedsProcessing;
			}

		protected:
			tDendrites			_Dendrites;
			tAxons				_Axons;

			bool				_Processed;

			tNeurotransmitter	_CurValue;
			tNeurotransmitter	_PrevValue;

			tNeurotransmitter	_Threshold;		// Only fire if it is '>=' this value

		protected:
			// The private versions only act on 'this', but public versions maintain both sides
			void _addDendrite( Ptr neuron, tNeurotransmitter initialWeight )
			{
				if ( !neuron )
					throw std::runtime_error( "Toolbox::NeuralNetwork::AddDendrite(): No neuron provided." );

				_Dendrites.insert( std::pair<wPtr, tNeurotransmitter>(wPtr(neuron), initialWeight) );
			}

			void _removeDendrite( Ptr neuron )
			{
				if ( !neuron )
					throw std::runtime_error( "Toolbox::NeuralNetwork::RemoveDendrite(): No neuron provided." );

				auto d = _Dendrites.find( neuron );

				if ( d == _Dendrites.end() )
					throw std::runtime_error( "Toolbox::NeuralNetwork::RemoveDendrite(): Neuron not found." );

				_Dendrites.erase( d );
			}

			void _addAxon( Ptr neuron )
			{
				if ( !neuron )
					throw std::runtime_error( "Toolbox::NeuralNetwork::AddAxon(): No neuron provided." );

				_Axons.push_back( neuron );
			}

			void _removeAxon( Ptr neuron )
			{
				if ( !neuron )
					throw std::runtime_error( "Toolbox::NeuralNetwork::RemoveAxon(): No neuron provided." );

				for ( auto a = _Axons.begin(), a_end = _Axons.end(); a != a_end; ++a )
				{
					if ( a->lock() == neuron )
					{
						_Axons.erase( a );
						return;
					}
				}

				throw std::runtime_error( "Toolbox::NeuralNetwork::RemoveAxon(): Neuron not found." );
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
		tNeurotransmitter tNucleus<tNeurotransmitter>::Summation( std::shared_ptr< _Neuron<tNeurotransmitter> > self )
		{
			if ( !self )
				throw std::runtime_error( "Toolbox::NeuralNetwork::Nucleus::Summation(): No 'self' neuron provided!" );

			tNeurotransmitter Value = tNeurotransmitter( 0 );

			// Sum up the weighted inputs
			for ( auto d = self->Dendrites().begin(), d_end = self->Dendrites().end(); d != d_end; ++d )
			{
				auto Dendrite = (d->first).lock();

				if ( Dendrite )
					Value += Dendrite->Value() * d->second;
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

			TOOLBOX_MEMORY_POINTERS( ttNeuron )

			typedef _Neuron< tNeurotransmitter >			Parent;

		public:
			virtual ~tNeuron()
			{
			}

			// Returns 'true' if the neuron fires during procesing
			virtual bool Process()
			{
				// Check with our parent about processing (also sets internal state...should be called even if the return value is ignored)
				if ( !Parent::Process() )
					return false;

				this->_PrevValue = this->_CurValue;
				this->_CurValue = tNeurotransmitter( 0 );

				this->_CurValue = this->_Nucleus.Summation( this->shared_from_this() );

				this->_CurValue = this->_Nucleus.Activation( this->_CurValue );

				if ( this->_CurValue >= this->_Threshold )
					return true;
			}

		protected:
			// An interface/container for our activation functions, and related items
			// All neurons of this type will always use the same type of nucleus (hence static)
			static tNucleus 	_Nucleus;
		};


		// Make sure to declare our static nucleus
		template <typename tNucleus>
		tNucleus tNeuron< tNucleus >::_Nucleus;


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

			TOOLBOX_MEMORY_POINTERS( tLabeledNeuron )

		public:
			tLabeledNeuron():
				_Label( "Neuron" )
			{
			}

			tLabeledNeuron( const std::string &label ):
				_Label( label )
			{
			}

			virtual ~tLabeledNeuron()
			{
			}

			const std::string &Label() const
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


#endif // TOOLBOX_NEURALNETWORK_NEURON


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper


