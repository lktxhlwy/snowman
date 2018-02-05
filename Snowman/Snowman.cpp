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

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"

class SnowmanApp : public D3DApp
{
public:
	SnowmanApp(HINSTANCE hInstance);
	~SnowmanApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	float GetHillHeight(float x, float z)const;
	XMFLOAT3 GetHillNormal(float x, float z)const;
	void BuildLandGeometryBuffers();
	void BuildSnowmanGeometryBuffers();
	//void BuildWaveGeometryBuffers();

private:
	ID3D11Buffer * mLandVB;
	ID3D11Buffer* mLandIB;

	ID3D11Buffer* mSnowmanVB;
	ID3D11Buffer* mSnowmanIB;

	ID3D11ShaderResourceView* mGrassMapSRV;
	ID3D11ShaderResourceView* mSnowmanBodyMapSRV;

	DirectionalLight mDirLights[3];
	Material mLandMat;
	Material mSnowmanMat;

	XMFLOAT4X4 mGrassTexTransform;
	XMFLOAT4X4 mSnowmanBodyTexTransform;

	XMFLOAT4X4 mLandWorld;
	XMFLOAT4X4 mSnowmanWorld;

	float snowmanTheta;
	float snowmanMoveRadius;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	UINT mLandIndexCount;
	UINT mSnowmanIndexCount;
	//XMFLOAT2 mWaterTexOffset;

	XMFLOAT3 mEyePosW;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};

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
	: D3DApp(hInstance), mLandVB(0), mLandIB(0), mSnowmanVB(0), mSnowmanIB(0), mGrassMapSRV(0),
	mEyePosW(0.0f, 0.0f, 0.0f), mLandIndexCount(0), mTheta(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(80.0f)
{
	mMainWndCaption = L"Snowman";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mLandWorld, I);

	XMMATRIX mSnowmanScale = XMMatrixScaling(5.0f, 5.0f, 5.0f);
	snowmanMoveRadius = 30.0f;
	snowmanTheta = 0.0f;
	XMMATRIX mSnowmanOffset = XMMatrixTranslation(snowmanMoveRadius*cos(snowmanTheta), 8.0f, snowmanMoveRadius*sin(snowmanTheta));	
	XMStoreFloat4x4(&mSnowmanWorld, XMMatrixMultiply(mSnowmanScale, mSnowmanOffset));

	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);

	XMMATRIX grassTexScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);
	XMStoreFloat4x4(&mGrassTexTransform, grassTexScale);

	XMMATRIX snowmanBodyTexScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMStoreFloat4x4(&mSnowmanBodyTexTransform, snowmanBodyTexScale);

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

	mLandMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mLandMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mLandMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mSnowmanMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mSnowmanMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSnowmanMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
}

SnowmanApp::~SnowmanApp()
{
	ReleaseCOM(mLandVB);
	ReleaseCOM(mLandIB);
	ReleaseCOM(mSnowmanVB);
	ReleaseCOM(mSnowmanIB);
	ReleaseCOM(mGrassMapSRV);

	Effects::DestroyAll();
	InputLayouts::DestroyAll();
}

bool SnowmanApp::Init()
{
	if (!D3DApp::Init())
		return false;


	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);

	ID3D11Resource* texResource = nullptr;
	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/grass.dds", &texResource, &mGrassMapSRV));

	//HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
	//	L"Textures/snowman_body.dds", &texResource, &mGrassMapSRV));

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/snowman_body.dds", &texResource, &mSnowmanBodyMapSRV));

	BuildLandGeometryBuffers();
	BuildSnowmanGeometryBuffers();

	return true;
}

void SnowmanApp::OnResize()
{
	D3DApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void SnowmanApp::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius * sinf(mPhi)*cosf(mTheta);
	float z = mRadius * sinf(mPhi)*sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	mEyePosW = XMFLOAT3(x, y, z);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	//
	// Every quarter second, generate a random wave.
	//
	//static float t_base = 0.0f;
	//if( (mTimer.TotalTime() - t_base) >= 0.25f )
	//{
	//	t_base += 0.25f;
 //
	//	DWORD i = 5 + rand() % (mWaves.RowCount()-10);
	//	DWORD j = 5 + rand() % (mWaves.ColumnCount()-10);

	//	float r = MathHelper::RandF(1.0f, 2.0f);

	//	mWaves.Disturb(i, j, r);
	//}

	//mWaves.Update(dt);

	//
	// Update the wave vertex buffer with the new solution.
	//

	//D3D11_MAPPED_SUBRESOURCE mappedData;
	//HR(md3dImmediateContext->Map(mWavesVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	//Vertex::Basic32* v = reinterpret_cast<Vertex::Basic32*>(mappedData.pData);
	//for(UINT i = 0; i < mWaves.VertexCount(); ++i)
	//{
	//	v[i].Pos    = mWaves[i];
	//	v[i].Normal = mWaves.Normal(i);

	//	// Derive tex-coords in [0,1] from position.
	//	v[i].Tex.x  = 0.5f + mWaves[i].x / mWaves.Width();
	//	v[i].Tex.y  = 0.5f - mWaves[i].z / mWaves.Depth();
	//}

	//md3dImmediateContext->Unmap(mWavesVB, 0);

	//
	// Animate water texture coordinates.
	//

	//// Tile water texture.
	//XMMATRIX wavesScale = XMMatrixScaling(5.0f, 5.0f, 0.0f);

	//// Translate texture over time.
	//mWaterTexOffset.y += 0.05f*dt;
	//mWaterTexOffset.x += 0.1f*dt;	
	//XMMATRIX wavesOffset = XMMatrixTranslation(mWaterTexOffset.x, mWaterTexOffset.y, 0.0f);

	//// Combine scale and translation.
	//XMStoreFloat4x4(&mWaterTexTransform, wavesScale*wavesOffset);

	//Update the world matrix of snowman
	XMMATRIX mSnowmanScale = XMMatrixScaling(5.0f, 5.0f, 5.0f);
	snowmanMoveRadius = 30.0f;
	snowmanTheta += 2.0f*dt;
	if (snowmanTheta > 2 * MathHelper::Pi)
	{
		snowmanTheta -= 2 * MathHelper::Pi;
	}
	XMMATRIX mSnowmanOffset = XMMatrixTranslation(snowmanMoveRadius*cos(snowmanTheta), 8.0f, snowmanMoveRadius*sin(snowmanTheta));
	XMStoreFloat4x4(&mSnowmanWorld, XMMatrixMultiply(mSnowmanScale, mSnowmanOffset));
}

void SnowmanApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX viewProj = view * proj;

	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mEyePosW);

	//
	// Draw the hills with texture.
	//
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light3TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mLandVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mLandIB, DXGI_FORMAT_R32_UINT, 0);

		// Set per object constants.
		XMMATRIX world = XMLoadFloat4x4(&mLandWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world * view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mGrassTexTransform));
		Effects::BasicFX->SetMaterial(mLandMat);
		Effects::BasicFX->SetDiffuseMap(mGrassMapSRV);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mLandIndexCount, 0, 0);
	}

	//
	// Draw the snowman without texture;
	//
	//activeTech = Effects::BasicFX->Light3Tech;
	activeTech = Effects::BasicFX->Light3TexTech;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p) {
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mSnowmanVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mSnowmanIB, DXGI_FORMAT_R32_UINT, 0);


		XMMATRIX world = XMLoadFloat4x4(&mSnowmanWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world * view*proj;
		// Set per object constants.

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mSnowmanBodyTexTransform));
		Effects::BasicFX->SetMaterial(mSnowmanMat);
		Effects::BasicFX->SetDiffuseMap(mSnowmanBodyMapSRV);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mSnowmanIndexCount, 0, 0);
	}


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

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.01 unit in the scene.
		float dx = 0.05f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.05f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 50.0f, 500.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

float SnowmanApp::GetHillHeight(float x, float z)const
{
	//return 0.02f*( z*sinf(0.1f*x) + x*cosf(0.1f*z) );
	//return rand() / (double)(RAND_MAX)*0.1f*(z*sinf(0.1f*x) + x * cosf(0.1f*z));
	return 0;
}

XMFLOAT3 SnowmanApp::GetHillNormal(float x, float z)const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void SnowmanApp::BuildLandGeometryBuffers()
{
	GeometryGenerator::MeshData grid;

	GeometryGenerator geoGen;

	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);
	//geoGen.CreateGrid(1600.0f, 1600.0f, 500, 500, grid);

	mLandIndexCount = grid.Indices.size();

	//
	// Extract the vertex elements we are interested and apply the height function to
	// each vertex.  
	//

	std::vector<Vertex::Basic32> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		p.y = GetHillHeight(p.x, p.z);

		vertices[i].Pos = p;
		vertices[i].Normal = GetHillNormal(p.x, p.z);
		vertices[i].Tex = grid.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * grid.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mLandVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mLandIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &grid.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mLandIB));
}


void SnowmanApp::BuildSnowmanGeometryBuffers()
{
	GeometryGenerator::MeshData snowman;

	GeometryGenerator geoGen;

	geoGen.CreateSphere(1.0f, 64, 64, snowman);

	mSnowmanIndexCount = snowman.Indices.size();

	//
	// Extract the vertex elements we are interested and apply the height function to
	// each vertex.  
	//

	std::vector<Vertex::Basic32> vertices(snowman.Vertices.size());
	for (size_t i = 0; i < snowman.Vertices.size(); ++i)
	{
		XMFLOAT3 p = snowman.Vertices[i].Position;

		vertices[i].Pos = p;
		vertices[i].Normal = snowman.Vertices[i].Normal;
		vertices[i].Tex = snowman.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * snowman.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mSnowmanVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mSnowmanIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &snowman.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mSnowmanIB));
}
