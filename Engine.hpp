#ifndef TOOLBOX_ENGINE_HPP
#define TOOLBOX_ENGINE_HPP

/*
 * Toolbox/Engine.hpp
 *
 * A simple, fixed-interval game-loop handler.
 */

/*****************************************************************************
 * Example Program:

	#include <iostream>
	#include <stdexcept>

	#include <Toolbox/Engine.hpp>


	class MyEngine : public Toolbox::Engine
	{
	public:
		virtual void Update( Toolbox::Engine::tTimeStep interval )
		{
			std::cout << "Update!  Interval: " << interval.count() << std::endl;

			if ( ++_UpdateCount >= 10 )
			{
				std::cout << "10 updates, exiting!" << std::endl;
				Stop();
			}
		}

		virtual void Interpolate( double value )
		{
			std::cout << "Interpolate!  Value: " << value << std::endl;
		}

		virtual void Render()
		{
			std::cout << "Render!" << std::endl;
		}

	public:
		MyEngine():
			_UpdateCount( 0 )
		{
		}

	protected:
		unsigned int	_UpdateCount;
	};


	int main( int argc, char *argv[] )
	{
		int ReturnCode = 0;

		try
		{
			MyEngine Engine;
			Engine.SetLogicFPS( 10 );
			Engine.SetRenderFPS( 30 );
			Engine.Start();
		}
		catch ( std::exception &ex )
		{
			std::cerr << "Fatal error: " << ex.what() << std::endl;
			ReturnCode = 1;
		}

		return ReturnCode;
	}

// Build with:
// g++ -std=c++14 -Wall -pedantic -o example
 ****************************************************************************/

/****************************************************************************/
/****************************************************************************/


#include <chrono>


namespace Toolbox
{
	class Engine
	{
	public:
		typedef std::chrono::high_resolution_clock	tClock;
		typedef std::chrono::nanoseconds			tTimeStep;

	public:
		virtual void Update( tTimeStep interval )
		{
			// Fixed intervals
			//   - Input
			//   - Game Update
		}

		virtual void Interpolate( double value )
		{
			// To assist with Render() calls between Update() calls
			// (CurState * value) + (PrevState * (1.0 - value))
		}

		virtual void Render()
		{
			// Output
		}

	public:
		Engine( unsigned int logicFPS = 30, unsigned int renderFPS = 60 ):
			_LogicTimeStep( 0 ),
			_RenderTimeStep( 0 ),
			_Running( false ),
			_LogicFPS( 0 ),
			_RenderFPS( 0 )
		{
			SetLogicFPS( logicFPS );
			SetRenderFPS( renderFPS );
		}

		virtual ~Engine()
		{
		}

		void Start()
		{
			_Running = true;
			loop();
		}

		void Stop()
		{
			_Running = false;
		}

		void SetLogicFPS( unsigned int FPS = 30 )
		{
			_LogicFPS = FPS;
			_LogicTimeStep = tTimeStep( std::chrono::milliseconds(1000 / _LogicFPS) );
		}

		void SetRenderFPS( unsigned int FPS = 60 )
		{
			_RenderFPS = FPS;
			_RenderTimeStep = tTimeStep( std::chrono::milliseconds(1000 / _RenderFPS) );
		}

	protected:
		bool			_Running;

		unsigned int	_LogicFPS;
		unsigned int	_RenderFPS;

		tTimeStep		_LogicTimeStep;
		tTimeStep		_RenderTimeStep;

	protected:
		void loop()
		{
			auto Now = tClock::now();
			auto Start = Now;
			tTimeStep LogicLag( 0 );
			tTimeStep RenderLag( 0 );

			while ( _Running )
			{
				Now = tClock::now();
				auto Delta = Now - Start;
				Start = Now;
				auto LagDelta = std::chrono::duration_cast< tTimeStep >( Delta );
				LogicLag += LagDelta;
				RenderLag += LagDelta;

				while ( LogicLag >= _LogicTimeStep )
				{
					LogicLag -= _LogicTimeStep;
					Update( _LogicTimeStep );
				}

				if ( RenderLag >= _RenderTimeStep )
				{
					Interpolate( static_cast<double>(LogicLag.count()) / static_cast<double>(_LogicTimeStep.count()) );
					Render();
					RenderLag -= _RenderTimeStep;
				}
			}
		}
	};
}


#endif // TOOLBOX_ENGINE_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=5 --style=ansi --indent-namespaces --indent-cases --pad-oper


