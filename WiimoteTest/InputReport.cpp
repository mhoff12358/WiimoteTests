#include "InputReport.h"

InputReport::InputReport(unsigned char* raw_buffer, unsigned int raw_buffer_length, bool pass_ownership) {
	buffer_length = raw_buffer_length;
	if (pass_ownership) {
		buffer = raw_buffer;
	} else {
		buffer = new unsigned char[buffer_length];
		memcpy(buffer, raw_buffer, buffer_length);
	}
}

bool InputReport::GetButtonPressed(ButtonMask button) const {
	return ((*reinterpret_cast<ButtonMask*>(buffer + 1)) & button) != 0;
}

std::vector<bool> InputReport::GetButtonsPressed(const std::vector<ButtonMask>& buttons) const {
	std::vector<bool> result;
	result.resize(buttons.size(), false);
	for (unsigned int i = 0; i < buttons.size(); i++) {
		result[i] = GetButtonPressed(buttons[i]);
	}
	return result;
}

Acceleration InputReport::GetAcceleration() const {
	return Acceleration(buffer);
}

void InputReport::DumpToStdout() const {
	std::cout << std::hex;
	for (unsigned int i = 0; i < buffer_length; i++) {
		std::cout << std::setfill('0') << std::setw(2) << (unsigned int)buffer[i] << " ";
	}
	std::cout << std::endl;
	std::cout << std::dec;
	std::cout << GetButtonPressed(A_MASK) << "\t" << GetButtonPressed(B_MASK) << std::endl;
	Acceleration acc = GetAcceleration();
	std::cout << "Acceleration: " << acc.X << "\t" << acc.Y << "\t" << acc.Z << std::endl;
}