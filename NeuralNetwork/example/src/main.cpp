/*
 * main.cpp
 *
 * A working example for Toolbox::NeuralNetwork
 */

#include <iostream>
#include <stdexcept>

#include <Toolbox/NeuralNetwork/Feedforward.hpp>
#include <Toolbox/NeuralNetwork/Trainer.hpp>


// Uses the default neuron behavior (multi-layer, feed-forward)
using Toolbox::NeuralNetwork::Nucleus;			// From <Toolbox/NeuralNetwork/Neuron.hpp>
using Toolbox::NeuralNetwork::Neuron;
using Toolbox::NeuralNetwork::LabeledNeuron;

using Toolbox::NeuralNetwork::Ganglion;			// From <Toolbox/NeuralNetwork/Ganglion.hpp>

using Toolbox::NeuralNetwork::Feedforward;		// From <Toolbox/NeuralNetwork/Feedforwrd.hpp>

using Toolbox::NeuralNetwork::TrainingData;		// From <Toolbox/NeuralNetwork/Trainer.hpp>
using Toolbox::NeuralNetwork::TrainingSet;
using Toolbox::NeuralNetwork::Trainer;


//
// A couple of "typing savers" for our training data
// The default network type uses a sigmoid activation, so our default ON/OFF values need to reflect that
//
const double OFF = Toolbox::NeuralNetwork::Default::OFF::Sigmoid;
const double ON  = Toolbox::NeuralNetwork::Default::ON::Sigmoid;


//
// Here are a few quick/dirty functions that let us peer into the network
//
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


int main( int argc, char *argv[] )
{
    try
    {
		//
		// Create the structure for a simple network capable of solving XOR using the "pre-built" feedforward network
		//
		Feedforward::Ptr XORNet = std::make_shared< Feedforward >();
		XORNet->NewInput( "Input 1" );
		XORNet->NewInput( "Input 2" );
		XORNet->NewHiddenLayer( 2 );
		XORNet->NewOutput( "Output" );
		XORNet->ConnectNetwork();

		std::cout << "The initial network state:" << std::endl;
		PrintGanglion( XORNet );

		//
		// Training Data
		//
		TrainingSet XORSet;
		XORSet.CopyIOFrom( *XORNet );			// Copy the inputs and outputs from our network

		TrainingData NewRow;
		NewRow.Input[ "Input 1" ] = OFF;		// If the input name here doesn't match one that exists within the TrainingSet, it will be ignored by the TrainingSet when adding the data
		NewRow.Input[ "Input 2" ] = OFF;
		NewRow.Output[ "Output" ] = OFF;		// Same with output names that don't match
		XORSet.AddRecord( NewRow );

		NewRow.Clear();
		NewRow.Input[ "Input 1" ] = ON;
		NewRow.Input[ "Input 2" ] = OFF;
		NewRow.Output[ "Output" ] = ON;
		XORSet.AddRecord( NewRow );

		NewRow.Clear();
		NewRow.Input[ "Input 1" ] = OFF;
		NewRow.Input[ "Input 2" ] = ON;
		NewRow.Output[ "Output" ] = ON;
		XORSet.AddRecord( NewRow );

		NewRow.Clear();
		NewRow.Input[ "Input 1" ] = ON;
		NewRow.Input[ "Input 2" ] = ON;
		NewRow.Output[ "Output" ] = OFF;
		XORSet.AddRecord( NewRow );

		//
		// Training
		//
		Trainer Backprop;

		double Error = 0;						// This allows us to get our optional error return parameter so we know exactly how trained our network is

		std::cout << "Training XOR dataset... (ON: " << ON << ", OFF: " << OFF << ")" << std::endl;
		if ( Backprop.BatchTrain(*XORNet, XORSet, &Error) )
			std::cout << "XOR dataset learned.  (Error: " << Error << ")" << std::endl;
		else
			std::cout << "XOR dataset NOT learned!  (Error: " << Error << ")" << std::endl;

		std::cout << "The trained network state:" << std::endl;
		PrintGanglion( XORNet );

		Error = 0;								// Prepare to read the error again during validation

		std::cout << "Validating XOR dataset..." << std::endl;
		if ( Backprop.Validate(*XORNet, XORSet, &Error) ) 
			std::cout << "XOR dataset validated.  (Error: " << Error << ")" << std::endl;
		else
			std::cout << "XOR dataset NOT validated!  (Error: " << Error << ")" << std::endl;

		std::cout << "Manual validation of XOR dataset (allows for visualization too):" << std::endl;
		XORNet->SetInput( "Input 1", OFF );
		XORNet->SetInput( "Input 2", OFF );
		XORNet->Process();
		std::cout << "OFF, OFF: " << XORNet->GetOutput( "Output" ) << std::endl;

		XORNet->SetInput( "Input 1", ON );
		XORNet->SetInput( "Input 2", OFF );
		XORNet->Process();
		std::cout << " ON, OFF: " << XORNet->GetOutput( "Output" ) << std::endl;

		XORNet->SetInput( "Input 1", OFF );
		XORNet->SetInput( "Input 2", ON );
		XORNet->Process();
		std::cout << "OFF,  ON: " << XORNet->GetOutput( "Output" ) << std::endl;

		XORNet->SetInput( "Input 1", ON );
		XORNet->SetInput( "Input 2", ON );
		XORNet->Process();
		std::cout << " ON,  ON: " << XORNet->GetOutput( "Output" ) << std::endl;

#if 0
		//
		// This section shows the creation and assignment of a completely pre-built neural network
		// The model network is the one found here: http://mattmazur.com/2015/03/17/a-step-by-step-backpropagation-example/
		//
		Feedforward::Ptr StepByStepNet = std::make_shared< Feedforward >();

		StepByStepNet->NewInput( "1" );
		StepByStepNet->NewInput( "2" );
		StepByStepNet->NewHiddenLayer( 2 );
		StepByStepNet->NewOutput( "1" );
		StepByStepNet->NewOutput( "2" );
		StepByStepNet->ConnectStepByStepNetwork();

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

		StepByStepNet->SetInput( "1", 0.05 );
		StepByStepNet->SetInput( "2", 0.10 );

		std::cout << "The initial network state:" << std::endl;
		PrintGanglion( StepByStepNet );
		std::cout << "\n\n" << std::endl;

		StepByStepNet->Process();

		double Out1 = StepByStepNet->GetOutput( "1" );
		double Out2 = StepByStepNet->GetOutput( "2" );

		double Error1 = (0.01 - Out1);
		Error1 = (Error1 * Error1) / 2;

		double Error2 = (0.99 - Out2);
		Error2 = (Error2 * Error2) / 2;

		double TotalError = (Error1 + Error2);

		std::cout << "Out 1: " << Out1 << "  Out 2: " << Out2 << std::endl;
		std::cout << "Error 1: " << Error1 << "  Error 2: " << Error2 << "  Total Error: " << TotalError << std::endl;
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


