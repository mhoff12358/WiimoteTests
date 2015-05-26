#include "stdafx.h"
#include "NunchuckState.h"

NunchuckState::NunchuckState() : acceleration() {
	stick_position[0] = 0xFF;
	stick_position[1] = 0xFF;
	c_pressed = false;
	z_pressed = false;
}

NunchuckState::NunchuckState(unsigned char buffer_type, unsigned char* extension_buffer)
{
	acceleration = Acceleration(buffer_type, extension_buffer);
	memcpy(stick_position, extension_buffer, 2);
	c_pressed = (extension_buffer[5] & 0x02) == 0;
	z_pressed = (extension_buffer[5] & 0x01) == 0;
}


NunchuckState::~NunchuckState()
{
}
