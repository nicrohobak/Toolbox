/*
 * main.cpp
 *
 * A working example for Toolbox::NeuralNetwork
 */

#include <iostream>
#include <stdexcept>

#include <cmath>

#include <Toolbox/NeuralNetwork/Feedforward.hpp>


#define USE_CUSTOM_NUCLEUS	0


#if USE_CUSTOM_NUCLEUS
	typedef double tNetworkDataType;

	// Defines custom neuron functions and the core network data unit (double, in this case)
	class CustomNucleus : public Toolbox::NeuralNetwork::Nucleus< tNetworkDataType >
	{
	public:
		virtual tNeurotransmitter Activation( tNetworkDataType value )
		{
			return 5.5;
		}
	};

	typedef CustomNucleus										Nucleus;
	typedef Toolbox::NeuralNetwork::tNeuron< Nucleus >			Neuron;
	typedef Toolbox::NeuralNetwork::tLabeledNeuron< Nucleus >	LabeledNeuron;
	typedef Toolbox::NeuralNetwork::tGanglion< Neuron >			Ganglion;
#else
	// Uses the default neuron behavior (multi-layer, feed-forward)
	using Toolbox::NeuralNetwork::Nucleus;
	using Toolbox::NeuralNetwork::Neuron;
	using Toolbox::NeuralNetwork::LabeledNeuron;
	using Toolbox::NeuralNetwork::Ganglion;
	using Toolbox::NeuralNetwork::Feedforward;
#endif


//
// Here are a few quick/dirty functions that let us peer into the network
//
void PrintNeuron( Neuron::Ptr neuron, unsigned int indent_spaces = 2 )
{
	if ( !neuron )
		throw std::runtime_error( "PrintNeuron(): No neuron provided." );

	std::string Indent("");
	Indent.resize( indent_spaces, ' ' );

	std::cout << Indent << "Value: " << neuron->Value() << "  (Prev: " << neuron->PrevValue() << ")" << std::endl;
	std::cout << Indent << "Dendrites (" << neuron->Dendrites().size() << "):" << std::endl;

	LabeledNeuron::Ptr ln;
	bool First = true;

	for ( auto d = neuron->Dendrites().begin(), d_end = neuron->Dendrites().end(); d != d_end; ++d )
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

	if ( !neuron->Dendrites().empty() )
		std::cout << std::endl;

	std::cout << Indent << "Axons (" << neuron->Axons().size() << "):" << std::endl;

	First = true;
	for ( auto a = neuron->Axons().begin(), a_end = neuron->Axons().end(); a != a_end; ++a )
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

	if ( !neuron->Axons().empty() )
		std::cout << std::endl;
}


void PrintNeuron( LabeledNeuron::Ptr neuron, unsigned int indent_spaces = 2 )
{
	if ( !neuron )
		throw std::runtime_error( "PrintNeuron(): No neuron provided." );

	std::string Indent("");
	Indent.resize( indent_spaces, ' ' );

	std::cout << Indent << "Neuron: " << neuron->Label() << std::endl;

	PrintNeuron( std::dynamic_pointer_cast< Neuron >(neuron), indent_spaces + 2 );
}


void PrintGanglion( Ganglion::Ptr network, unsigned int indent_spaces = 2 )
{
	if ( !network )
		throw std::runtime_error( "PrintGanglion(): No network provided." );

	std::string Indent("");
	Indent.resize( indent_spaces, ' ' );

	std::cout << Indent << "---------------------------------------" << std::endl;
	std::cout << Indent << "-- Inputs --" << std::endl;

	for ( auto i = network->Inputs().begin(), i_end = network->Inputs().end(); i != i_end; ++i )
		PrintNeuron( i->second, indent_spaces + 2 );

	std::cout << Indent << "-- Hidden Layers --" << std::endl;

	unsigned int NumLayers = 0;
	for ( auto l = network->Hidden().begin(), l_end = network->Hidden().end(); l != l_end; ++l )
	{
		++NumLayers;

		std::cout << Indent << "  -- Hidden " << NumLayers << " --" << std::endl;

		for ( auto h = l->second.begin(), h_end = l->second.end(); h != h_end; ++h )
			PrintNeuron( h->second, indent_spaces + 4 );
	}


	std::cout << Indent << "-- Outputs --" << std::endl;

	for ( auto o = network->Outputs().begin(), o_end = network->Outputs().end(); o != o_end; ++o )
		PrintNeuron( o->second, indent_spaces + 2 );

	std::cout << Indent << "---------------------------------------" << std::endl;
}




int main( int argc, char *argv[] )
{
    try
    {
		// Here we can use our custom Ganglion created above...
//		Ganglion::Ptr test = std::make_shared< Ganglion >();

		// Or the "pre-built" feedforward network
		Feedforward::Ptr test = std::make_shared< Feedforward >();

		// Create the structure for a simple network capable of solving XOR
		test->AddInput( "Input 1" );
		test->AddInput( "Input 2" );
		test->AddHiddenLayer( 4 );
		test->AddHiddenLayer( 3 );
		test->AddOutput( "Output" );
		test->ConnectNetwork();

		std::cout << "The initial network state:" << std::endl;
		PrintGanglion( test );
		std::cout << "\n\n" << std::endl;

		// Training of the network goes here...

		// Once trained, the network can be used by setting the inputs...
		double Input1 = -1.8, Input2 = 5.3;
		std::cout << "Setting input 1 to: " << Input1 << std::endl;
		std::cout << "Setting input 2 to: " << Input2 << std::endl;
		test->SetInput( "Input 1", Input1 );
		test->SetInput( "Input 2", Input2  );

		// ...and processing the network
		std::cout << "Processing..." << std::endl;
		test->Process();

		// So we can see the output
		std::cout << "Output: " << test->GetOutput( "Output" ) << "\n\n" << std::endl;

		std::cout << "The final state of the network:" << std::endl;
		PrintGanglion( test );
		//std::cout << "\n\n" << std::endl;
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


