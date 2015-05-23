#include "WiimoteHandler.h"

WiimoteHandler::WiimoteHandler() {

}

void WiimoteHandler::SetPipe(HANDLE bluetooth_pipe) {
	pipe = bluetooth_pipe;
	GatherData();
}

void WiimoteHandler::GatherData() {
	if (HidD_GetPreparsedData(pipe, &preparsed_data)) {
		NTSTATUS get_caps_err = HidP_GetCaps(preparsed_data, &capabilities);
		return ReportErrors("GETTING CAPABILITIES", get_caps_err);
	} else {
		return ReportErrors(L"GETTING PREPARSED DATA");
	}
}

void WiimoteHandler::ReportErrors(wchar_t* prepend_msg) {
	reportLastError(prepend_msg);
}

void WiimoteHandler::ReportErrors(std::string prepend_msg, NTSTATUS status) {
	if (status != HIDP_STATUS_SUCCESS) {
		std::cout << prepend_msg << ": " << reportHIDStatus(status) << std::endl;
	}
}

InputReport WiimoteHandler::GatherInputReport() {
	OVERLAPPED async_overlap;
	ZeroMemory(&async_overlap, sizeof(async_overlap));
	async_overlap.Offset = 0;
	async_overlap.OffsetHigh = 0;
	async_overlap.Pointer = 0;
	HANDLE readEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!readEvent) {
		std::cout << "EVENT CREATION ERROR" << std::endl;
	}
	reportLastError(L"CREATE WRITE EVENT");
	async_overlap.hEvent = readEvent;

	unsigned char* buffer = new unsigned char[capabilities.InputReportByteLength];
	ZeroMemory(buffer, capabilities.InputReportByteLength);
	
	ReadFile(pipe, buffer, capabilities.InputReportByteLength, NULL, &async_overlap);
	reportLastError(L"ATTEMPTING READ");
	WaitForSingleObject(readEvent, INFINITE);

	return InputReport(buffer, capabilities.InputReportByteLength, true);
}

void WiimoteHandler::HandleInputReport(const InputReport& report) {
	report.DumpToStdout();
}

void WiimoteHandler::WatchForInputReports() {
	while (true) {
		HandleInputReport(GatherInputReport());
	}
}

void WiimoteHandler::SendOutputReport(unsigned char* buffer) {
	HidD_SetOutputReport(pipe, buffer, capabilities.OutputReportByteLength);
}