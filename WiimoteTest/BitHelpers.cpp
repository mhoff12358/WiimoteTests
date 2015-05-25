#include "BitHelpers.h"

void UnpackAccelerationCalibration(unsigned char* calibration_data, int* result) {
	result[0] = (calibration_data[0] << 2) + ((calibration_data[3] & 0x30) >> 4);
	result[1] = (calibration_data[1] << 2) + ((calibration_data[3] & 0x0C) >> 2);
	result[2] = (calibration_data[2] << 2) + ((calibration_data[3] & 0x03));
}