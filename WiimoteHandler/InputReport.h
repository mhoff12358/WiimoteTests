#pragma once

#include "stdafx.h"
#include "WiimoteEnums.h"
#include "LinearAccelerationReading.h"
#include "IRData.h"
#include "ButtonState.h"
#include "NunchuckState.h"
#include "MotionPlusState.h"
#include <cstring>
#include <iostream>
#include <vector>


class InputReport {
public:
	InputReport(unsigned char* raw_buffer, unsigned int raw_buffer_length, bool pass_ownership);
	~InputReport();

	unsigned char* GetBuffer() const;

	LinearAccelerationReading GetLinearAccelerationReading() const;
	IRData GetIRData() const;
	ButtonState GetButtonState() const;
	NunchuckState GetNunchuckState() const;
	MotionPlusState GetMotionPlusState() const;

	unsigned char GetInputReportType() const;

	// Memory request helpers
	unsigned int GetDataSize() const;
	unsigned int GetDataError() const;
	unsigned int GetDataOffset() const;
	unsigned char* GetDataPacket(bool request_ownership) const;

	// Acknowledge helpers
	unsigned char GetAckOutputReportCode() const;
	unsigned char GetAckError() const;

	void DumpToStdout() const;

private:
	unsigned char* buffer;
	unsigned int buffer_length;
};