#pragma once

#include "WiimoteEnums.h"
#include <vector>

class ButtonState
{
public:
	ButtonState(unsigned char* buffer);
	ButtonState();

	bool GetButtonPressed(ButtonMask button) const;
	std::vector<bool> GetButtonsPressed(const std::vector<ButtonMask>& buttons) const;
	
private:
	unsigned char button_data[2];
};

