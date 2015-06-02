// WiimoteTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include <thread>
#include <mutex>

#include <vector>

#include "ErrorReporting.h"
#include "WiimoteHandler.h"
#include "OutputReport.h"

#include "BeginDirectx.h"

void dumpstuff(WiimoteHandler* wiimote) {
	VRBackendBasics graphics_objects = BeginDirectx(false, "");
	MSG msg;
	int prev_time = timeGetTime();
	int frame_index = 0;

	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				break;
			}
			else if (msg.message == WM_KEYDOWN) {
				graphics_objects.input_handler->HandleKeydown(msg.wParam);
			}
		}

		int new_time = timeGetTime();
		int time_delta = new_time - prev_time;
		prev_time = new_time;

		graphics_objects.input_handler->UpdateStates(frame_index);
		graphics_objects.world->UpdateLogic(time_delta);

		graphics_objects.render_pipeline->Render();

		++frame_index;
	}

	// clean up DirectX and COM
	graphics_objects.view_state->Cleanup();
	if (graphics_objects.input_handler->IsOculusActive()) {
		graphics_objects.oculus->Cleanup();
	}

}

int _tmain(int argc, _TCHAR* argv[]) {
	WiimoteHandler wiimote;

	std::vector<HANDLE> wiimote_handles = WiimoteHandler::GetWiimoteHandles();
	if (wiimote_handles.size() < 1) {
		return 1;
	}
	wiimote.SetPipe(wiimote_handles[0]);
	wiimote.ActivateIRCamera();
	wiimote.SendOutputReport(OutputReportTemplates::status_request);
	wiimote.SendOutputReport(OutputReportTemplates::set_leds);
	wiimote.SetDataReportingMethod(0x37, false);
	wiimote.SendOutputReport(OutputReportTemplates::request_calibration);
	wiimote.CheckForMotionPlus();
	std::thread wiimote_update_thread(&WiimoteHandler::WatchForInputReports, &wiimote);
	wiimote_update_thread.detach();

	dumpstuff(&wiimote);

	//return oldmain(argc, argv);
	return 0;
}
