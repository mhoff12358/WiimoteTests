#include "ErrorReporting.h"

bool reportLastError(wchar_t* prepend_msg, DWORD expected_result) {
	DWORD errcode = GetLastError();
	wchar_t msgout[512];
	if (errcode != expected_result) {
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			errcode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			msgout,
			511,
			NULL);
		wprintf(L"%s: %s", prepend_msg, msgout);
		return false;
	}
	return true;
}

bool reportLastError(wchar_t* prepend_msg) {
	return reportLastError(prepend_msg, 0);
}


std::string reportHIDStatus(NTSTATUS instatus) {
	switch (instatus) {
	case HIDP_STATUS_SUCCESS:
		return "HIDP_STATUS_SUCCESS";
	case HIDP_STATUS_INVALID_PREPARSED_DATA:
		return "HIDP_STATUS_INVALID_PREPARSED_DATA";
	case HIDP_STATUS_INVALID_REPORT_LENGTH:
		return "HIDP_STATUS_INVALID_REPORT_LENGTH";
	case HIDP_STATUS_INVALID_REPORT_TYPE:
		return "HIDP_STATUS_INVALID_REPORT_TYPE";
	case HIDP_STATUS_REPORT_DOES_NOT_EXIST:
		return "HIDP_STATUS_REPORT_DOES_NOT_EXIST";
	}
	return "UNKNOWN";
}