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

#include "Snowman.h"

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
	: D3DApp(hInstance), mVB(0), mIB(0), mGrassMapSRV(0),
	mEyePosW(0.0f, 0.0f, 0.0f), mLandIndexCount(0), mTheta(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(2.5f)
{
	mMainWndCaption = L"Snowman_test";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

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

	subObjsCount = 1;
	std::vector<SubObject> subs(subObjsCount);
	subObjs = subs;

}

SnowmanApp::~SnowmanApp()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mGrassMapSRV);
	ReleaseCOM(mCrateMapSRV);
	ReleaseCOM(mSnowmanBodyMapSRV);
	ReleaseCOM(mSnowmanHeadMapSRV);
	ReleaseCOM(mSnowmanArmMapSRV);
	ReleaseCOM(mSnowmanHandMapSRV);
	ReleaseCOM(mSnowmanScarfMapSRV);

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

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/WoodCrate01.dds", &texResource, &mCrateMapSRV));

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/snowman_body.dds", &texResource, &mSnowmanBodyMapSRV));

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
		L"Textures/snowman_body.dds", &texResource, &mSnowmanHeadMapSRV));

	//HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
	//	L"Textures/snowman_arm.dds", &texResource, &mSnowmanArmMapSRV));

	//HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
	//	L"Textures/snowman_hand.dds", &texResource, &mSnowmanHandMapSRV));

	//HR(DirectX::CreateDDSTextureFromFile(md3dDevice,
	//	L"Textures/snowman_scarf.dds", &texResource, &mSnowmanScarfMapSRV));

	BuildGeometryBuffers();
	BindSourceToSubObjs();

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
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light2TexTech;
	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

		for (size_t i = 0; i < subObjs.size(); i++)
		{
			XMMATRIX world = subObjs[i].mScale * subObjs[i].mOffset;
			//world = XMMatrixIdentity();
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(subObjs[i].mTexTransform);
			Effects::BasicFX->SetMaterial(subObjs[i].mMat);
			Effects::BasicFX->SetDiffuseMap(subObjs[i].mTexMapSRV);

			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(subObjs[i].indexCount, subObjs[i].indexOffset, subObjs[i].vertexOffset);
			md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

			/*XMMATRIX world = XMMatrixIdentity();
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world * view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Material mTemp;
			mTemp.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			mTemp.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			mTemp.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
			Effects::BasicFX->SetMaterial(mTemp);
			Effects::BasicFX->SetDiffuseMap(mCrateMapSRV);

			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);*/
		}
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

//void SnowmanApp::BuildLandGeometryBuffers()
//{
//	GeometryGenerator::MeshData grid;
//
//	GeometryGenerator geoGen;
//
//	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);
//	//geoGen.CreateGrid(1600.0f, 1600.0f, 500, 500, grid);
//
//	mLandIndexCount = grid.Indices.size();
//
//	//
//	// Extract the vertex elements we are interested and apply the height function to
//	// each vertex.  
//	//
//
//	std::vector<Vertex::Basic32> vertices(grid.Vertices.size());
//	for (size_t i = 0; i < grid.Vertices.size(); ++i)
//	{
//		XMFLOAT3 p = grid.Vertices[i].Position;
//
//		p.y = GetHillHeight(p.x, p.z);
//
//		vertices[i].Pos = p;
//		vertices[i].Normal = GetHillNormal(p.x, p.z);
//		vertices[i].Tex = grid.Vertices[i].TexC;
//	}
//
//	D3D11_BUFFER_DESC vbd;
//	vbd.Usage = D3D11_USAGE_IMMUTABLE;
//	vbd.ByteWidth = sizeof(Vertex::Basic32) * grid.Vertices.size();
//	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vbd.CPUAccessFlags = 0;
//	vbd.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA vinitData;
//	vinitData.pSysMem = &vertices[0];
//	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mLandVB));
//
//	//
//	// Pack the indices of all the meshes into one index buffer.
//	//
//
//	D3D11_BUFFER_DESC ibd;
//	ibd.Usage = D3D11_USAGE_IMMUTABLE;
//	ibd.ByteWidth = sizeof(UINT) * mLandIndexCount;
//	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	ibd.CPUAccessFlags = 0;
//	ibd.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA iinitData;
//	iinitData.pSysMem = &grid.Indices[0];
//	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mLandIB));
//}


//void SnowmanApp::BuildSnowmanGeometryBuffers()
//{
//	GeometryGenerator::MeshData snowman;
//
//	GeometryGenerator geoGen;
//
//	geoGen.CreateSphere(1.0f, 64, 64, snowman);
//
//	mSnowmanIndexCount = snowman.Indices.size();
//
//	//
//	// Extract the vertex elements we are interested and apply the height function to
//	// each vertex.  
//	//
//
//	std::vector<Vertex::Basic32> vertices(snowman.Vertices.size());
//	for (size_t i = 0; i < snowman.Vertices.size(); ++i)
//	{
//		XMFLOAT3 p = snowman.Vertices[i].Position;
//
//		vertices[i].Pos = p;
//		vertices[i].Normal = snowman.Vertices[i].Normal;
//		vertices[i].Tex = snowman.Vertices[i].TexC;
//	}
//
//	D3D11_BUFFER_DESC vbd;
//	vbd.Usage = D3D11_USAGE_IMMUTABLE;
//	vbd.ByteWidth = sizeof(Vertex::Basic32) * snowman.Vertices.size();
//	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vbd.CPUAccessFlags = 0;
//	vbd.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA vinitData;
//	vinitData.pSysMem = &vertices[0];
//	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mSnowmanVB));
//
//	//
//	// Pack the indices of all the meshes into one index buffer.
//	//
//
//	D3D11_BUFFER_DESC ibd;
//	ibd.Usage = D3D11_USAGE_IMMUTABLE;
//	ibd.ByteWidth = sizeof(UINT) * mSnowmanIndexCount;
//	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	ibd.CPUAccessFlags = 0;
//	ibd.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA iinitData;
//	iinitData.pSysMem = &snowman.Indices[0];
//	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mSnowmanIB));
//}

//void SnowmanApp::BuildGeometryBuffers()
//{
//	GeometryGenerator::MeshData box;
//
//	GeometryGenerator geoGen;
//	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
//
//	// Cache the vertex offsets to each object in the concatenated vertex buffer.
//	mBoxVertexOffset = 0;
//
//	// Cache the index count of each object.
//	mBoxIndexCount = box.Indices.size();
//
//	// Cache the starting index for each object in the concatenated index buffer.
//	mBoxIndexOffset = 0;
//
//	UINT totalVertexCount = box.Vertices.size();
//
//	UINT totalIndexCount = mBoxIndexCount;
//
//	//
//	// Extract the vertex elements we are interested in and pack the
//	// vertices of all the meshes into one vertex buffer.
//	//
//
//	std::vector<Vertex::Basic32> vertices(totalVertexCount);
//
//	UINT k = 0;
//	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
//	{
//		vertices[k].Pos = box.Vertices[i].Position;
//		vertices[k].Normal = box.Vertices[i].Normal;
//		vertices[k].Tex = box.Vertices[i].TexC;
//	}
//
//	D3D11_BUFFER_DESC vbd;
//	vbd.Usage = D3D11_USAGE_IMMUTABLE;
//	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
//	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vbd.CPUAccessFlags = 0;
//	vbd.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA vinitData;
//	vinitData.pSysMem = &vertices[0];
//	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));
//
//	//
//	// Pack the indices of all the meshes into one index buffer.
//	//
//
//	std::vector<UINT> indices;
//	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
//
//	D3D11_BUFFER_DESC ibd;
//	ibd.Usage = D3D11_USAGE_IMMUTABLE;
//	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
//	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	ibd.CPUAccessFlags = 0;
//	ibd.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA iinitData;
//	iinitData.pSysMem = &indices[0];
//	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
//}


void SnowmanApp::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(160.0f, 160.0f, 50, 50, grid);
	geoGen.CreateSphere(1.0f, 20, 20, sphere);
	geoGen.CreateCylinder(1.0f, 1.0f, 1.0f, 20, 20, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset = 0;
	mGridVertexOffset = box.Vertices.size();
	mSphereVertexOffset = mGridVertexOffset + grid.Vertices.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();

	// Cache the index count of each object.
	mBoxIndexCount = box.Indices.size();
	mGridIndexCount = grid.Indices.size();
	mSphereIndexCount = sphere.Indices.size();
	mCylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset = 0;
	mGridIndexOffset = mBoxIndexCount;
	mSphereIndexOffset = mGridIndexOffset + mGridIndexCount;
	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;

	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount =
		mBoxIndexCount +
		mGridIndexCount +
		mSphereIndexCount +
		mCylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex = box.Vertices[i].TexC;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].Tex = grid.Vertices[i].TexC;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].Tex = sphere.Vertices[i].TexC;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].Tex = cylinder.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void SnowmanApp::BindSourceToSubObjs()
{
	//0-terrian
	//1-box
	//2-snowman body,3-snowman head,4-snowman left hand,5-snowman right hand,6-snowman left arm,7-snowman right arm,8-snowman scarf ---- static 
	//9-snowman body,10-snowman head,11-snowman left hand,12-snowman right hand,13-snowman left arm,14-snowman right arm,15-snowman scarf ---- moving
	for (size_t i = 0; i < subObjs.size(); i++) {
		switch (i) {
		//case 1: //0-terrian
		//	subObjs[i].setTexture(mGrassMapSRV, XMMatrixScaling(5.0f, 5.0f, 5.0f));
		//	subObjs[i].setMaterial(ObjsData::mGridMat);
		//	subObjs[i].setBufferOffsetAndCount(
		//		mGridVertexOffset,
		//		mGridIndexOffset,
		//		mGridIndexCount);
		//	subObjs[i].setScaleAndOffset(
		//		XMMatrixScaling(1.0f, 1.0f, 1.0f),
		//		XMMatrixTranslation(0.0f, 0.0f, 0.0f));
		//	break;
		case 0: //1-box
			subObjs[i].setTexture(mCrateMapSRV, XMMatrixScaling(1.0f, 1.0f, 1.0f));
			subObjs[i].setMaterial(ObjsData::mOtherMat);
			subObjs[i].setBufferOffsetAndCount(
				mBoxVertexOffset,
				mBoxIndexOffset,
				mBoxIndexCount);
			subObjs[i].setScaleAndOffset(
				XMMatrixScaling(1.0f, 1.0f, 1.0f),
				XMMatrixTranslation(0.0f, 0.0f, 0.0f));
			subObjs[i].setMovingState(ObjsData::moveRadius, ObjsData::moveSpeed);
			break;
		//case 2: //snowman body static
		//	subObjs[i].setTexture(mSnowmanBodyMapSRV, XMMatrixScaling(1.0f, 1.0f, 0.0f));
		//	subObjs[i].setMaterial(ObjsData::mOtherMat);
		//	subObjs[i].setBufferOffsetAndCount(
		//		mSphereVertexOffset,
		//		mSphereIndexOffset,
		//		mSphereIndexCount);
		//	subObjs[i].setScaleAndOffset(
		//		XMMatrixScaling(ObjsData::snowmanBodyRadius, ObjsData::snowmanBodyRadius, ObjsData::snowmanBodyRadius),
		//		XMMatrixTranslation(0.0f, ObjsData::snowmanBodyRadius, 0.0f));
		//	break;
		//case 3: //snowman head static
		//	subObjs[i].setTexture(mSnowmanHeadMapSRV, XMMatrixScaling(1.0f, 1.0f, 0.0f));
		//	subObjs[i].setMaterial(ObjsData::mOtherMat);
		//	subObjs[i].setBufferOffsetAndCount(
		//		mSphereVertexOffset,
		//		mSphereIndexOffset,
		//		mSphereIndexCount);
		//	subObjs[i].setScaleAndOffset(
		//		XMMatrixScaling(ObjsData::snowmanHeadRadius, ObjsData::snowmanHeadRadius, ObjsData::snowmanHeadRadius),
		//		XMMatrixTranslation(0.0f, ObjsData::snowmanBodyRadius * 2 + ObjsData::snowmanHeadRadius - 0.5f, 0.0f));
		//	break;
		//case 4: //snowman body moving
		//	subObjs[i].setTexture(mSnowmanBodyMapSRV, XMMatrixScaling(1.0f, 1.0f, 0.0f));
		//	subObjs[i].setMaterial(ObjsData::mOtherMat);
		//	subObjs[i].setBufferOffsetAndCount(
		//		mSphereVertexOffset,
		//		mSphereIndexOffset,
		//		mSphereIndexCount);
		//	subObjs[i].setScaleAndOffset(
		//		XMMatrixScaling(ObjsData::snowmanBodyRadius, ObjsData::snowmanBodyRadius, ObjsData::snowmanBodyRadius),
		//		XMMatrixTranslation(ObjsData::moveRadius, ObjsData::snowmanBodyRadius + ObjsData::boxLength, 0.0f));
		//	subObjs[i].setMovingState(ObjsData::moveRadius, ObjsData::moveSpeed);
		//	break;
		//case 5: //snowman head moving
		//	subObjs[i].setTexture(mSnowmanBodyMapSRV, XMMatrixScaling(1.0f, 1.0f, 0.0f));
		//	subObjs[i].setMaterial(ObjsData::mOtherMat);
		//	subObjs[i].setBufferOffsetAndCount(
		//		mSphereVertexOffset,
		//		mSphereIndexOffset,
		//		mSphereIndexCount);
		//	subObjs[i].setScaleAndOffset(
		//		XMMatrixScaling(ObjsData::snowmanHeadRadius, ObjsData::snowmanHeadRadius, ObjsData::snowmanHeadRadius),
		//		XMMatrixTranslation(ObjsData::moveRadius, ObjsData::snowmanHeadRadius + ObjsData::boxLength + ObjsData::snowmanBodyRadius * 2 -0.5f, 0.0f));
		//	subObjs[i].setMovingState(ObjsData::moveRadius, ObjsData::moveSpeed);
		default:
			break;
		}
	}
}