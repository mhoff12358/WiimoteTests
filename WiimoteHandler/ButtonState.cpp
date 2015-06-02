#include "stdafx.h"
#include "ButtonState.h"

ButtonState::ButtonState() {
	button_data[0] = 0;
	button_data[1] = 0;
}

ButtonState::ButtonState(unsigned char* buffer)
{
	switch (buffer[0]) {
		case STATUS:
		case READ_MEM_AND_REG:
		case ACK_OUTPUT_REPORT:
		case DATA_BUT:
		case DATA_BUT_ACC:
		case DATA_BUT_EXT8:
		case DATA_BUT_ACC_IR12:
		case DATA_BUT_EXT19:
		case DATA_BUT_ACC_EXT16:
		case DATA_BUT_IR10_EXT9:
		case DATA_BUT_ACC_IR10_EXT6: {
			memcpy(button_data, buffer+1, 2);
		}
	}
}

bool ButtonState::GetButtonPressed(ButtonMask button) const {
	return ((*reinterpret_cast<const ButtonMask*>(button_data)) & button) != 0;
}

std::vector<bool> ButtonState::GetButtonsPressed(const std::vector<ButtonMask>& buttons) const {
	std::vector<bool> result;
	result.resize(buttons.size(), false);
	for (unsigned int i = 0; i < buttons.size(); i++) {
		result[i] = GetButtonPressed(buttons[i]);
	}
	return result;
}