#include "stdafx.h"
#include "ButtonState.h"


ButtonState::ButtonState(unsigned char* state_buffer)
{
	memcpy(buffer, state_buffer, 2);
}

unsigned char* ButtonState::GetState() {
	return buffer;
}