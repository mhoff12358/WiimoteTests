#include "stdafx.h"
#include "OutputReport.h"


OutputReport::OutputReport(unsigned char* raw_buffer, bool pass_ownership)
{
	if (pass_ownership) {
		buffer = raw_buffer;
	} else {
		buffer = new unsigned char[22];
		memcpy(buffer, raw_buffer, 22);
	}
}


OutputReport::~OutputReport()
{
}

unsigned char* OutputReport::GetBuffer() const {
	return buffer;
}


OutputReportBuffer ConstructMemoryWrite(bool use_registers, unsigned int offset, unsigned int size, unsigned char* data) {
	OutputReportBuffer result;
	ZeroMemory(result.buffer, 22);
	result.buffer[0] = 0x16; // Identify as a mem write
	if (use_registers) {
		result.buffer[1] |= 0x04; // Specify registers not memory
	}
	// Only copy the 3 least significant bytes from the offset
	unsigned int net_offset = htonl(offset);
	unsigned int net_size = htonl(size);
	unsigned char a, b, c, d;
	memcpy(result.buffer + 2, reinterpret_cast<unsigned char*>(&net_offset) + 1, 3);
	// Only copy the 1 least significan byte from the size
	memcpy(result.buffer + 5, reinterpret_cast<unsigned char*>(&net_size) + 3, 1);
	// Copy the data into the buffer, throwing out data after the first 16 bytes
	memcpy(result.buffer + 6, data, min(16, size));
	return result;
}

OutputReportBuffer ConstructMemoryWrite(bool use_registers, unsigned int offset, unsigned char single_data_byte) {
	return ConstructMemoryWrite(use_registers, offset, 1, &single_data_byte);
}

namespace OutputReportTemplates {
	unsigned char status_request[22] = { 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char set_leds[22] = { 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char core_buttons[22] = { 0x12, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char acc_ir_ext[22] = { 0x12, 0x00, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char ir_enable_1[22] = { 0x13, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	unsigned char ir_enable_2[22] = { 0x1a, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	unsigned char request_calibration[22] = { 0x17, 0x00, 0x00, 0x00, 0x16, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	unsigned char sensitivity_block1[9] = { 0x02, 0x00, 0x00, 0x71, 0x01, 0x00, 0xaa, 0x00, 0x64 };
	unsigned char sensitivity_block2[2] = { 0x63, 0x03 };
}