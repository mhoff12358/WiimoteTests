#pragma once

#include "stdafx.h"
#include <iostream>

void DumpError();
void DumpWSAError();

std::pair<BLUETOOTH_DEVICE_INFO, BLUETOOTH_RADIO_INFO> FindBluetoothDevices();
bool AuthWiimote();