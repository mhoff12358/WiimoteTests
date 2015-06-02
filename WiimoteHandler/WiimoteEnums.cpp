#include "WiimoteEnums.h"

int GetButtonOffset(InputReportType report_type) {
	switch (report_type) {
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
	case DATA_BUT_ACC_IR10_EXT6:
	case DATA_INTERLEAVE_1:
	case DATA_INTERLEAVE_2:
	case DATA_INTERLEAVE_BOTH:
		return 1;
	default:
		return -1;
	}
}

int GetLinearAccelerationReadingOffset(InputReportType report_type) {
	switch (report_type) {
	case DATA_BUT_ACC:
	case DATA_BUT_ACC_IR12:
	case DATA_BUT_ACC_EXT16:
	case DATA_BUT_ACC_IR10_EXT6:
		return 3;
	default:
		return -1;
	}
}

int GetExtensionOffset(InputReportType report_type) {
	switch (report_type) {
	case DATA_EXT21:
		return 1;
	case DATA_BUT_EXT8:
	case DATA_BUT_EXT19:
		return 3;
	case DATA_BUT_ACC_EXT16:
		return 6;
	case DATA_BUT_IR10_EXT9:
		return 13;
	case DATA_BUT_ACC_IR10_EXT6:
		return 16;
	default:
		return -1;
	}
}

int GetIROffset(InputReportType report_type) {
	switch (report_type) {
	case DATA_BUT_ACC_IR12:
		return 6;
	case DATA_BUT_IR10_EXT9:
		return 3;
	case DATA_BUT_ACC_IR10_EXT6:
		return 6;
	default:
		return -1;
	}
}
