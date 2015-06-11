#include "WiimoteHandler.h"


WiimoteHandler::WiimoteHandler()
	: current_state({ Quaternion(0, 0, 0, 1) }),
	gravity({ { 0, 0, -1 } })
{
	acceleration_calibration[0] = 0;
	acceleration_calibration[1] = 0;
	acceleration_calibration[2] = 0;
	has_motion_plus = false;
	has_extension = false;
	calibrate_motion_plus = false;
	PushCurrentState();
}

std::vector<HANDLE> WiimoteHandler::GetWiimoteHandles() {
	std::vector<HANDLE> handles;

	GUID hid_guid;
	HidD_GetHidGuid(&hid_guid);
	HWND fict_parent = 0;
	HDEVINFO device_info = SetupDiGetClassDevs(&hid_guid, NULL, fict_parent, DIGCF_DEVICEINTERFACE);
	SP_DEVICE_INTERFACE_DATA device_data;
	ZeroMemory(&device_data, sizeof(device_data));
	device_data.cbSize = sizeof(device_data);
	//device_data.cbSize = 5;
	DWORD device_interface_index = 0;
	while (SetupDiEnumDeviceInterfaces(device_info, NULL, &hid_guid, device_interface_index, &device_data)) {
		DWORD device_detail_size = 0;
		SetupDiGetDeviceInterfaceDetail(device_info, &device_data, NULL, 0, &device_detail_size, NULL);
		reportLastError(L"DEVICE DETAIL SIZE GET", ERROR_INSUFFICIENT_BUFFER);

		int total_device_detail_size = FIELD_OFFSET(SP_DEVICE_INTERFACE_DETAIL_DATA, DevicePath[device_detail_size]);
		PSP_DEVICE_INTERFACE_DETAIL_DATA device_detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)::operator new(total_device_detail_size);
		ZeroMemory(device_detail, total_device_detail_size);
		device_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		reportLastError(L"DEV DETAIL CREATED");

		if (SetupDiGetDeviceInterfaceDetail(device_info, &device_data, device_detail, device_detail_size, &device_detail_size, NULL)) {
			reportLastError(L"DEVICE DETAIL GET");
			HANDLE device_file = CreateFile(device_detail->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			if (reportLastError(L"OPEN DEVICE FILE")) {
				HIDD_ATTRIBUTES device_attrib;
				if (HidD_GetAttributes(device_file, &device_attrib)) {
					std::cout << "HI" << std::endl;
					if (device_attrib.VendorID == wiimoteVendorID && device_attrib.ProductID == wiimoteProductID) {
						handles.push_back(device_file);
					}
				}
				else {
					reportLastError(L"GET DEVICE ATTRIBUTES");
				}
			}
		}
		else {
			//reportLastError(L"DEVICE DETAIL GET");
		}
		device_interface_index++;
	}
	reportLastError(L"ENUMERATING DEVICE INTERFACES");
	return handles;
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
	reportLastError(L"ATTEMPTING READ", ERROR_IO_PENDING);
	WaitForSingleObject(readEvent, INFINITE);

	return InputReport(buffer, capabilities.InputReportByteLength, true);
}

void WiimoteHandler::HandleInputReport(const InputReport& report) {
	//report.DumpToStdout();
	unsigned char* buffer = report.GetBuffer();
	switch (buffer[0]) {
	case STATUS: {
		SetHasExtension((buffer[3] & 0x02) != 0);
		report.DumpToStdout();
		break;
	}
	case READ_MEM_AND_REG: {
		unsigned int data_size = report.GetDataSize();
		unsigned int data_error = report.GetDataError();
		unsigned int data_offset = report.GetDataOffset();
		unsigned char* data_buffer = report.GetDataPacket(false);
		if (data_offset == 0x16 || data_offset == 0x20) {
			CalibrateAccelerometer(data_buffer);
		} else if (data_offset == 0xFA) { // Sorta implies a motion plus data request
			ActivateWiiMotionPlus();
			std::cout << "EXTENSION GET THINGS" << std::endl;
		}
		report.DumpToStdout();
		break;
	}
	case ACK_OUTPUT_REPORT: {
		unsigned char output_code = report.GetAckOutputReportCode();
		unsigned char error = report.GetAckError();
		std::cout << "OUTPUT CODE: " << (unsigned int)output_code << ", " << (unsigned int)error <<  std::endl;
		break;
	}
	case DATA_BUT_ACC_IR10_EXT6: {
		previous_data = current_data;
		twice_previous_state_time = previous_state_time;
		previous_state_time = current_state_time;
		QueryPerformanceCounter(&current_state_time);
		current_data.acceleration = report.GetLinearAccelerationReading();
		current_data.ir_data = report.GetIRData();
		current_data.button_state = report.GetButtonState();
		current_data.nunchuck_state = report.GetNunchuckState();
		current_data.motion_plus_state = report.GetMotionPlusState();
		//std::cout << acc.direction[0] << "\t" << acc.direction[1] << "\t" << acc.direction[2] << std::endl;
		// Needs to be modified so it detects nunchuck information, as
		// has_extension also detects the motion plus
		if (has_extension) {
			if (nunchuck_stick_calibration[0] == -1 && current_data.nunchuck_state.stick_position[0] != 0xFF) {
				nunchuck_stick_calibration[0] = current_data.nunchuck_state.stick_position[0];
				nunchuck_stick_calibration[1] = current_data.nunchuck_state.stick_position[1];
			}
		}
		/*
		for (int i = 0; i < 3; i++) {
			int true_directional_acc = acc.acceleration[i] - acceleration_calibration[i];
			if (abs(true_directional_acc - gravity_calibration[i]) < 4) {
				std::cout << i << " UP" << std::endl;
			} else if (abs(true_directional_acc + gravity_calibration[i]) < 4) {
				std::cout << i << " DOWN" << std::endl;
			}
		}
		*/
		/*
		for (int i = 0; i < 4; i++) {
			std::cout << ird.points[i].point[0] << ", " << ird.points[i].point[1] << "\t";
		}
		std::cout << std::endl;
		*/
		/*
		if (has_extension) {
			std::cout << es.stick_position[0] - nunchuck_stick_calibration[0] << ",\t" << es.stick_position[1] - nunchuck_stick_calibration[1] << std::endl;
		}
		*/
		//std::cout << mps.rotation[0] << std::endl;
		//report.DumpToStdout();
		UpdateCurrentState();
		PushCurrentState();
		break;
	}
	default:
		report.DumpToStdout();
		break;
	}
}

void WiimoteHandler::CalibrateAccelerometer(unsigned char* calibration_data) {
	UnpackLinearAccelerationReadingCalibration(calibration_data, acceleration_calibration);
	UnpackLinearAccelerationReadingCalibration(calibration_data+4, gravity_calibration);
	for (int i = 0; i < 3; i++) { gravity_calibration[i] -= acceleration_calibration[i]; }
}

void WiimoteHandler::RequestCalibrateMotionPlus() {
	calibrate_motion_plus = true;
}

void WiimoteHandler::ActivateIRCamera() {
	SendOutputReport(OutputReportTemplates::ir_enable_1);
	Sleep(75);
	SendOutputReport(OutputReportTemplates::ir_enable_2);
	Sleep(75);
	SendOutputReport(ConstructMemoryWrite(true, 0xb00030, 0x08).buffer);
	Sleep(75);
	SendOutputReport(ConstructMemoryWrite(true, 0xb00000, 9, OutputReportTemplates::sensitivity_block1).buffer);
	Sleep(75);
	SendOutputReport(ConstructMemoryWrite(true, 0xb0001a, 2, OutputReportTemplates::sensitivity_block2).buffer);
	Sleep(75);
	SendOutputReport(ConstructMemoryWrite(true, 0xb00033, 0x1).buffer);
	Sleep(75);
	SendOutputReport(ConstructMemoryWrite(true, 0xb00030, 0x08).buffer);
}

void WiimoteHandler::ActivateExtension() {
	SendOutputReport(ConstructMemoryWrite(true, 0xA400F0, 0x55).buffer);
	Sleep(75);
	SendOutputReport(ConstructMemoryWrite(true, 0xA400FB, 0x00).buffer);
}

void WiimoteHandler::ActivateWiiMotionPlus() {
	has_motion_plus = true;
	SendOutputReport(ConstructMemoryWrite(true, 0xA600F0, 0x55).buffer);
	Sleep(75);
	SendOutputReport(ConstructMemoryWrite(true, 0xA600FE, 0x04).buffer);
}

void WiimoteHandler::CheckForMotionPlus() {
	SendOutputReport(ConstructMemoryRead(true, 0xA600FA, 6).buffer);
}

void WiimoteHandler::WatchForInputReports() {
	while (true) {
		HandleInputReport(GatherInputReport());
	}
}

void WiimoteHandler::SendOutputReport(const OutputReport& report) {
	SendOutputReport(report.GetBuffer());
}

void WiimoteHandler::SendOutputReport(unsigned char* buffer) {
	HidD_SetOutputReport(pipe, buffer, capabilities.OutputReportByteLength);
}


void WiimoteHandler::SetDataReportingMethod(unsigned char report_mode, bool continuous) {
	current_report_mode = report_mode;
	continuous_reporting = continuous;
	SetWiimoteDataReportingMethod();
}

void WiimoteHandler::SetWiimoteDataReportingMethod() {
	unsigned char output_report[22];
	ZeroMemory(output_report, 22);
	output_report[0] = 0x12;
	output_report[1] = 0x02;
	if (continuous_reporting) {
		output_report[1] |= 0x04;
	}
	output_report[2] = current_report_mode;
	SendOutputReport(output_report);
}

void WiimoteHandler::SetHasExtension(bool extension_plugged_in) {
	bool old_has_extension = has_extension;
	has_extension = extension_plugged_in;
	std::cout << has_extension << std::endl;
	if (has_extension != old_has_extension) {
		SetWiimoteDataReportingMethod();
	}
	if (has_extension && !has_motion_plus) {
		ActivateExtension();
	}

	for (int i = 0; i < 2; i++) { nunchuck_stick_calibration[i] = -1; }
}

void WiimoteHandler::UpdateCurrentState() {
	static int a = 0;
	LinearAcceleration acc(current_data.acceleration, acceleration_calibration, gravity_calibration);
	//std::cout << acc.magnitude << std::endl;
	//std::cout << acc.acceleration[0] << "\t" << acc.acceleration[1] << "\t" << acc.acceleration[2] << std::endl;
	if (has_motion_plus) {
		if (calibrate_motion_plus) {
			calibrate_motion_plus = false;
			for (int i = 0; i < 3; i++) {
				motion_plus_calibration[i] = current_data.motion_plus_state.rotation[i];
			}
		}
		RotationalMotion rot_acc(previous_data.motion_plus_state.rotation, motion_plus_calibration, previous_data.motion_plus_state.rotation_mode, (current_state_time.QuadPart - twice_previous_state_time.QuadPart) / 2);
		if (previous_data.button_state.GetButtonPressed(B_MASK)) {
			std::cout << rot_acc.rotation_in_radians[0] << std::endl;
			current_state.orientation = current_state.orientation * Quaternion::RotationAboutAxis(AID_Z, rot_acc.rotation_in_radians[0]);
		}
	}
	if ((abs(acc.magnitude - 1.0f) < 0.03f) && current_data.button_state.GetButtonPressed(A_MASK)) {
		current_state.orientation = Quaternion::RotationBetweenVectors(std::array<float, 3>({ { 0, 0, 1 } }), acc.direction);
	}
}

void WiimoteHandler::PushCurrentState() {
	outward_facing_state.store(current_state, std::memory_order_seq_cst);
}

WiimoteState WiimoteHandler::GetCurrentState() {
	return outward_facing_state.load(std::memory_order_seq_cst);
}