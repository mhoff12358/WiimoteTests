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

void InputReport::DumpToStdout() const {
	std::cout << std::hex;
	for (int i = 0; i < buffer_length; i++) {
		std::cout << std::setfill('0') << std::setw(2) << (unsigned int)buffer[i] << " ";
	}
	std::cout << std::endl;
	std::cout << std::dec;
}