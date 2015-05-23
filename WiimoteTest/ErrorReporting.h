#pragma once

#include "stdafx.h"
#include <iostream>

bool reportLastError(wchar_t* prepend_msg, DWORD expected_result);

bool reportLastError(wchar_t* prepend_msg);

std::string reportHIDStatus(NTSTATUS instatus);