#pragma once

#include "stdafx.h"
#include <cstring>
#include <iostream>

class InputReport {
public:
	InputReport(unsigned char* raw_buffer, unsigned int raw_buffer_length, bool pass_ownership);

	void DumpToStdout() const;

private:
	unsigned char* buffer;
	unsigned int buffer_length;
};