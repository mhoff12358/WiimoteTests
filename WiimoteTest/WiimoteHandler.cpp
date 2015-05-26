#include "WiimoteHandler.h"

WiimoteHandler::WiimoteHandler() {
	acceleration_calibration[0] = 0;
	acceleration_calibration[1] = 0;
	acceleration_calibration[2] = 0;
	has_motion_plus = false;
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
		Acceleration acc = report.GetAcceleration();
		IRData ird = report.GetIRData();
		ButtonState bs = report.GetButtonState();
		NunchuckState es = report.GetNunchuckState();
		MotionPlusState mps = report.GetMotionPlusState();
		// Needs to be modified so it detects nunchuck information, as
		// has_extension also detects the motion plus
		if (has_extension) {
			if (nunchuck_stick_calibration[0] == -1 && es.stick_position[0] != 0xFF) {
				nunchuck_stick_calibration[0] = es.stick_position[0];
				nunchuck_stick_calibration[1] = es.stick_position[1];
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
		std::cout << mps.rotation[0] << std::endl;
		//report.DumpToStdout();
		break;
	}
	default:
		report.DumpToStdout();
		break;
	}
}

void WiimoteHandler::CalibrateAccelerometer(unsigned char* calibration_data) {
	UnpackAccelerationCalibration(calibration_data, acceleration_calibration);
	UnpackAccelerationCalibration(calibration_data+4, gravity_calibration);
	for (int i = 0; i < 3; i++) { gravity_calibration[i] -= acceleration_calibration[i]; }
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