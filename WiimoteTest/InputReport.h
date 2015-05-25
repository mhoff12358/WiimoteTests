#pragma once

#include "stdafx.h"
#include "WiimoteEnums.h"
#include "Acceleration.h"
#include "IRData.h"
#include "ButtonState.h"
#include "ExtensionState.h"
#include <cstring>
#include <iostream>
#include <vector>


class InputReport {
public:
	InputReport(unsigned char* raw_buffer, unsigned int raw_buffer_length, bool pass_ownership);
	~InputReport();

	unsigned char* GetBuffer() const;

	Acceleration GetAcceleration() const;
	IRData GetIRData() const;
	ButtonState GetButtonState() const;
	ExtensionState GetExtensionState() const;

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