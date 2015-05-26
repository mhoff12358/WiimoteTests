#pragma once

enum InputReportType : unsigned char {
	STATUS = 0x20,
	READ_MEM_AND_REG = 0x21,
	ACK_OUTPUT_REPORT = 0x22,
	DATA_BUT = 0x30,
	DATA_BUT_ACC = 0x31,
	DATA_BUT_EXT8 = 0x32,
	DATA_BUT_ACC_IR12 = 0x33,
	DATA_BUT_EXT19 = 0x34,
	DATA_BUT_ACC_EXT16 = 0x35,
	DATA_BUT_IR10_EXT9 = 0x36,
	DATA_BUT_ACC_IR10_EXT6 = 0x37,
	DATA_EXT21 = 0x3d,
	DATA_INTERLEAVE_1 = 0x3e,
	DATA_INTERLEAVE_2 = 0x3f,
	DATA_INTERLEAVE_BOTH = 0x40,

	// These aren't input reports, but they're other types of data reports like
	// extension controller data.
	NUNCHUNK = 0x50,
	NUNCHUCK_INTERLEAVE = 0x51,
	MOTION_PLUS = 0x52,
	MOTION_PLUS_INTERLEAVE = 0x53,
};

int GetButtonOffset(InputReportType report_type);
int GetAccelerationOffset(InputReportType report_type);
int GetExtensionOffset(InputReportType report_type);
int GetIROffset(InputReportType report_type);

enum ButtonMask : unsigned short {
	LEFT_MASK = 0x0001,
	RIGHT_MASK = 0x0002,
	DOWN_MASK = 0x0004,
	UP_MASK = 0x0008,
	PLUS_MASK = 0x0010,
	TWO_MASK = 0x0100,
	ONE_MASK = 0x0200,
	B_MASK = 0x0400,
	A_MASK = 0x0800,
	MINUS_MASK = 0x1000,
	HOME_MASK = 0x8000,
};