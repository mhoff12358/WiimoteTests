#pragma once

#include "stdafx.h"

#include "InputReport.h"
#include "ErrorReporting.h"

class WiimoteHandler {
public:
	WiimoteHandler();

	/**
	 * Takes in a file handle to the wiimote, it should already have been verified
	 * to be a wiimote connection
	 */
	void SetPipe(HANDLE bluetooth_pipe);

	/**
	 * Fills in the preparsed data and the device capabilities
	 */
	void GatherData();

	/**
	 * Reports any existing errors to stdout
	 */
	void ReportErrors(wchar_t* prepend_msg);
	void ReportErrors(std::string prepend_msg, NTSTATUS status);

	/**
	 * Loads a single input report from the wiimote, regardless of what type it is
	 */
	InputReport GatherInputReport();

	/**
	 * Given an input report, update all internal state appropriately
	 */
	void HandleInputReport(const InputReport& report);

	/**
	 * Enters an infinite loop of gathering and handling input reports
	 */
	void WatchForInputReports();

	/**
	 * Writes a given output report to the wiimote
	 */
	void SendOutputReport(unsigned char* buffer);

	/**
	 * Initializes the IR camera
	 */
	void ActivateIRCamera();

private:
	HANDLE pipe;
	PHIDP_PREPARSED_DATA preparsed_data;
	HIDP_CAPS capabilities;
};