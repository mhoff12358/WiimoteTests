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

LinearAccelerationReading InputReport::GetLinearAccelerationReading() const {
	return LinearAccelerationReading(buffer[0], buffer);
}

IRData InputReport::GetIRData() const {
	return IRData(buffer);
}

ButtonState InputReport::GetButtonState() const {
	return ButtonState(buffer);
}

NunchuckState InputReport::GetNunchuckState() const {
	return NunchuckState(NUNCHUNK, buffer+GetExtensionOffset((InputReportType)buffer[0]));
}

MotionPlusState InputReport::GetMotionPlusState() const {
	return MotionPlusState(MOTION_PLUS, buffer + GetExtensionOffset((InputReportType)buffer[0]));
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
	//std::cout << GetButtonPressed(A_MASK) << "\t" << GetButtonPressed(B_MASK) << std::endl;
	//LinearAccelerationReading acc = GetLinearAccelerationReading();
	//std::cout << "LinearAccelerationReading: " << acc.acceleration[0] << "\t" << acc.acceleration[1] << "\t" << acc.acceleration[2] << std::endl;
}