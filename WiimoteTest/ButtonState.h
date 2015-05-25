#pragma once
class ButtonState
{
public:
	ButtonState(unsigned char* state_buffer);

	unsigned char* GetState();
	
private:
	unsigned char buffer[2];
};

