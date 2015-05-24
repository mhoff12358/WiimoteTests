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

InputReport::~InputReport() {
	delete[] buffer;
}

unsigned char* InputReport::GetBuffer() const {
	return buffer;
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

unsigned int InputReport::GetDataSize() const {
	return (buffer[3] & 0xF0) >> 4;
}

unsigned int InputReport::GetDataError() const {
	return (buffer[3] & 0x0F);
}

unsigned int InputReport::GetDataOffset() const {
	return (buffer[4] << 8) + (buffer[5]);
}

unsigned char* InputReport::GetDataPacket(bool request_ownership) const {
	if (!request_ownership) {
		return buffer + 6;
	}
	unsigned char* new_buffer = new unsigned char[16];
	memcpy(new_buffer, buffer + 6, 16);
	return new_buffer;
}

unsigned char InputReport::GetAckOutputReportCode() const {
	return buffer[3];
}

unsigned char InputReport::GetAckError() const {
	return buffer[4];
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