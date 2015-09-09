#include "Defines.h"
#include "FixedMath.h"
#include "TrigLUT.h"

fixed_t FixedMath::Sin(angle_t x)
{
	if(x <= DEGREES_90)
	{
		return (fixed_t)(pgm_read_byte(&TrigLUT[x]));
	}
	else if(x <= DEGREES_180)
	{
		return (fixed_t)(pgm_read_byte(&TrigLUT[DEGREES_180 - x]));
	}
	else if(x <= DEGREES_270)
	{
		return -1 * (fixed_t)(pgm_read_byte(&TrigLUT[x - DEGREES_180]));
	}
	else
	{
		return -1 * (fixed_t)(pgm_read_byte(&TrigLUT[DEGREES_360 - x]));
	}
}

