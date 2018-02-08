//***************************************************************************************
// Sky.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Models.h"
#include "GeometryGenerator.h"
#include "Camera.h"
#include "Vertex.h"
#include "Effects.h"


SnowmanModel::SnowmanModel(ID3D11Device* devices)
	:device(devices)
{
	
}

void* SnowmanModel::operator new(size_t size)
{
	void* p = _aligned_malloc(size, 16);
	if (p == 0)  throw std::bad_alloc();
	return p;
}

void SnowmanModel::operator delete(void* p)
{
	SnowmanModel* pc = static_cast<SnowmanModel*>(p);
	_aligned_free(p);
}

void SnowmanModel::Init()
{
	ID3D11Resource* texResource = nullptr;
	HR(DirectX::CreateDDSTextureFromFile(device,
		L"Textures/snowman_body.dds", &texResource, &mBodyMapSRV));
	HR(DirectX::CreateDDSTextureFromFile(device,
		L"Textures/snowman_head.dds", &texResource, &mHeadMapSRV));
	HR(DirectX::CreateDDSTextureFromFile(device,
		L"Textures/snowman_head.dds", &texResource, &mArmMapSRV));
	HR(DirectX::CreateDDSTextureFromFile(device,
		L"Textures/snowman_head.dds", &texResource, &mHandMapSRV));
	HR(DirectX::CreateDDSTextureFromFile(device,
		L"Textures/snowman_head.dds", &texResource, &mScarfMapSRV));

	mBodyTexTransform = XMMatrixScaling(1.0f, 1.0f, 0.0f);
	mHeadTexTransform = XMMatrixScaling(1.0f, 1.0f, 0.0f);
	mArmTexTransform = XMMatrixScaling(1.0f, 1.0f, 0.0f);
	mHandTexTransform = XMMatrixScaling(1.0f, 1.0f, 0.0f);
	mScarfTexTransform = XMMatrixScaling(1.0f, 1.0f, 0.0f);

	mMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
	mMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	
	GenerateModel();
}

SnowmanModel::~SnowmanModel()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mBodyMapSRV);
	ReleaseCOM(mHeadMapSRV);
	ReleaseCOM(mArmMapSRV);
	ReleaseCOM(mHandMapSRV);
	ReleaseCOM(mScarfMapSRV);
}


void SnowmanModel::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, CXMMATRIX mWorld, const Camera& camera)
{
	XMMATRIX view = camera.View();
	XMMATRIX proj = camera.Proj();
	XMMATRIX viewProj = camera.ViewProj();

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		dc->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		dc->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(mWorld);
		XMMATRIX worldViewProj = mWorld * view*proj;

		Effects::BasicFX->SetEyePosW(camera.GetPosition());

		Effects::BasicFX->SetWorld(mWorld);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);		
		Effects::BasicFX->SetMaterial(mMat);

		//draw body
		Effects::BasicFX->SetDiffuseMap(mBodyMapSRV);
		Effects::BasicFX->SetTexTransform(mBodyTexTransform);
		activeTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mBodyIndexCount, mBodyIndexOffset, mBodyVertexOffset);

		//draw head
		Effects::BasicFX->SetDiffuseMap(mHeadMapSRV);
		Effects::BasicFX->SetTexTransform(mHeadTexTransform);
		activeTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mHeadIndexCount, mHeadIndexOffset, mHeadVertexOffset);

		//draw left arm
		Effects::BasicFX->SetDiffuseMap(mArmMapSRV);
		Effects::BasicFX->SetTexTransform(mArmTexTransform);
		activeTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mLeftArmIndexCount, mLeftArmIndexOffset, mLeftArmVertexOffset);

		//draw right arm
		Effects::BasicFX->SetDiffuseMap(mArmMapSRV);
		Effects::BasicFX->SetTexTransform(mArmTexTransform);
		activeTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mRightArmIndexCount, mRightArmIndexOffset, mRightArmVertexOffset);

		//draw left hand
		Effects::BasicFX->SetDiffuseMap(mHandMapSRV);
		Effects::BasicFX->SetTexTransform(mHandTexTransform);
		activeTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mLeftHandIndexCount, mLeftHandIndexOffset, mLeftHandVertexOffset);

		//draw right hand
		Effects::BasicFX->SetDiffuseMap(mHandMapSRV);
		Effects::BasicFX->SetTexTransform(mHandTexTransform);
		activeTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mRightHandIndexCount, mRightHandIndexOffset, mRightHandVertexOffset);

		//draw scarf
		Effects::BasicFX->SetDiffuseMap(mScarfMapSRV);
		Effects::BasicFX->SetTexTransform(mScarfTexTransform);
		activeTech->GetPassByIndex(p)->Apply(0, dc);
		dc->DrawIndexed(mScarfIndexCount, mScarfIndexOffset, mScarfVertexOffset);
	}
}

void SnowmanModel::GenerateModel()
{
	//generate basic geometries
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateSphere(1.0f, 20, 20, sphere);
	geoGen.CreateCylinder(1.0f, 1.0f, 1.0f, 20, 20, cylinder);

	//sphere:body, head, left hand, right hand
	//cylinder:left arm, right arm, scarf
	UINT totalVertexCount = 4 * sphere.Vertices.size() + 
							3 * cylinder.Vertices.size();

	UINT totalIndexCount = 4 * sphere.Indices.size() +
							3 * cylinder.Indices.size();

	mBodyVertexOffset = 0;
	mHeadVertexOffset = mBodyVertexOffset + sphere.Vertices.size();
	mLeftArmVertexOffset = mHeadVertexOffset + sphere.Vertices.size();
	mRightArmVertexOffset = mLeftArmVertexOffset + cylinder.Vertices.size();
	mLeftHandVertexOffset = mRightArmVertexOffset + cylinder.Vertices.size();
	mRightHandVertexOffset = mLeftHandVertexOffset + sphere.Vertices.size();
	mScarfVertexOffset = mRightHandVertexOffset + sphere.Vertices.size();

	mBodyIndexCount = sphere.Indices.size();
	mHeadIndexCount = sphere.Indices.size();
	mLeftArmIndexCount = cylinder.Indices.size();
	mRightArmIndexCount = cylinder.Indices.size();
	mLeftHandIndexCount = sphere.Indices.size();
	mRightHandIndexCount = sphere.Indices.size();
	mScarfIndexCount = cylinder.Indices.size();

	mBodyIndexOffset = 0;
	mHeadIndexOffset = mBodyIndexOffset + sphere.Indices.size();
	mLeftArmIndexOffset = mHeadIndexOffset + sphere.Indices.size();
	mRightArmIndexOffset = mLeftArmIndexOffset + cylinder.Indices.size();
	mLeftHandIndexOffset = mRightArmIndexOffset + cylinder.Indices.size();
	mRightHandIndexOffset = mLeftHandIndexOffset + sphere.Indices.size();
	mScarfIndexOffset = mRightHandIndexOffset + sphere.Indices.size();


	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	#pragma region assemble_objects
	UINT k = 0;
	//body, let the center of body be the center of snowman
	//缩放
	XMMATRIX mScale = XMMatrixScaling(bodyRadius, bodyRadius, bodyRadius);
	XMMATRIX mOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMMATRIX mRotation = XMMatrixIdentity();
	XMMATRIX mTrans = mScale * mOffset * mRotation;
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		XMStoreFloat3(&vertices[k].Pos, XMVector3Transform(XMLoadFloat3(&sphere.Vertices[i].Position), mTrans));
		XMStoreFloat3(&vertices[k].Normal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&sphere.Vertices[i].Normal), mTrans)));
		vertices[k].Tex = sphere.Vertices[i].TexC;
	}

	//head
	//缩放后平移
	mScale = XMMatrixScaling(headRadius, headRadius, headRadius);
	mOffset = XMMatrixTranslation(0.0f, headRadius + bodyRadius, 0.0f);
	mRotation = XMMatrixIdentity();
	mTrans = mScale * mOffset * mRotation;
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		XMStoreFloat3(&vertices[k].Pos, XMVector3Transform(XMLoadFloat3(&sphere.Vertices[i].Position), mTrans));
		XMStoreFloat3(&vertices[k].Normal, XMVector3TransformNormal(XMLoadFloat3(&sphere.Vertices[i].Normal), mTrans));
		vertices[k].Tex = sphere.Vertices[i].TexC;
	}

	//left arm
	//缩放平移旋转
	mScale = XMMatrixScaling(armRadius, armLength, armRadius);
	mOffset = XMMatrixTranslation(0.0f, armLength / 2, 0.0f);
	mRotation = XMMatrixRotationX(-armAngle);
	mTrans = mScale * mOffset * mRotation;
	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		XMStoreFloat3(&vertices[k].Pos, XMVector3Transform(XMLoadFloat3(&cylinder.Vertices[i].Position), mTrans));
		XMStoreFloat3(&vertices[k].Normal, XMVector3TransformNormal(XMLoadFloat3(&cylinder.Vertices[i].Normal), mTrans));
		vertices[k].Tex = cylinder.Vertices[i].TexC;
	}

	//right arm
	//缩放平移旋转
	mScale = XMMatrixScaling(armRadius, armLength, armRadius);
	mOffset = XMMatrixTranslation(0.0f, armLength/2, 0.0f);
	mRotation = XMMatrixRotationX(armAngle);
	mTrans = mScale * mOffset * mRotation;
	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		XMStoreFloat3(&vertices[k].Pos, XMVector3Transform(XMLoadFloat3(&cylinder.Vertices[i].Position), mTrans));
		XMStoreFloat3(&vertices[k].Normal, XMVector3TransformNormal(XMLoadFloat3(&cylinder.Vertices[i].Normal), mTrans));
		vertices[k].Tex = cylinder.Vertices[i].TexC;
	}

	//left hand
	//缩放后平移
	mScale = XMMatrixScaling(handRadius, handRadius, handRadius);
	mOffset = XMMatrixTranslation(0.0f, armLength*cos(-armAngle), armLength*sin(-armAngle));
	mRotation = XMMatrixIdentity();
	mTrans = mScale * mOffset * mRotation;
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		XMStoreFloat3(&vertices[k].Pos, XMVector3Transform(XMLoadFloat3(&sphere.Vertices[i].Position), mTrans));
		XMStoreFloat3(&vertices[k].Normal, XMVector3TransformNormal(XMLoadFloat3(&sphere.Vertices[i].Normal), mTrans));
		vertices[k].Tex = sphere.Vertices[i].TexC;
	}

	//right hand
	//缩放后平移
	mScale = XMMatrixScaling(handRadius, handRadius, handRadius);
	mOffset = XMMatrixTranslation(0.0f, armLength*cos(-armAngle), armLength*sin(armAngle));
	mRotation = XMMatrixIdentity();
	mTrans = mScale * mOffset * mRotation;
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		XMStoreFloat3(&vertices[k].Pos, XMVector3Transform(XMLoadFloat3(&sphere.Vertices[i].Position), mTrans));
		XMStoreFloat3(&vertices[k].Normal, XMVector3TransformNormal(XMLoadFloat3(&sphere.Vertices[i].Normal), mTrans));
		vertices[k].Tex = sphere.Vertices[i].TexC;
	}

	//scarf
	//缩放后平移
	mScale = XMMatrixScaling(scarfRadius, scarfLength, scarfRadius);
	mOffset = XMMatrixTranslation(0.0f, bodyRadius, 0.0f);
	mRotation = XMMatrixIdentity();
	mTrans = mScale * mOffset * mRotation;
	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		XMStoreFloat3(&vertices[k].Pos, XMVector3Transform(XMLoadFloat3(&cylinder.Vertices[i].Position), mTrans));
		XMStoreFloat3(&vertices[k].Normal, XMVector3TransformNormal(XMLoadFloat3(&cylinder.Vertices[i].Normal), mTrans));
		vertices[k].Tex = cylinder.Vertices[i].TexC;
	}
	#pragma endregion

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbd, &vinitData, &mVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
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
	HR(device->CreateBuffer(&ibd, &iinitData, &mIB));
}



//void BoxModel::Init()
//{
//	ID3D11Resource* texResource = nullptr;
//	HR(DirectX::CreateDDSTextureFromFile(device,
//		L"Textures/WoodCrate01.dds", &texResource, &mCrateMapSRV));
//
//	mMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
//	mMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//	mMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
//	mMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
//
//	GenerateModel();
//}

//BoxModel::~BoxModel()
//{
//	ReleaseCOM(mVB);
//	ReleaseCOM(mIB);
//	ReleaseCOM(mCrateMapSRV);
//}

//void BoxModel::GenerateModel()
//{
//	GeometryGenerator::MeshData box;	
//	GeometryGenerator geoGen;
//	geoGen.CreateBox(boxLength, boxLength, boxLength, box);
//
//	std::vector<Vertex::Basic32> vertices(box.Vertices.size());
//
//	for (auto i = 0; i < box.Vertices.size(); ++i)
//	{
//		vertices[i].Pos = box.Vertices[i].Position;
//		vertices[i].Normal = box.Vertices[i].Normal;
//		vertices[i].Tex = box.Vertices[i].TexC;
//	}	
//
//	D3D11_BUFFER_DESC vbd;
//	vbd.Usage = D3D11_USAGE_IMMUTABLE;
//	vbd.ByteWidth = sizeof(Vertex::Basic32) * box.Vertices.size();
//	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vbd.CPUAccessFlags = 0;
//	vbd.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA vinitData;
//	vinitData.pSysMem = &vertices[0];
//	HR(device->CreateBuffer(&vbd, &vinitData, &mVB));
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
//	ibd.ByteWidth = sizeof(UINT) * box.Indices.size();
//	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	ibd.CPUAccessFlags = 0;
//	ibd.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA iinitData;
//	iinitData.pSysMem = &indices[0];
//	HR(device->CreateBuffer(&ibd, &iinitData, &mIB));
//}

//SnowmanModel* Models::SnowmanModelData = nullptr;
//void Models::InitAll(ID3D11Device* device)
//{
//	//SnowmanModelData = new SnowmanModel(device);
//	//由于SnowmanModel类里使用了XMMATRIX,要求地址16对齐，
//	//直接使用new的话只有8位对齐，会导致向量运算出错。
//	//也可以在类里重构operator new和operator delete.
//	SnowmanModelData = (SnowmanModel*)_aligned_malloc(sizeof(SnowmanModel),16);
//	SnowmanModelData->SnowmanModel::SnowmanModel(device);
//
//	//BoxModelData = new BoxModel(device);
//}
//
//void Models::DestroyAll()
//{
//	//SafeDelete(SnowmanModelData);
//	SnowmanModelData->~SnowmanModel();
//	_aligned_free(SnowmanModelData);
//	//SafeDelete(BoxModelData);
//}

SnowmanModel* Models::SnowmanModelData = nullptr;
void Models::InitAll(ID3D11Device* device)
{
	SnowmanModelData = new SnowmanModel(device);
	SnowmanModelData->Init();

	//BoxModelData = new BoxModel(device);
}

void Models::DestroyAll()
{
	SafeDelete(SnowmanModelData);
}
