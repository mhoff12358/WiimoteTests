#pragma once

#include "stdafx.h"

#include "InputReport.h"
#include "ErrorReporting.h"
#include "BitHelpers.h"
#include "OutputReport.h"

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
	void SendOutputReport(const OutputReport& buffer);

	/**
	 * Initializes the IR camera
	 */
	void ActivateIRCamera();

	/**
	 * Initializes the extension
	 */
	void ActivateExtension();

	/**
	 * Handles the accelerometer calibration given the data loaded from memory
	 */
	void CalibrateAccelerometer(unsigned char* calibration_data);

	/**
	 * Sets the desired data reporting method that the wiimote should be set to
	 */
	void SetDataReportingMethod(unsigned char report_mode, bool continuous);

	/**
	 * Tells the wiimote to report in the current desired reporting mode
	 */
	void SetWiimoteDataReportingMethod();

	/**
	 * Sets whether there is an extension plugged in. Importantly, this has
	 * internal triggers for getting/clearing extension calibration data
	 */
	void SetHasExtension(bool extension_plugged_in);

private:
	HANDLE pipe;
	PHIDP_PREPARSED_DATA preparsed_data;
	HIDP_CAPS capabilities;

	int acceleration_calibration[3];
	int gravity_calibration[3];

	int nunchuck_stick_calibration[2];

	bool has_extension;

	unsigned char current_report_mode;
	bool continuous_reporting;
};