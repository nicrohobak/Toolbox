#ifndef TOOLBOX_NEURALNETWORK_FEEDFORWARD
#define TOOLBOX_NEURALNETWORK_FEEDFORWARD

/*
 * Toolbox/NeuralNetwork/Feedforward.hpp
 *
 * A basic, fully-connected, feed-forward neural network.
 */


/****************************************************************************
 * Notes:
 *
 * - A standard multi-layer, feed-forward network is the default operating
 *   mode for a ganglion, so implementation here is simple.
 *
 ****************************************************************************
 * TODO:
 *
 * - Add backprop training functions/interface
 *
 ****************************************************************************
	typedef Toolbox::NeuralNetwork::Feedforward		Feedforward; // a.k.a tFeedforward< tNeuron<tNucleus<double>> >

	// Generate a quick XOR-capable network
	Feedforward MyFeedforward;
	MyFeedforward->AddInput( "Input 1" );
	MyFeedforward->AddInput( "Input 2" );
	MyFeedforward->AddHiddenLayer( 4 );
	MyFeedforward->AddHiddenLayer( 3 );
	MyFeedforward->AddOutput( "Output" );
	MyFeedforward->ConnectNetwork();

	// ... Train the network here ...

	// Once trained, execute by setting the input values
	MyFeedforward->SetInput( "Input 1", 1.0 );
	MyFeedforward->SetInput( "Input 2", 0.0 );

	// And then processing the network
	MyFeedforward->Process();

	// Finally, get our output from the network
	double Output = MyFeedforward->GetOutput( "Output" );

 ****************************************************************************/
/****************************************************************************/


#include <Toolbox/NeuralNetwork/Ganglion.hpp>


namespace Toolbox
{
	namespace NeuralNetwork
	{
		typedef Ganglion		Feedforward;
	}
}


#endif // TOOLBOX_NEURALNETWORK_FEEDFORWARD


// vim: tabstop=4 shiftwidth=4



