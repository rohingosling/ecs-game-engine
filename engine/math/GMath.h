//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines inline mathematical utility functions for the game engine, including value clamping, linear
//   interpolation, and random number generation for both floating-point and integer types.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <random>

//---------------------------------------------------------------------------------------------------------------------
// Namespace: engine
//
// Description:
//
//   Core namespace for the game engine framework. 
//
//   Contains math utilities, platform abstractions, resource management, and application infrastructure used to build 
//   game applications on top of the ECS layer.
//
//---------------------------------------------------------------------------------------------------------------------

namespace engine
{
	//-----------------------------------------------------------------------------------------------------------------
	// Method: clamp
	//
	// Description:
	//
	//   Restrict a value to lie within a specified minimum and maximum range. 
	//
	//   If the value falls outside the range, it is clamped to the nearest boundary.
	//
	// Arguments:
	//
	//   value (double):
	//     The input value to be clamped.
	//
	//   minVal (double):
	//     The lower bound of the clamping range.
	//
	//   maxVal (double):
	//     The upper bound of the clamping range.
	//
	// Returns:
	//
	//   The clamped value, guaranteed to be within the range [minVal, maxVal].
	//
	//-----------------------------------------------------------------------------------------------------------------

	inline double clamp ( double value, double minVal, double maxVal )
	{
		return std::max ( minVal, std::min ( value, maxVal ) );
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: lerp
	//
	// Description:
	//
	//   Perform lerp (linear interpolation) between two values using an interpolation factor. 
	//
	//   When t is 0 the result is a, when t is 1 the result is b, and intermediate values of t blend smoothly between 
	//   the two endpoints.
	//
	// Arguments:
	//
	//   a (double):
	//     The start value of the interpolation range.
	//
	//   b (double):
	//     The end value of the interpolation range.
	//
	//   t (double):
	//     The interpolation factor, typically in the range [0, 1].
	//
	// Returns:
	//
	//   The interpolated value between a and b at parameter t.
	//
	//-----------------------------------------------------------------------------------------------------------------

	inline double lerp ( double a, double b, double t )
	{
		return a + t * ( b - a );
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: randomInRange
	//
	// Description:
	//
	//   Generate a uniformly distributed random floating-point number within the specified range. 
	//
	//   Uses a thread-local Mersenne Twister engine seeded from the system random device.
	//
	// Arguments:
	//
	//   minVal (double):
	//     The inclusive lower bound of the random range.
	//
	//   maxVal (double):
	//     The inclusive upper bound of the random range.
	//
	// Returns:
	//
	//   A uniformly distributed random double in the range [minVal, maxVal].
	//
	//-----------------------------------------------------------------------------------------------------------------

	inline double randomInRange ( double minVal, double maxVal )
	{
		// Initialize a thread-local Mersenne Twister PRNG, seeded once per thread from the system's random device.

		static thread_local std::mt19937 gen
		{
			std::random_device{}( )
		};

		// Create a uniform real distribution over the closed interval [minVal, maxVal].

		std::uniform_real_distribution <double> dist ( minVal, maxVal );

		// Generate and return a random double drawn from the distribution.

		return dist ( gen );
	}

	//-----------------------------------------------------------------------------------------------------------------
	// Method: randomIntInRange
	//
	// Description:
	//
	//   Generate a uniformly distributed random integer within the specified inclusive range.
	//
	//   - Uses a thread-local Mersenne Twister engine seeded from the system random device.
	//
	//   - Mersenne Twister is a widely used pseudorandom number generator known for its long period and good
	//     statistical properties.
	//
	// Arguments:
	//
	//   minVal (int):
	//     The inclusive lower bound of the random integer range.
	//
	//   maxVal (int):
	//     The inclusive upper bound of the random integer range.
	//
	// Returns:
	//
	//   A uniformly distributed random integer in the range [minVal, maxVal].
	//
	//-----------------------------------------------------------------------------------------------------------------

	inline int randomIntInRange ( int minVal, int maxVal )
	{
		// Initialize a thread-local Mersenne Twister PRNG, seeded once per thread from the system's random device.

		static thread_local std::mt19937 gen
		{
			std::random_device{}( )
		};

		// Create a uniform integer distribution over the closed interval [minVal, maxVal].

		std::uniform_int_distribution <int> dist ( minVal, maxVal );

		// Generate and return a random integer drawn from the distribution.

		return dist ( gen );
	}
}
