//***************************************************************************************
// TexturedHillsAndWaves.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates texture tiling and texture animation.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
//***************************************************************************************

//#include<Windows.h>

#include "SnowmanApp.h"
#include <math.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	SnowmanApp theApp(hInstance);

	if (!theApp.Init())
		return 0;

	return theApp.Run();
}

SnowmanApp::SnowmanApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
	mMainWndCaption = L"SnowmanApp";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
	
	//set lights
	mDirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	//specify the number of subobjects
	

}

SnowmanApp::~SnowmanApp()
{
	SafeDelete(mSky);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	Models::DestroyAll();
}

bool SnowmanApp::Init()
{
	if (!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	Models::InitAll(md3dDevice);

	XMFLOAT3 center(60.0f, 0.0f, -40.0f);

	mCam.LookAt(XMFLOAT3(40.0f, 20.0f, 0.0f),
				XMFLOAT3(60.0f, 4.0f, -30.0f),
				XMFLOAT3(0.0f, 1.0f, 0.0f));

	snowman.BindModels(Models::SnowmanModelData);
	snowman.SetPostion(XMFLOAT3(80.0f, 12.0f, -30.0f));
	snowman.SetAnimationState(XMFLOAT3(60.0f, 12.0f, -30.0f), 20.0f, 1.0f);
	
	snowmanStatic.BindModels(Models::SnowmanModelData);
	snowmanStatic.SetPostion(XMFLOAT3(60.0f, 4.0f, -30.0f));

	box.BindModels(Models::BoxModelData);
	box.SetPostion(XMFLOAT3(80.0f, 4.0f, -30.0f));
	box.SetAnimationState(XMFLOAT3(60.0f, 4.0f, -30.0f), 20.0f, 1.0f);

	terrain.BindModels(Models::TerrainModelData);
	terrain.SetPostion(XMFLOAT3(0.0f, 0.0f, 0.0f));

	mSky = new Sky(md3dDevice, L"Textures/snowcube1024.dds", 5000.0f);

	return true;
}

void SnowmanApp::OnResize()
{
	D3DApp::OnResize();
	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void SnowmanApp::UpdateScene(float dt)
{
	//
	// Control the camera.
	//
	if (GetAsyncKeyState('W') & 0x8000)
		mCam.Walk(20.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCam.Walk(-20.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCam.Strafe(-20.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCam.Strafe(20.0f*dt);

	snowman.UpdatePostion(dt);
	box.UpdatePostion(dt);

	mCam.UpdateViewMatrix();
}


void SnowmanApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);

	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light3TexTech;


	snowman.Draw(md3dImmediateContext, activeTech, mCam);	
	snowmanStatic.Draw(md3dImmediateContext, activeTech, mCam);
	box.Draw(md3dImmediateContext, activeTech, mCam);
	terrain.Draw(md3dImmediateContext, activeTech, mCam);

	mSky->Draw(md3dImmediateContext, mCam);

	HR(mSwapChain->Present(0, 0));
}

void SnowmanApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void SnowmanApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void SnowmanApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
