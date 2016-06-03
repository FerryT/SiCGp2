/*************************************************************
 * Vector structure -- See header file for more information. *
 *************************************************************/

#include <math.h>

#include "Vector.h"

namespace Base {

//------------------------------------------------------------------------------

unit Vec2d::length() const
{
	return sqrt(x * x + y * y);
}

//------------------------------------------------------------------------------

unit Vec2d::angle() const
{
	return atan2(y, x);
}

//------------------------------------------------------------------------------

} /* namespace Base */

//------------------------------------------------------------------------------
