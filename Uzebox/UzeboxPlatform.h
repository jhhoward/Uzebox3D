#ifndef UZEBOX_PLATFORM_H_
#define UZEBOX_PLATFORM_H_

#include <avr/pgmspace.h>
extern "C" {
#include "Uzebox.h"
}
#include "Platform.h"


class UzeboxPlatform : public PlatformBase
{
public:
	void playSound(uint8_t id) {}

	void update();
};

extern UzeboxPlatform Platform;

#endif
