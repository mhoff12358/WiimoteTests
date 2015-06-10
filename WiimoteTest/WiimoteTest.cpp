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
#include "ObjLoader.h"

Entity makewiimote(VRBackendBasics graphics_objects) {
	std::vector<Vertex> vertices;
	VertexType vertex_type = VertexType(std::vector<D3D11_INPUT_ELEMENT_DESC>({
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	}));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f }));

	vertices.push_back(Vertex(vertex_type, { -1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, {  1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, {  1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, {  1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f }));

	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f }));

	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f }));

	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f }));

	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f }));

	Model model = graphics_objects.resource_pool->LoadModel("wiimote", vertices, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ConstantBufferTyped<TransformationMatrixAndInvTransData>* object_settings = new ConstantBufferTyped<TransformationMatrixAndInvTransData>(CB_PS_VERTEX_SHADER);
	object_settings->CreateBuffer(graphics_objects.view_state->device_interface);
	object_settings->SetBothTransformations(DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(3.14f), DirectX::XMMatrixTranslation(0, 0, -4)));
	object_settings->PushBuffer(graphics_objects.view_state->device_context);

	return Entity(
		ES_NORMAL,
		graphics_objects.resource_pool->LoadPixelShader("shaders.hlsl"),
		graphics_objects.resource_pool->LoadVertexShader("shaders.hlsl", vertex_type.GetVertexType(), vertex_type.GetSizeVertexType()),
		ShaderSettings(NULL),
		model,
		object_settings);
}

void dumpstuff(WiimoteHandler* wiimote) {
	VRBackendBasics graphics_objects = BeginDirectx(false, "");
	MSG msg;
	int prev_time = timeGetTime();
	int frame_index = 0;

	//VertexType vertex_type = common_vertex_types[1];
	VertexType vertex_type = ObjLoader::vertex_type;
	ConstantBufferTyped<TransformationMatrixAndInvTransData>* object_settings = new ConstantBufferTyped<TransformationMatrixAndInvTransData>(CB_PS_VERTEX_SHADER);
	object_settings->CreateBuffer(graphics_objects.view_state->device_interface);
	object_settings->SetBothTransformations(DirectX::XMMatrixTranslation(0, 0, -4));
	object_settings->PushBuffer(graphics_objects.view_state->device_context);
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(vertex_type, { -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	vertices.push_back(Vertex(vertex_type, { -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f }));
	Model mod = graphics_objects.resource_pool->LoadModel("square", vertices, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphics_objects.entity_handler->AddEntity(Entity(
		ES_DISABLED,
		graphics_objects.resource_pool->LoadPixelShader("objshader.hlsl"),
		graphics_objects.resource_pool->LoadVertexShader("objshader.hlsl", vertex_type.GetVertexType(), vertex_type.GetSizeVertexType()),
		ShaderSettings(NULL),
		graphics_objects.resource_pool->LoadModel("C:\\Users\\Matt\\Desktop\\rhod.obj"),
		object_settings));
	//root_render_group.entities.emplace_back(ES_NORMAL, ps, vs, ss, mod, object_settings);
	
	unsigned int wiimote_entity_id = graphics_objects.entity_handler->AddEntity(makewiimote(graphics_objects));
	
	graphics_objects.entity_handler->FinishUpdate();

	Quaternion obj_orient;

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

				switch (msg.wParam) {
				case 'W':
					obj_orient = Quaternion(0.15643446504023086901010531946717, 0, 0, 0.98768834059513772619004024769344) * obj_orient;
					break;
				case 'S':
					obj_orient = Quaternion(-0.15643446504023086901010531946717, 0, 0, 0.98768834059513772619004024769344) * obj_orient;
					break;
				case 'A':
					obj_orient = Quaternion(0, 0.15643446504023086901010531946717, 0, 0.98768834059513772619004024769344) * obj_orient;
					break;
				case 'D':
					obj_orient = Quaternion(0, -0.15643446504023086901010531946717, 0, 0.98768834059513772619004024769344) * obj_orient;
					break;
				}
			}
		}

		obj_orient = wiimote->GetCurrentState().orientation;
		// Convert orientation from wiimote coord system to screen coord system
		float tmp = obj_orient.y;
		obj_orient.y = obj_orient.z;
		obj_orient.z = tmp;
		obj_orient.x = -obj_orient.x;
//		std::cout << acos(obj_orient.w) << std::endl;
		ConstantBufferTyped<TransformationMatrixAndInvTransData>* wiimote_settings = graphics_objects.entity_handler->GetEntityObjectSettings<TransformationMatrixAndInvTransData>(wiimote_entity_id);
		wiimote_settings->SetBothTransformations(
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixRotationQuaternion(
					DirectX::XMVectorSet(
						obj_orient.x,
						obj_orient.y,
						obj_orient.z,
						obj_orient.w)),
					DirectX::XMMatrixTranslation(0, 0, -4)));
		graphics_objects.entity_handler->FinishUpdate();

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

class A {
	
public:
	A(int thing) : t(thing) {}
	int t;
};

int _tmain(int argc, _TCHAR* argv[]) {
	WiimoteHandler wiimote;

	std::vector<HANDLE> wiimote_handles = WiimoteHandler::GetWiimoteHandles();
	if (wiimote_handles.size() < 1) {
		//return 1;
	}
	else {
		wiimote.SetPipe(wiimote_handles[0]);
		wiimote.ActivateIRCamera();
		wiimote.SendOutputReport(OutputReportTemplates::status_request);
		wiimote.SendOutputReport(OutputReportTemplates::set_leds);
		wiimote.SetDataReportingMethod(0x37, false);
		wiimote.SendOutputReport(OutputReportTemplates::request_calibration);
		wiimote.CheckForMotionPlus();
		std::thread wiimote_update_thread(&WiimoteHandler::WatchForInputReports, &wiimote);
		wiimote_update_thread.detach();
	}

	dumpstuff(&wiimote);

	//return oldmain(argc, argv);
	return 0;
}
