#pragma once

#include "stdafx.h"

#include <array>
#include <atomic>

#include "InputReport.h"
#include "ErrorReporting.h"
#include "BitHelpers.h"
#include "OutputReport.h"
#include "LinearAcceleration.h"
#include "RotationalMotion.h"
#include "Quaternion.h"

struct WiimoteState {
	Quaternion orientation;
};

struct WiimoteSensorState {
	LinearAccelerationReading acceleration;
	IRData ir_data;
	ButtonState button_state;
	NunchuckState nunchuck_state;
	MotionPlusState motion_plus_state;
};

class WiimoteHandler {
public:
	static const USHORT wiimoteVendorID = 0x057e;
	static const USHORT wiimoteProductID = 0x0306;
	static std::vector<HANDLE> GetWiimoteHandles();

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
	 * Initializes the non-motion plus extension
	 */
	void ActivateExtension();

	/**
	 * Initializes the wii motion plus that should be attached
	 */
	void ActivateWiiMotionPlus();

	/**
	 * Handles the accelerometer calibration given the data loaded from memory
	 */
	void CalibrateAccelerometer(unsigned char* calibration_data);

	/**
	 * Tells the motion plus to calibrate the next chance it gets
	 */
	void RequestCalibrateMotionPlus();

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

	/**
	 * Checks the registers for the location of the wii motion plus. This
	 * doesn't actually receive the response, its up to HanldeInputReport to do
	 * that.
	 */
	void CheckForMotionPlus();

	/**
	 * Updates the current state based on the current sensor data
	 */
	void UpdateCurrentState();

	/**
	 * Stores the current state of the wiimote in the outward facing state so
	 * that it can be accessed asynchronously
	 */
	void PushCurrentState();

	/**
	 * Gets the currently outwardly facing state
	 */
	WiimoteState GetCurrentState();

	/**
	 * Sets how rapidly the orientation is corrected based on the gravity vector
	 */
	void SetGravityCorrectionScale(float new_gravity_correction_scale);

private:
	HANDLE pipe;
	PHIDP_PREPARSED_DATA preparsed_data;
	HIDP_CAPS capabilities;

	// Handler calibration values
	float gravity_correction_scale;

	// Calibration values of the wiimote itself
	int acceleration_calibration[3];
	int gravity_calibration[3];
	int motion_plus_calibration[3];
	int nunchuck_stick_calibration[2];

	// Known values about the state of the wiimote
	bool has_extension;
	bool has_motion_plus;
	std::atomic<bool> calibrate_motion_plus;

	unsigned char current_report_mode;
	bool continuous_reporting;

	// Current state storage
	WiimoteSensorState previous_data;
	WiimoteSensorState current_data;
	LARGE_INTEGER twice_previous_state_time;
	LARGE_INTEGER previous_state_time;
	LARGE_INTEGER current_state_time;
	WiimoteState current_state;
	std::array<float, 3> gravity;
	std::atomic<WiimoteState> outward_facing_state;
};