#pragma once

#include "stdafx.h"
#include "WiimoteEnums.h"
#include "Acceleration.h"
#include <cstring>
#include <iostream>
#include <vector>


class InputReport {
public:
	InputReport(unsigned char* raw_buffer, unsigned int raw_buffer_length, bool pass_ownership);

	bool GetButtonPressed(ButtonMask button) const;
	std::vector<bool> GetButtonsPressed(const std::vector<ButtonMask>& buttons) const;

	Acceleration GetAcceleration() const;

	void DumpToStdout() const;

private:
	unsigned char* buffer;
	unsigned int buffer_length;
};