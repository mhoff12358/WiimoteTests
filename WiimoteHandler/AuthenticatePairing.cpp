#include "AuthenticatePairing.h"

void DumpError() {
	LPTSTR errorText = NULL;

	FormatMessage(
		// use system message tables to retrieve error text
		FORMAT_MESSAGE_FROM_SYSTEM
		// allocate buffer on local heap for error text
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		// Important! will fail otherwise, since we're not 
		// (and CANNOT) pass insertion parameters
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&errorText,  // output 
		0, // minimum size for output buffer
		NULL);

	std::wcout << std::wstring((wchar_t*)errorText) << std::endl;
}

void DumpWSAError() {
	LPTSTR errorText = NULL;

	FormatMessage(
		// use system message tables to retrieve error text
		FORMAT_MESSAGE_FROM_SYSTEM
		// allocate buffer on local heap for error text
		| FORMAT_MESSAGE_ALLOCATE_BUFFER
		// Important! will fail otherwise, since we're not 
		// (and CANNOT) pass insertion parameters
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&errorText,  // output 
		0, // minimum size for output buffer
		NULL);

	std::wcout << std::wstring((wchar_t*)errorText) << std::endl;
}

std::pair<BLUETOOTH_DEVICE_INFO, BLUETOOTH_RADIO_INFO> FindBluetoothDevices() {
	BLUETOOTH_FIND_RADIO_PARAMS radio_params;
	ZeroMemory(&radio_params, sizeof(radio_params));
	radio_params.dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);
	HANDLE radio_handle;
	HBLUETOOTH_RADIO_FIND radio_search_result = BluetoothFindFirstRadio(&radio_params, &radio_handle);

	if (radio_search_result == NULL) {
		std::wcout << L"error getting radio" << std::endl;
		DumpError();
	}
	BLUETOOTH_RADIO_INFO radio_info;
	ZeroMemory(&radio_info, sizeof(radio_info));
	radio_info.dwSize = sizeof(radio_info);
	DWORD radio_info_result = BluetoothGetRadioInfo(radio_handle, (BLUETOOTH_RADIO_INFO*)&radio_info);

	std::wcout << L"radio gotten" << std::endl;

	BLUETOOTH_DEVICE_SEARCH_PARAMS device_search_params;
	ZeroMemory(&device_search_params, sizeof(device_search_params));
	device_search_params.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	device_search_params.fReturnAuthenticated = true;
	device_search_params.fReturnRemembered = true;
	device_search_params.fReturnUnknown = true;
	device_search_params.fReturnConnected = true;
	device_search_params.fIssueInquiry = true;
	device_search_params.cTimeoutMultiplier = 4;
	device_search_params.hRadio = NULL;

	BLUETOOTH_DEVICE_INFO device_info;
	ZeroMemory(&device_info, sizeof(device_info));
	device_info.dwSize = sizeof(device_info);
	HBLUETOOTH_DEVICE_FIND device_search = BluetoothFindFirstDevice(&device_search_params, &device_info);

	if (device_search == NULL) {
		std::cout << "error getting device" << std::endl;
		DumpError();
	}

	std::wcout << std::wstring(device_info.szName) << L": " << device_info.fConnected << std::endl;
	if ((std::wstring(device_info.szName) == std::wstring(L"Nintendo RVL-CNT-01")) && device_info.fConnected) {
		return std::make_pair(device_info, radio_info);
	}
}

bool AuthWiimote() {
	auto bluetooth_info = FindBluetoothDevices();
	BLUETOOTH_DEVICE_INFO wiimote_info = bluetooth_info.first;
	BLUETOOTH_RADIO_INFO radio_info = bluetooth_info.second;

	BLUETOOTH_OOB_DATA_INFO reverse_address_oob;
	WCHAR reverse_address[6];
	for (int i = 0; i < 6; i++) {
		reverse_address_oob.C[5 - i] = wiimote_info.Address.rgBytes[i];
		reverse_address[i] = wiimote_info.Address.rgBytes[i];
	}

	reverse_address[0] = 0x13;
	reverse_address[1] = 0x71;
	reverse_address[2] = 0xDA;
	reverse_address[3] = 0x7D;
	reverse_address[4] = 0x1A;
	reverse_address[5] = 0x00;
	HBLUETOOTH_AUTHENTICATION_REGISTRATION auth_registration;

#pragma warning(disable: 4995)
	DWORD auth_result = BluetoothAuthenticateDevice(NULL, NULL, &wiimote_info, reverse_address, 6);
#pragma warning(enable: 4995)

	return auth_result == 0;
}