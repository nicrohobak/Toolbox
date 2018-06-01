#ifndef TOOLBOX_NEURALNETWORK_RECURRENT_HPP
#define TOOLBOX_NEURALNETWORK_RECURRENT_HPP

/*
 * Toolbox/NeuralNetwork/Recurrent.hpp
 *
 * A basic, fully-connected, recurrent, feed-forward neural network.
 */


/****************************************************************************
 * Notes:
 *
 * ***** THIS IS CURRENTLY UNTESTED *****
 *
 * - Please see the documentation included with Ganglion.hpp for more info.
 *
 ****************************************************************************/


#include <Toolbox/NeuralNetwork/Ganglion.hpp>

#include <vector>


namespace Toolbox
{
	namespace NeuralNetwork
	{
		// Defaults
		namespace Default
		{
			constexpr size_t	RecurrentMemorySize	= 1;		// Default size of recurrent iterations to store in node "memory"
		}


		//
		// Helps to create a special recurrency-aware neuron
		//
		template <typename _tNeurotransmitter = Default::tNeurotransmitter>
		class tRecurrentNucleus : public tNucleus< _tNeurotransmitter >
		{
		public:
			typedef _tNeurotransmitter				tNeurotransmitter;
			typedef tNucleus< tNeurotransmitter >	tParent;

			// Defined below tRecurrentNeuron
			virtual tNeurotransmitter Accumulator( std::shared_ptr< _Neuron<tNeurotransmitter> > self );
		};

		typedef tRecurrentNucleus<>			RecurrentNucleus;


		//
		// Our new RecurrentNeuron
		//
		template <typename _tNucleus = RecurrentNucleus, size_t _MemorySize = Default::RecurrentMemorySize>
		class tRecurrentNeuron : public tNeuron< _tNucleus >
		{
		public:
			typedef _tNucleus									tNucleus;
			typedef tNeuron< tNucleus >							tParent;
			typedef typename tParent::tNeurotransmitter			tNeurotransmitter;
			typedef tRecurrentNeuron< tNucleus, _MemorySize >	tSelf;

			TOOLBOX_POINTERS( tSelf )

		public:
			std::list< tNeurotransmitter >				Memory;
			std::vector< tNeurotransmitter >			MemoryWeight;

		public:
			tRecurrentNeuron():
				MemoryWeight( _MemorySize )
			{
			}

			tRecurrentNeuron( const tNeurotransmitter &threshold ):
				tParent( threshold ),
				MemoryWeight( _MemorySize )
			{
			}

			virtual std::string Label() const
			{
				std::stringstream label("(R)");
				label << this;
				return label.str();
			}

			virtual bool Process( bool useThreshold = true )
			{
				// Can't just call the parent since we need to borrow the value, so we've got to essentially copy the content so we can add what we need

				// Check with our parent about processing (also sets internal state...should be called even if the return value is ignored)
				if ( !tParent::Process(useThreshold) )
					return false;

				this->_PrevValue = this->_CurValue;
				this->_CurValue = tNeurotransmitter();		// Zero out our value to prepare for our accumulator

				// Update our value
				this->_CurValue = this->Nucleus.Accumulator( this->shared_from_this() );

				////////////////////////////////////////
				// Recurrency
				Memory.push_back( this->_CurValue );

				if ( Memory.size() > _MemorySize )
					Memory.pop_front();
				////////////////////////////////////////

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
	
			friend class tRecurrentNucleus< tNeurotransmitter >;
		};

		typedef tRecurrentNeuron<>	RecurrentNeuron;


		//
		// Definition of the RecurrentNucleus activation function, now that we have our RecurrentNeuron available for use
		//
		template <typename tNeurotransmitter>
		tNeurotransmitter tRecurrentNucleus<tNeurotransmitter>::Accumulator( std::shared_ptr< _Neuron<tNeurotransmitter> > self )
		{
			tNeurotransmitter Value = tParent::Accumulator( self );

			typedef tRecurrentNeuron< tRecurrentNucleus<tNeurotransmitter> >	tExpectedType;
			typename tExpectedType::Ptr Self = std::dynamic_pointer_cast< tExpectedType >( self );

			if ( Self )
			{
				// Recurrency
				size_t mw = 0;
				for ( auto m = Self->Memory.begin(), m_end = Self->Memory.end(); m != m_end; ++m, ++mw )
					Value += (*m) * Self->MemoryWeight[mw];
			}

			return Value;
		}


		//
		// And finally, our recurrent neural network
		//
		template <size_t _MemorySize = Default::RecurrentMemorySize, typename _tNeuron = tRecurrentNeuron<RecurrentNucleus, _MemorySize>>
		class tRecurrent : public tGanglion< _tNeuron >
		{
		public:
			typedef _tNeuron							ttNeuron;
			typedef tGanglion< ttNeuron >				tParent;
			typedef tRecurrent< _MemorySize, ttNeuron >	tSelf;

			TOOLBOX_POINTERS( tSelf )
		};

		typedef tRecurrent<>	Recurrent;
	}
}


#endif // TOOLBOX_NEURALNETWORK_RECURRENT_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper



