//***************************************************************************************
// Model.cpp by Li Kai.
//***************************************************************************************


#include "ClassModel.h"
#include "Camera.h"
#include "Vertex.h"
#include "Effects.h"

void SubModel::SetTexResource(ID3D11ShaderResourceView* mSRV,CXMMATRIX mTransform,Material mat)
{
	mTexMapSRV = mSRV;
	mTexTransform = mTransform;
	mMat = mat;
}

void SubModel::SetBufferResource(UINT vc, UINT vo, UINT ic, UINT io)
{
	vertexCount = vc;
	vertexOffset = vo;
	indexCount = ic;
	indexOffset = io;
}

Model::Model(ID3D11Device* de)
	:device(de), mVB(nullptr), mIB(nullptr) {}

Model::~Model()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void Model::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, CXMMATRIX mWorld, const Camera& camera)
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

		for (size_t i = 0; i < Subs.size(); i++)
		{
			Effects::BasicFX->SetMaterial(Subs[i].mMat);
			Effects::BasicFX->SetDiffuseMap(Subs[i].mTexMapSRV);
			Effects::BasicFX->SetTexTransform(Subs[i].mTexTransform);
			activeTech->GetPassByIndex(p)->Apply(0, dc);
			dc->DrawIndexed(Subs[i].indexCount, Subs[i].indexOffset, Subs[i].vertexOffset);
		}
	}
}

void Model::AssembleSubModel(std::vector<Vertex::Basic32> &vertices, std::vector<UINT> &indices, GeometryGenerator::MeshData &geo, CXMMATRIX mTrans, SubModel& sub)
{
	sub.indexOffset = indices.size();
	sub.indexCount = geo.Indices.size();
	indices.insert(indices.end(), geo.Indices.begin(), geo.Indices.end());

	sub.vertexCount = geo.Vertices.size();
	sub.vertexOffset = vertices.size();

	std::vector<Vertex::Basic32> temp(sub.vertexCount);
	vertices.insert(vertices.end(), temp.begin(), temp.end());

	UINT k = sub.vertexOffset;
	for (size_t i = 0; i < geo.Vertices.size(); ++i, ++k)
	{
		XMStoreFloat3(&vertices[k].Pos, XMVector3Transform(XMLoadFloat3(&geo.Vertices[i].Position), mTrans));
		XMStoreFloat3(&vertices[k].Normal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&geo.Vertices[i].Normal), mTrans)));
		vertices[k].Tex = geo.Vertices[i].TexC;
	}
}



SnowmanModel::SnowmanModel(ID3D11Device* de)
	:Model(de) {}

SnowmanModel::~SnowmanModel() {}

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
	TexMgr.Init(device);
	TexMgr.CreateTexture(L"Textures/snowman_body.dds");
	TexMgr.CreateTexture(L"Textures/snowman_head.dds");
	TexMgr.CreateTexture(L"Textures/snowman_arm.dds");
	TexMgr.CreateTexture(L"Textures/snowman_hand.dds");
	TexMgr.CreateTexture(L"Textures/snowman_scarf.dds");

	XMMATRIX mTexTransform = XMMatrixScaling(1.0f, 1.0f, 0.0f);

	Material mMat;
	mMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
	mMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	std::vector<SubModel> temp(7);
	Subs = temp;
	
	Subs[0].SetTexResource(TexMgr[L"Textures/snowman_body.dds"], mTexTransform, mMat);	//body
	Subs[1].SetTexResource(TexMgr[L"Textures/snowman_head.dds"], mTexTransform, mMat);	//head
	Subs[2].SetTexResource(TexMgr[L"Textures/snowman_arm.dds"], mTexTransform, mMat);	//left arm
	Subs[3].SetTexResource(TexMgr[L"Textures/snowman_arm.dds"], mTexTransform, mMat);	//right arm
	Subs[4].SetTexResource(TexMgr[L"Textures/snowman_hand.dds"], mTexTransform, mMat);	//left hand
	Subs[5].SetTexResource(TexMgr[L"Textures/snowman_hand.dds"], mTexTransform, mMat);	//right hand
	Subs[6].SetTexResource(TexMgr[L"Textures/snowman_scarf.dds"], mTexTransform, mMat);	//scarf

	GenerateModel();
}

void SnowmanModel::GenerateModel()
{
	//generate basic geometries
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateSphere(1.0f, 20, 20, sphere);
	geoGen.CreateCylinder(1.0f, 1.0f, 1.0f, 20, 20, cylinder);
	
	std::vector<Vertex::Basic32> vertices;
	std::vector<UINT> indices;

#pragma region assemble_objects
	//body, let the center of body be the center of snowman
	//缩放
	XMMATRIX mScale = XMMatrixScaling(bodyRadius, bodyRadius, bodyRadius);
	XMMATRIX mOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMMATRIX mRotation = XMMatrixIdentity();
	XMMATRIX mTrans = mScale * mOffset * mRotation;
	AssembleSubModel(vertices, indices, sphere, mTrans, Subs[0]);

	//head
	//缩放后平移
	mScale = XMMatrixScaling(headRadius, headRadius, headRadius);
	mOffset = XMMatrixTranslation(0.0f, headRadius + bodyRadius, 0.0f);
	mRotation = XMMatrixIdentity();
	mTrans = mScale * mOffset * mRotation;
	AssembleSubModel(vertices, indices, sphere, mTrans, Subs[1]);

	//left arm
	//缩放平移旋转
	mScale = XMMatrixScaling(armRadius, armLength, armRadius);
	mOffset = XMMatrixTranslation(0.0f, armLength / 2, 0.0f);
	mRotation = XMMatrixRotationX(-armAngle);
	mTrans = mScale * mOffset * mRotation;
	AssembleSubModel(vertices, indices, cylinder, mTrans, Subs[2]);

	//right arm
	//缩放平移旋转
	mScale = XMMatrixScaling(armRadius, armLength, armRadius);
	mOffset = XMMatrixTranslation(0.0f, armLength / 2, 0.0f);
	mRotation = XMMatrixRotationX(armAngle);
	mTrans = mScale * mOffset * mRotation;
	AssembleSubModel(vertices, indices, cylinder, mTrans, Subs[3]);

	//left hand
	//缩放后平移
	mScale = XMMatrixScaling(handRadius, handRadius, handRadius);
	mOffset = XMMatrixTranslation(0.0f, armLength*cos(-armAngle), armLength*sin(-armAngle));
	mRotation = XMMatrixIdentity();
	mTrans = mScale * mOffset * mRotation;
	AssembleSubModel(vertices, indices, sphere, mTrans, Subs[4]);

	//right hand
	//缩放后平移
	mScale = XMMatrixScaling(handRadius, handRadius, handRadius);
	mOffset = XMMatrixTranslation(0.0f, armLength*cos(-armAngle), armLength*sin(armAngle));
	mRotation = XMMatrixIdentity();
	mTrans = mScale * mOffset * mRotation;
	AssembleSubModel(vertices, indices, sphere, mTrans, Subs[5]);

	//scarf
	//缩放后平移
	mScale = XMMatrixScaling(scarfRadius, scarfLength, scarfRadius);
	mOffset = XMMatrixTranslation(0.0f, bodyRadius, 0.0f);
	mRotation = XMMatrixIdentity();
	mTrans = mScale * mOffset * mRotation;
	AssembleSubModel(vertices, indices, sphere, mTrans, Subs[6]);
#pragma endregion

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbd, &vinitData, &mVB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(device->CreateBuffer(&ibd, &iinitData, &mIB));
}


BoxModel::BoxModel(ID3D11Device* de)
	:Model(de) {}

BoxModel::~BoxModel() {}

void* BoxModel::operator new(size_t size)
{
	void* p = _aligned_malloc(size, 16);
	if (p == 0)  throw std::bad_alloc();
	return p;
}

void BoxModel::operator delete(void* p)
{
	BoxModel* pc = static_cast<BoxModel*>(p);
	_aligned_free(p);
}

void BoxModel::Init()
{
	TexMgr.Init(device);
	TexMgr.CreateTexture(L"Textures/WoodCrate01.dds");
	XMMATRIX mTexTransform = XMMatrixScaling(1.0f, 1.0f, 0.0f);

	Material mMat;
	mMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
	mMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	//only a box
	std::vector<SubModel> temp(1);
	Subs = temp;

	Subs[0].SetTexResource(TexMgr[L"Textures/WoodCrate01.dds"], mTexTransform, mMat);

	GenerateModel();
}


void BoxModel::GenerateModel()
{
	GeometryGenerator::MeshData box;	
	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

	std::vector<Vertex::Basic32> vertices;
	std::vector<UINT> indices;

	XMMATRIX mScale = XMMatrixScaling(boxLength, boxLength, boxLength);
	XMMATRIX mOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMMATRIX mRotation = XMMatrixIdentity();
	XMMATRIX mTrans = mScale * mOffset * mRotation;
	AssembleSubModel(vertices, indices, box, mTrans, Subs[0]);


	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * box.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbd, &vinitData, &mVB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * box.Indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(device->CreateBuffer(&ibd, &iinitData, &mIB));
}

