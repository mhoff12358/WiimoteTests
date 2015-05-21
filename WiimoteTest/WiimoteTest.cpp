// WiimoteTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

PBLUETOOTH_DEVICE_INFO selectDevice() {
	BLUETOOTH_SELECT_DEVICE_PARAMS dev_sel;

	ZeroMemory(&dev_sel, sizeof(dev_sel));
	dev_sel.dwSize = sizeof(dev_sel);
	dev_sel.cNumOfClasses = 0;
	dev_sel.prgClassOfDevices = NULL;
	dev_sel.pszInfo = L"HELLO!";
	dev_sel.hwndParent = NULL;
	dev_sel.fForceAuthentication = FALSE;
	dev_sel.fShowAuthenticated = TRUE;
	dev_sel.fShowRemembered = TRUE;
	dev_sel.fShowUnknown = TRUE;
	dev_sel.fAddNewDeviceWizard = FALSE;
	dev_sel.fSkipServicesPage = FALSE;
	dev_sel.pfnDeviceCallback = NULL;
	dev_sel.pvParam = NULL;
	dev_sel.cNumDevices = 1;
	dev_sel.pDevices = NULL;

	bool selectSuccess = BluetoothSelectDevices(&dev_sel);

	if (selectSuccess) {
		return dev_sel.pDevices;
	} else {
		return NULL;
	}
}

void connectToDevice(BLUETOOTH_DEVICE_INFO device) {
	SOCKADDR_BTH device_address;
	device_address.addressFamily = AF_BTH;
	GUID hid_guid;
	HidD_GetHidGuid(&hid_guid);
	device_address.serviceClassId = hid_guid;
	device_address.port = 0;
	device_address.btAddr = device.Address.ullLong;


}

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

char status_request[22] = { 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
char set_leds[22] = { 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
char core_buttons[22] = { 0x12, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void DumpReport(unsigned char* buffer, int length) {
	std::cout << std::hex;
	for (int i = 0; i < length; i++) {
		std::cout << std::setfill('0') << std::setw(2) << (unsigned int)buffer[i] << " ";
	}
	std::cout << std::endl;
	std::cout << std::dec;
}

void WriteToDevice(HANDLE device_io, PHIDP_PREPARSED_DATA preparsed_data, HIDP_CAPS device_caps, char* msg) {
	char request_buf[22];// = new char[device_caps.OutputReportByteLength];
	ZeroMemory(request_buf, device_caps.OutputReportByteLength);
	NTSTATUS report_status = HidP_InitializeReportForID(HidP_Output, 0x15, preparsed_data, request_buf, device_caps.OutputReportByteLength);
	std::cout << "HID report creation status: " << reportHIDStatus(report_status) << std::endl;

	reportLastError(L"PRE WRITE");
	OVERLAPPED async_overlap;
	ZeroMemory(&async_overlap, sizeof(async_overlap));
	async_overlap.Offset = 0;
	async_overlap.OffsetHigh = 0;
	async_overlap.Pointer = 0;
	HANDLE writeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!writeEvent) {
		std::cout << "EVENT CREATION ERROR" << std::endl;
	}
	reportLastError(L"CREATE WRITE EVENT");
	async_overlap.hEvent = writeEvent;

	DWORD num_bytes_written;
	char* pkt = new char[65];
	ZeroMemory(pkt, 65);
	/*
	if (!WriteFile(device_io, msg, 22, NULL, &async_overlap)) {
		//if (!WriteFile(device_io, status_request, 22, &num_bytes_written, NULL)) {
		reportLastError(L"ATTEMPTING WRITE");
	}
	WaitForSingleObject(writeEvent, INFINITE);*/

	HidD_SetOutputReport(device_io, msg, 22);

	reportLastError(L"WAITING FOR WRITE");
	ResetEvent(writeEvent);
	reportLastError(L"RESETTING EVENT");
}




unsigned char* ReadFromDevice(HANDLE device_io, PHIDP_PREPARSED_DATA preparsed_data, HIDP_CAPS device_caps) {
	OVERLAPPED async_overlap;
	ZeroMemory(&async_overlap, sizeof(async_overlap));
	async_overlap.Offset = 0;
	async_overlap.OffsetHigh = 0;
	async_overlap.Pointer = 0;
	HANDLE writeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!writeEvent) {
		std::cout << "EVENT CREATION ERROR" << std::endl;
	}
	reportLastError(L"CREATE WRITE EVENT");
	async_overlap.hEvent = writeEvent;

	unsigned char* buffer = new unsigned char[device_caps.OutputReportByteLength];
	//unsigned char buffer[22];
	ZeroMemory(buffer, 22);
	DWORD total_bytes_read = 0;

	/*
	while (total_bytes_read < device_caps.OutputReportByteLength) {
		DWORD new_bytes_read;
		if (!ReadFile(device_io, buffer + total_bytes_read, device_caps.OutputReportByteLength - total_bytes_read, &new_bytes_read, &async_overlap)) {
			reportLastError(L"ATTEMPTING READ");
			return NULL;
		}
		total_bytes_read += new_bytes_read;
	}
	*/

	ReadFile(device_io, buffer, device_caps.OutputReportByteLength, NULL, &async_overlap);
	reportLastError(L"ATTEMPTING READ");
	WaitForSingleObject(writeEvent, INFINITE);

	return buffer;
}

USHORT wiimoteVendorID = 0x057e;
USHORT wiimoteProductID = 0x0306;

int _tmain(int argc, _TCHAR* argv[])
{
	GUID hid_guid;
	HidD_GetHidGuid(&hid_guid);
	HWND fict_parent = 0;
	HDEVINFO device_info = SetupDiGetClassDevs(&hid_guid, NULL, fict_parent, DIGCF_DEVICEINTERFACE);
	SP_DEVICE_INTERFACE_DATA device_data;
	ZeroMemory(&device_data, sizeof(device_data));
	device_data.cbSize = sizeof(device_data);

	int errcnt = 0;
	int succnt = 0;
	int foundcnt = 0;

	DWORD device_interface_index = 0;
	while (SetupDiEnumDeviceInterfaces(device_info, NULL, &hid_guid, device_interface_index, &device_data)) {
		SP_DEVICE_INTERFACE_DETAIL_DATA device_detail;
		ZeroMemory(&device_detail, sizeof(device_detail));
		device_detail.cbSize = sizeof(device_detail);
		//device_detail.cbSize = 5;
		reportLastError(L"DEV DETAIL CREATED");

		DWORD device_detail_size = 0;
		SetupDiGetDeviceInterfaceDetail(device_info, &device_data, NULL, 0, &device_detail_size, NULL);
		reportLastError(L"DEVICE DETAIL SIZE GET", ERROR_INSUFFICIENT_BUFFER);
		if (SetupDiGetDeviceInterfaceDetail(device_info, &device_data, &device_detail, device_detail_size, &device_detail_size, NULL)) {
			reportLastError(L"DEVICE DETAIL GET");
			HANDLE device_file = CreateFile(device_detail.DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			//HANDLE device_file = CreateFile(device_detail.DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
			if (reportLastError(L"OPEN DEVICE FILE")) {
				HIDD_ATTRIBUTES device_attrib;
				if (HidD_GetAttributes(device_file, &device_attrib)) {
					succnt++;
					if (device_attrib.VendorID == wiimoteVendorID && device_attrib.ProductID == wiimoteProductID) {
						PHIDP_PREPARSED_DATA preparsed_data;
						if (HidD_GetPreparsedData(device_file, &preparsed_data)) {
							HIDP_CAPS device_capabilities;
							NTSTATUS get_caps_err = HidP_GetCaps(preparsed_data, &device_capabilities);

							WriteToDevice(device_file, preparsed_data, device_capabilities, core_buttons);
							//WriteToDevice(device_file, preparsed_data, device_capabilities, set_leds);
							while (true) {
								WriteToDevice(device_file, preparsed_data, device_capabilities, status_request);
								unsigned char* read_in = ReadFromDevice(device_file, preparsed_data, device_capabilities);
								/*if (!HidD_SetOutputReport(device_file, status_request, 3)) {
									reportLastError(L"ERROR SETTING OUTPUT REPORT");
									std::cout << "ERROR SETTING OUTPUT REPORT" << std::endl;
									}*/
								DumpReport(read_in, 22);
								std::cin.get();
								//WriteToDevice(device_file, preparsed_data, device_capabilities, set_leds);
							}
							foundcnt++;
						} else {
							reportLastError(L"Getting preparsed data");
							errcnt++;
						}
					}
				} else {
					reportLastError(L"GET DEVICE ATTRIBUTES");
					errcnt++;
				}
			} else {
				errcnt++;
			}
		} else {
			reportLastError(L"DEVICE DETAIL GET");
			errcnt++;
		}

		device_interface_index++;
	}

	std::cout << foundcnt << " " << succnt << " " << errcnt << std::endl;

	return 0;
}

