#pragma once

struct OutputReportBuffer {
	unsigned char buffer[22];
};

class OutputReport
{
public:
	OutputReport(unsigned char* raw_buffer, bool pass_ownership);
	~OutputReport();

	unsigned char* GetBuffer() const;

private:
	unsigned char* buffer;
};

OutputReportBuffer ConstructMemoryWrite(bool use_registers, unsigned int offset, unsigned int size, unsigned char* data);
OutputReportBuffer ConstructMemoryWrite(bool use_registers, unsigned int offset, unsigned char single_data_byte);

OutputReportBuffer ConstructMemoryRead(bool use_registers, unsigned int offset, unsigned int size);

namespace OutputReportTemplates {
	extern unsigned char status_request[22];
	extern unsigned char set_leds[22];
	extern unsigned char core_buttons[22];
	extern unsigned char acc_ir_ext[22];
	extern unsigned char ir_enable_1[22];
	extern unsigned char ir_enable_2[22];

	extern unsigned char request_calibration[22];

	extern unsigned char sensitivity_block1[9];
	extern unsigned char sensitivity_block2[2];
}