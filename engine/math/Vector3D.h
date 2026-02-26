
//---------------------------------------------------------------------------------------------------------------------
// Project: ECS (Entity Component System) Game Engine
// Version: 2.1 (C++17 Upgrade)
// Date:    2013
// Author:  Rohin Gosling
//
// Description:
//
//   Defines the Vector3D struct, a three-dimensional vector with double-precision components.
//
//   Provides arithmetic operators, dot product, and length utilities for 3D math operations.
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
	// Struct: Vector3D
	//
	// Description:
	//
	//   A three-dimensional vector with double-precision x, y, and z components.
	//
	//   Supports element-wise arithmetic via overloaded operators, dot product computation, and magnitude calculation.
	//
	//*****************************************************************************************************************

	struct Vector3D
	{
		//=============================================================================================================
		// Data Members
		//=============================================================================================================

		double x = 0.0;
		double y = 0.0;
		double z = 0.0;

		//=============================================================================================================
		// Constructors
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Constructor 1/2: Vector3D
		//
		// Description:
		//
		//   Default constructor. Initializes all three components to zero.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector3D () = default;

		//-------------------------------------------------------------------------------------------------------------
		// Constructor 2/2: Vector3D
		//
		// Description:
		//
		//   Construct a Vector3D with the specified x, y, and z component values.
		//
		// Arguments:
		//
		//   x (double):
		//     The x component of the vector.
		//
		//   y (double):
		//     The y component of the vector.
		//
		//   z (double):
		//     The z component of the vector.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector3D ( double x, double y, double z ) : x ( x ), y ( y ), z ( z ) {}

		//=============================================================================================================
		// Operators
		//=============================================================================================================

		//-------------------------------------------------------------------------------------------------------------
		// Method: operator+
		//
		// Description:
		//
		//   Compute the element-wise sum of this vector and another Vector3D.
		//
		// Arguments:
		//
		//   rhs (const Vector3D&):
		//     The right-hand operand to add.
		//
		// Returns:
		//
		//   A new Vector3D containing the component-wise sum.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector3D  operator+ ( const Vector3D& rhs ) const {
			return { x + rhs.x, y + rhs.y, z + rhs.z };
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: operator-
		//
		// Description:
		//
		//   Compute the element-wise difference of this vector and another Vector3D.
		//
		// Arguments:
		//
		//   rhs (const Vector3D&):
		//     The right-hand operand to subtract.
		//
		// Returns:
		//
		//   A new Vector3D containing the component-wise difference.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector3D  operator- ( const Vector3D& rhs ) const {
			return { x - rhs.x, y - rhs.y, z - rhs.z };
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
		//   A new Vector3D with each component multiplied by s.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector3D  operator* ( double s )        const {
			return { x * s,     y * s,     z * s };
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: operator+=
		//
		// Description:
		//
		//   Add another Vector3D to this vector in place, modifying all three components.
		//
		// Arguments:
		//
		//   rhs (const Vector3D&):
		//     The right-hand operand to add.
		//
		// Returns:
		//
		//   A reference to this vector after the addition.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector3D& operator+=( const Vector3D& rhs )
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;

			return *this;
		}

		//-------------------------------------------------------------------------------------------------------------
		// Method: operator-=
		//
		// Description:
		//
		//   Subtract another Vector3D from this vector in place, modifying all three components.
		//
		// Arguments:
		//
		//   rhs (const Vector3D&):
		//     The right-hand operand to subtract.
		//
		// Returns:
		//
		//   A reference to this vector after the subtraction.
		//
		//-------------------------------------------------------------------------------------------------------------

		Vector3D& operator-=( const Vector3D& rhs )
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;

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
		//   Compute the dot product of this vector and another Vector3D.
		//
		// Arguments:
		//
		//   rhs (const Vector3D&):
		//     The right-hand operand for the dot product.
		//
		// Returns:
		//
		//   The scalar dot product (x*rhs.x + y*rhs.y + z*rhs.z).
		//
		//-------------------------------------------------------------------------------------------------------------

		double dot ( const Vector3D& rhs )  const {
			return x * rhs.x + y * rhs.y + z * rhs.z;
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
		//   The squared length of the vector (x*x + y*y + z*z).
		//
		//-------------------------------------------------------------------------------------------------------------

		double lengthSq ()            const {
			return x * x + y * y + z * z;
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

		double length ()              const {
			return std::sqrt ( lengthSq () );
		}

		//=============================================================================================================
		// Mutators
		//=============================================================================================================

		// None

		//=============================================================================================================
		// Methods
		//=============================================================================================================

		// None
	};
}
