//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the Vector2D struct, a two-dimensional vector with double-precision components.
//
//   Provides arithmetic operators, dot product, length, and normalization utilities for 2D math operations.
//
// TODO:
//
//   1. None.
//
//---------------------------------------------------------------------------------------------------------------------

#pragma once

#include <cmath>

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
	//*****************************************************************************************************************
	// Struct: Vector2D
	//
	// Description:
	//
	//   A two-dimensional vector with double-precision x and y components.
	//
	//   Supports element-wise arithmetic via overloaded operators, dot product computation, magnitude calculation,
	//   and unit-vector normalization.
	//
	//*****************************************************************************************************************

	struct Vector2D
	{
		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		double x = 0.0;
		double y = 0.0;

		//=============================================================================================================
		// Constructors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Constructor 1/2: Vector2D
		//
		// Description:
		//
		//   Default constructor. Initializes both components to zero.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector2D () = default;

		//-------------------------------------------------------------------------------------------------------------
		// Constructor 2/2: Vector2D
		//
		// Description:
		//
		//   Construct a Vector2D with the specified x and y component values.
		//
		// Arguments:
		//
		//   x (double):
		//     The x component of the vector.
		//
		//   y (double):
		//     The y component of the vector.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector2D ( double x, double y ) : x ( x ), y ( y ) {}

		//=============================================================================================================
		// Operators
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: operator+
		//
		// Description:
		//
		//   Compute the element-wise sum of this vector and another Vector2D.
		//
		// Arguments:
		//
		//   rhs (const Vector2D&):
		//     The right-hand operand to add.
		//
		// Returns:
		//
		//   A new Vector2D containing the component-wise sum.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector2D  operator+ ( const Vector2D& rhs ) const
		{
			// Return a new vector whose components are the pairwise sums of this vector and the right-hand operand.

			return { x + rhs.x, y + rhs.y };
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: operator-
		//
		// Description:
		//
		//   Compute the element-wise difference of this vector and another Vector2D.
		//
		// Arguments:
		//
		//   rhs (const Vector2D&):
		//     The right-hand operand to subtract.
		//
		// Returns:
		//
		//   A new Vector2D containing the component-wise difference.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector2D  operator- ( const Vector2D& rhs ) const
		{
			// Return a new vector whose components are the pairwise differences of this vector and the right-hand operand.

			return { x - rhs.x, y - rhs.y };
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: operator*
		//
		// Description:
		//
		//   Compute the scalar multiplication of this vector by a double value.
		//
		// Arguments:
		//
		//   s (double):
		//     The scalar multiplier.
		//
		// Returns:
		//
		//   A new Vector2D with each component multiplied by s.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector2D  operator* ( double s ) const
		{
			// Return a new vector with each component scaled by the given scalar multiplier.

			return { x * s, y * s };
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: operator/
		//
		// Description:
		//
		//   Compute the scalar division of this vector by a double value.
		//
		// Arguments:
		//
		//   s (double):
		//     The scalar divisor.
		//
		// Returns:
		//
		//   A new Vector2D with each component divided by s.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector2D  operator/ ( double s ) const
		{
			// Return a new vector with each component divided by the given scalar divisor.

			return { x / s, y / s };
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: operator+=
		//
		// Description:
		//
		//   Add another Vector2D to this vector in place, modifying both components.
		//
		// Arguments:
		//
		//   rhs (const Vector2D&):
		//     The right-hand operand to add.
		//
		// Returns:
		//
		//   A reference to this vector after the addition.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector2D& operator+=( const Vector2D& rhs )
		{
			// Add the right-hand operand's components to this vector's components in place.

			x += rhs.x;
			y += rhs.y;

			// Return a reference to this vector so that compound assignment expressions can be chained.

			return *this;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: operator-=
		//
		// Description:
		//
		//   Subtract another Vector2D from this vector in place, modifying both components.
		//
		// Arguments:
		//
		//   rhs (const Vector2D&):
		//     The right-hand operand to subtract.
		//
		// Returns:
		//
		//   A reference to this vector after the subtraction.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector2D& operator-=( const Vector2D& rhs )
		{
			x -= rhs.x;
			y -= rhs.y;

			return *this;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: operator*=
		//
		// Description:
		//
		//   Multiply this vector by a scalar in place, scaling both components.
		//
		// Arguments:
		//
		//   s (double):
		//     The scalar multiplier.
		//
		// Returns:
		//
		//   A reference to this vector after the scaling.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector2D& operator*=( double s )
		{
			x *= s;
			y *= s;

			return *this;
		}

		//=============================================================================================================
		// Accessors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: dot
		//
		// Description:
		//
		//   Compute the dot product of this vector and another Vector2D.
		//
		// Arguments:
		//
		//   rhs (const Vector2D&):
		//     The right-hand operand for the dot product.
		//
		// Returns:
		//
		//   The scalar dot product (x*rhs.x + y*rhs.y).
		//
		//-------------------------------------------------------------------------------------------------------------

		double dot ( const Vector2D& rhs )  const
		{
			return x * rhs.x + y * rhs.y;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: lengthSq
		//
		// Description:
		//
		//   Compute the squared magnitude of this vector, avoiding the cost of a square root operation.
		//
		// Returns:
		//
		//   The squared length of the vector (x*x + y*y).
		//
		//-------------------------------------------------------------------------------------------------------------

		double lengthSq () const
		{
			return x * x + y * y;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Value Accessor: length
		//
		// Description:
		//
		//   Compute the Euclidean magnitude of this vector.
		//
		// Returns:
		//
		//   The length of the vector as a double.
		//
		//-------------------------------------------------------------------------------------------------------------

		double length () const
		{
			return std::sqrt ( lengthSq () );
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: normalized
		//
		// Description:
		//
		//   Return a unit-length copy of this vector.
		//
		//   If the vector has zero length, a zero vector is returned.
		//
		// Returns:
		//
		//   A new Vector2D of unit length pointing in the same direction, or a zero vector if length is zero.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector2D normalized () const
		{
			// Compute the magnitude and, if non-zero, return a unit vector by dividing each component by the length.

			double len = length ();
			if ( len > 0.0 ) return { x / len, y / len };

			// The vector has zero length, so normalization is undefined. Return a zero vector as a safe fallback.

			return { 0.0, 0.0 };
		}
	};
}
