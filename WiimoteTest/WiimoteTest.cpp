// WiimoteTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include <thread>
#include <mutex>

#include <vector>

#include "ErrorReporting.h"
#include "WiimoteHandler.h"

unsigned char status_request[22] = { 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char set_leds[22] = { 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char core_buttons[22] = { 0x12, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char acc_ir_ext[22] = { 0x12, 0x00, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char ir_enable_1[22] = { 0x13, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char ir_enable_2[22] = { 0x1a, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

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

std::vector<HANDLE> GetMatchingHandles(USHORT matchedVID, USHORT matchedPID) {
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
					if (device_attrib.VendorID == matchedVID && device_attrib.ProductID == matchedPID) {
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

void dumpstuff(const WiimoteHandler& wiimote) {
	while (true) {
		//std::cout << "HEY" << std::endl;
	}
}

int _tmain(int argc, _TCHAR* argv[]) {
	WiimoteHandler wiimote;
	std::thread dumpthread(&dumpstuff, wiimote);
	dumpthread.detach();

	std::vector<HANDLE> wiimote_handles = GetMatchingHandles(wiimoteVendorID, wiimoteProductID);
	if (wiimote_handles.size() < 1) {
		return 1;
	}
	wiimote.SetPipe(wiimote_handles[0]);
	wiimote.SendOutputReport(ir_enable_1);
	wiimote.SendOutputReport(ir_enable_2);
	wiimote.SendOutputReport(set_leds);
	wiimote.SendOutputReport(acc_ir_ext);
	wiimote.WatchForInputReports();


	//return oldmain(argc, argv);
	return 0;
}
