#include "stdafx.h"

/**
 * calibration_data should be data retrieved from the wiimote for calibration.
 * The format is XX YY ZZ LL where XX, YY, and ZZ are the high 8 bits of a 10
 * bit unsigned number. The lower two bits are all stored in LL in the format
 * __XXYYZZ.
 * Result is an array of 3 integers that is used as output, and should
 * correspond to X, Y, Z.
 */
void UnpackLinearAccelerationReadingCalibration(unsigned char* calibration_data, int* result);
