/*
 * main.cpp
 *
 * A working example for Toolbox::NeuralNetwork
 */

#include <iostream>
#include <stdexcept>

#include <Toolbox/NeuralNetwork/Feedforward.hpp>
#include <Toolbox/NeuralNetwork/Trainer.hpp>


//////////////////////////////////////////////////////////////////////////////
// Typedefs, variable definitions, etc.
//////////////////////////////////////////////////////////////////////////////
using Toolbox::NeuralNetwork::Nucleus;			// From <Toolbox/NeuralNetwork/Neuron.hpp>
using Toolbox::NeuralNetwork::Neuron;
using Toolbox::NeuralNetwork::LabeledNeuron;

using Toolbox::NeuralNetwork::Ganglion;			// From <Toolbox/NeuralNetwork/Ganglion.hpp>

using Toolbox::NeuralNetwork::Feedforward;		// From <Toolbox/NeuralNetwork/Feedforwrd.hpp>

using Toolbox::NeuralNetwork::TrainingData;		// From <Toolbox/NeuralNetwork/Trainer.hpp>
using Toolbox::NeuralNetwork::TrainingSet;
using Toolbox::NeuralNetwork::Trainer;


// A couple of "typing savers" for our training data
// The default network type uses a sigmoid activation, so our default ON/OFF values need to reflect that
const double OFF = Toolbox::NeuralNetwork::Default::OFF::Sigmoid;
const double ON  = Toolbox::NeuralNetwork::Default::ON::Sigmoid;
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Here are a few quick/dirty functions that let us peer into the network
//////////////////////////////////////////////////////////////////////////////
void PrintNeuron( Neuron::Ptr neuron, unsigned int indent_spaces = 2 )
{
	if ( !neuron )
		throw std::runtime_error( "PrintNeuron(): No neuron provided." );

	std::string Indent("");
	Indent.resize( indent_spaces, ' ' );

	std::cout << Indent << "Neuron: " << neuron->Label() << std::endl;

	Indent.push_back( ' ' );
	Indent.push_back( ' ' );

	std::cout << Indent << "Value: " << neuron->Value() << "  (Prev: " << neuron->PrevValue() << ")" << std::endl;
	std::cout << Indent << "Dendrites (" << neuron->Dendrites.size() << "):" << std::endl;

	LabeledNeuron::Ptr ln;
	bool First = true;

	for ( auto d = neuron->Dendrites.begin(), d_end = neuron->Dendrites.end(); d != d_end; ++d )
	{
		ln = std::dynamic_pointer_cast< LabeledNeuron >( (d->first).lock() );

		if ( First )
		{
			std::cout << Indent << "  ";
			First = false;
		}
		else
		{
			std::cout << ", ";
		}

		if ( ln )
			std::cout << ln->Label() << " (" << d->second << ")";
		else
			std::cout << (d->first).lock() << " (" << d->second << ")";
	}

	if ( !neuron->Dendrites.empty() )
		std::cout << std::endl;

	std::cout << Indent << "Axons (" << neuron->Axons.size() << "):" << std::endl;

	First = true;
	for ( auto a = neuron->Axons.begin(), a_end = neuron->Axons.end(); a != a_end; ++a )
	{
		ln = std::dynamic_pointer_cast< LabeledNeuron >( a->lock() );

		if ( First )
		{
			std::cout << Indent << "  ";
			First = false;
		}
		else
			std::cout << ", ";

		if ( ln )
			std::cout << ln->Label();
		else
			std::cout << a->lock();
	}

	if ( !neuron->Axons.empty() )
		std::cout << std::endl;
}


void PrintGanglion( Ganglion::Ptr network, unsigned int indent_spaces = 2 )
{
	if ( !network )
		throw std::runtime_error( "PrintGanglion(): No network provided." );

	std::string Indent("");
	Indent.resize( indent_spaces, ' ' );

	std::cout << Indent << "---------------------------------------" << std::endl;
	std::cout << Indent << "-- Bias: " << network->BiasNeuron << std::endl;
	std::cout << Indent << "-- Inputs --" << std::endl;

	for ( auto i = network->Input.begin(), i_end = network->Input.end(); i != i_end; ++i )
		PrintNeuron( i->second, indent_spaces + 2 );

	std::cout << Indent << "-- Hidden Layers --" << std::endl;

	unsigned int NumLayers = 0;
	for ( auto l = network->Hidden.begin(), l_end = network->Hidden.end(); l != l_end; ++l )
	{
		++NumLayers;

		std::cout << Indent << "  -- Hidden " << NumLayers << " --" << std::endl;

		for ( auto h = l->second.begin(), h_end = l->second.end(); h != h_end; ++h )
			PrintNeuron( h->second, indent_spaces + 4 );
	}


	std::cout << Indent << "-- Outputs --" << std::endl;

	for ( auto o = network->Output.begin(), o_end = network->Output.end(); o != o_end; ++o )
		PrintNeuron( o->second, indent_spaces + 2 );

	std::cout << Indent << "---------------------------------------" << std::endl;
}
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Prepare an "easy interface" to work with training data in its raw form
//////////////////////////////////////////////////////////////////////////////
struct XORRow
{
	double Input[ 2 ];
	double Output[ 1 ];
};


const size_t XORData_Max = 4;

const XORRow XORData[ XORData_Max ] =
{
	// Inputs        Outputs
	//   1    2        1
	{ { OFF, OFF },	{ OFF }	},
	{ {  ON, OFF },	{  ON }	},
	{ { OFF,  ON },	{  ON }	},
	{ {  ON,  ON },	{ OFF }	}
};
//////////////////////////////////////////////////////////////////////////////


int main( int argc, char *argv[] )
{
    try
    {
		//
		// Create the structure for a simple network capable of solving XOR using the "pre-built" feedforward network
		//
		std::cout << "---------------------------------------" << std::endl;
		std::cout << " XOR" << std::endl;
		std::cout << "---------------------------------------" << std::endl;
		Feedforward::Ptr XORNet = std::make_shared< Feedforward >();
		XORNet->NewInput( "Input 1" );
		XORNet->NewInput( "Input 2" );
		XORNet->NewHiddenLayer( 2 );
		XORNet->NewOutput( "Output" );
		XORNet->ConnectNetwork();

		std::cout << "  The initial XOR network state:" << std::endl;
		PrintGanglion( XORNet );

		//
		// Prepare Training Data
		// Use our prepared XORData above to populate our XORSet
		//
		TrainingSet XORSet( *XORNet );			// Copy the inputs and outputs from our network
		TrainingData NewRow;					// Note: If any inputs or output names don't match those of in the TrainingSet, they will be ignored when being added to the TrainingSet!

		for ( size_t x = 0; x < XORData_Max; ++x )
		{
			NewRow.Clear();
			NewRow.Input[ "Input 1" ] = XORData[ x ].Input[ 0 ];
			NewRow.Input[ "Input 2" ] = XORData[ x ].Input[ 1 ];
			NewRow.Output[ "Output" ] = XORData[ x ].Output[ 0 ];
			XORSet.AddRecord( NewRow );
		}

		//
		// Train the network
		//
		Trainer Backprop;

		double Error = 0;						// This allows us to get our optional error return parameter so we know exactly how trained our network is
		size_t NumCycles = 0;					// This allows us to get our optional count of how many cycles it took our network to train

		std::cout << "  Training XOR dataset... (ON: " << ON << ", OFF: " << OFF << ")" << std::endl;
		if ( Backprop.BatchTrain(*XORNet, XORSet, &Error, &NumCycles) )
			std::cout << "  * XOR dataset learned.  (Cycles: " << NumCycles << ", Error: " << Error << ")" << std::endl;
		else
			std::cout << "  * XOR dataset NOT learned!  (Cycles: " << NumCycles << ", Error: " << Error << ")" << std::endl;

		std::cout << "  The trained XOR network state:" << std::endl;
		PrintGanglion( XORNet );

		//
		// Validate the network (typically with a different data set, but in this case we just reuse the original training data)
		//
		Error = 0;								// Prepare to read the error again during validation

		std::cout << "  Validating XOR dataset..." << std::endl;
		if ( Backprop.Validate(*XORNet, XORSet, &Error) ) 
			std::cout << "  * XOR dataset validated.  (Error: " << Error << ")" << std::endl;
		else
			std::cout << "  * XOR dataset NOT validated!  (Error: " << Error << ")" << std::endl;

		//
		// Manual Testing
		//
		std::cout << "  Manual validation of XOR dataset (allows for visualization too):" << std::endl;
		XORNet->SetInput( "Input 1", OFF );
		XORNet->SetInput( "Input 2", OFF );
		XORNet->Process();
		std::cout << "  OFF, OFF: " << XORNet->GetOutput( "Output" ) << std::endl;

		XORNet->SetInput( "Input 1", ON );
		XORNet->SetInput( "Input 2", OFF );
		XORNet->Process();
		std::cout << "   ON, OFF: " << XORNet->GetOutput( "Output" ) << std::endl;

		XORNet->SetInput( "Input 1", OFF );
		XORNet->SetInput( "Input 2", ON );
		XORNet->Process();
		std::cout << "  OFF,  ON: " << XORNet->GetOutput( "Output" ) << std::endl;

		XORNet->SetInput( "Input 1", ON );
		XORNet->SetInput( "Input 2", ON );
		XORNet->Process();
		std::cout << "   ON,  ON: " << XORNet->GetOutput( "Output" ) << std::endl;
		std::cout << "---------------------------------------" << std::endl;

// For easy disabling
#if 0
		//
		// This section shows the creation and assignment of a completely pre-built neural network
		// The model network is the one found here: http://mattmazur.com/2015/03/17/a-step-by-step-backpropagation-example/
		//
		std::cout << "---------------------------------------" << std::endl;
		std::cout << " http://mattmazur.com/2015/03/17/a-step-by-step-backpropagation-example/" << std::endl;
		std::cout << "---------------------------------------" << std::endl;
		Feedforward::Ptr StepByStepNet = std::make_shared< Feedforward >();

		// Create the basic network framework
		StepByStepNet->NewInput( "1" );
		StepByStepNet->NewInput( "2" );
		StepByStepNet->NewHiddenLayer( 2 );
		StepByStepNet->NewOutput( "1" );
		StepByStepNet->NewOutput( "2" );

		// And connect everything (this can be done manually too, but for fully-connected feedforward nets, this is fine)
		StepByStepNet->ConnectNetwork();

		// Set custom weight values on each specific neuron...keep in mind that each dendrite holds the weight value for each associated input
		// There is only 1 hidden layer, so the first Hidden index is simply 0.  If we had two hidden layers, an index of 1 would address the 2nd layer, etc.
		StepByStepNet->Hidden[0][0]->Dendrites[ StepByStepNet->Input["1"] ] = 0.15;
		StepByStepNet->Hidden[0][0]->Dendrites[ StepByStepNet->Input["2"] ] = 0.20;
		StepByStepNet->Hidden[0][0]->Dendrites[ StepByStepNet->BiasNeuron ] = 0.35;
		StepByStepNet->Hidden[0][1]->Dendrites[ StepByStepNet->Input["1"] ] = 0.25;
		StepByStepNet->Hidden[0][1]->Dendrites[ StepByStepNet->Input["2"] ] = 0.30;
		StepByStepNet->Hidden[0][1]->Dendrites[ StepByStepNet->BiasNeuron ] = 0.35;
		StepByStepNet->Output[ "1" ]->Dendrites[ StepByStepNet->Hidden[0][0] ] = 0.40;
		StepByStepNet->Output[ "1" ]->Dendrites[ StepByStepNet->Hidden[0][1] ] = 0.45;
		StepByStepNet->Output[ "1" ]->Dendrites[ StepByStepNet->BiasNeuron ] = 0.60;
		StepByStepNet->Output[ "2" ]->Dendrites[ StepByStepNet->Hidden[0][0] ] = 0.50;
		StepByStepNet->Output[ "2" ]->Dendrites[ StepByStepNet->Hidden[0][1] ] = 0.55;
		StepByStepNet->Output[ "2" ]->Dendrites[ StepByStepNet->BiasNeuron ] = 0.60;

		// Setting the input values as well
		StepByStepNet->SetInput( "1", 0.05 );
		StepByStepNet->SetInput( "2", 0.10 );

		std::cout << "  The initial network state:" << std::endl;
		PrintGanglion( StepByStepNet );

		// Run a forward-pass of the network
		StepByStepNet->Process();

		// Get the outputs
		double Out1 = StepByStepNet->GetOutput( "1" );
		double Out2 = StepByStepNet->GetOutput( "2" );

		// And run a couple of the calculations from the tutorial to verify
		double Error1 = (0.01 - Out1);
		Error1 = (Error1 * Error1) / 2;

		double Error2 = (0.99 - Out2);
		Error2 = (Error2 * Error2) / 2;

		double TotalError = (Error1 + Error2);

		std::cout << "Out 1: " << Out1 << "  Out 2: " << Out2 << std::endl;
		std::cout << "Error 1: " << Error1 << "  Error 2: " << Error2 << "  Total Error: " << TotalError << std::endl;
		std::cout << "---------------------------------------" << std::endl;
#endif // 0
    }
    catch ( std::exception &ex )
    {
		std::cerr << "Fatal error: " << ex.what() << std::endl;
		return 1;
    }

    return 0;
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


