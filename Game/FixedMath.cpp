#include "Engine.h"
#include "Defines.h"
#include "FixedMath.h"
#include "TrigLUT.h"

#ifndef USE_8BIT_ANGLE
#include <math.h>
#endif

fixed_t FixedMath::Sin(angle_t x)
{
#ifdef USE_8BIT_ANGLE
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
#else
	x = x & (DEGREES_360 - 1);
	return (fixed_t)(sin((x * 3.141592654) / DEGREES_180) * FIXED_ONE + 0.5);
#endif
}

fixed_t FixedMath::SinLong(angle_t x)
{
#ifdef USE_8BIT_ANGLE
	return Sin(x) * 8;
#else
	x = x & (DEGREES_360 - 1);
	return (fixed_t)(sin((x * 3.141592654) / DEGREES_180) * 8 * FIXED_ONE + 0.5);
#endif
}

int8_t clamp(int8_t x, int8_t lower, int8_t upper)
{
	if(x < lower) return lower;
	if(x > upper) return upper;
	return x;
}

uint8_t getRandomNumber()
{
	return getRandomNumber16() & 0xff;
}

uint16_t getRandomNumber16()
{
	static uint16_t randVal = 0xABC;

    uint16_t lsb = randVal & 1;
    randVal >>= 1;
    if (lsb == 1)
    	randVal ^= 0xB400u;

	return (randVal - 1);
}
