//***************************************************************************************
// Model.h by Li Kai.
// 
//***************************************************************************************

#ifndef MODEL_H
#define MODEL_H

#include "d3dUtil.h"
#include "TextureMgr.h"
#include "Vertex.h"
#include "GeometryGenerator.h"

class Camera;

//use SubModels to assemble total model
__declspec(align(16)) class SubModel
{
public:
	ID3D11ShaderResourceView* mTexMapSRV;
	XMMATRIX mTexTransform;
	Material mMat;

	UINT vertexCount;
	UINT vertexOffset;
	UINT indexCount;
	UINT indexOffset;

	void SetTexResource(ID3D11ShaderResourceView* mSRV, CXMMATRIX mTransform, Material mat);
	void SetBufferResource(UINT vc, UINT vo, UINT ic, UINT io);
};


//universal model class
class Model
{
public:
	Model(ID3D11Device* de);
	~Model();

	virtual void Init() = 0;
	virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, CXMMATRIX mWorld, const Camera& camera);
	virtual void GenerateModel() = 0;

protected:
	virtual void AssembleSubModel(std::vector<Vertex::Basic32> &vertices, std::vector<UINT> &indices, GeometryGenerator::MeshData &geo, CXMMATRIX mTrans, SubModel& sub);

protected:
	ID3D11Device* device;
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	std::vector<SubModel> Subs;
	TextureMgr TexMgr;		//统一管理该模型加载的纹理
};


//snowman model
__declspec(align(16)) class SnowmanModel :public Model
{
public:
	SnowmanModel(ID3D11Device* de);
	~SnowmanModel();

	void* operator new(size_t size);
	void operator delete(void *p);

	void Init();
	//void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, CXMMATRIX mWorld, const Camera& camera);

private:
	SnowmanModel(const SnowmanModel& rhs);
	SnowmanModel& operator=(const SnowmanModel& rhs);

	void GenerateModel();

private:
	float bodyRadius = 4.0f;
	float headRadius = 2.0f;
	float armRadius = 0.2f;
	float armLength = 8.0f;
	float armAngle = 0.25f*MathHelper::Pi;
	float handRadius = 0.5f;
	float scarfRadius = 1.2f;
	float scarfLength = 2.0f;
};


//box model
__declspec(align(16)) class BoxModel:public Model
{
public:
	BoxModel(ID3D11Device* device);
	~BoxModel();

	void* operator new(size_t size);
	void operator delete(void* p);

	void Init();
	//void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, CXMMATRIX mWorld, const Camera& camera);

private:
	BoxModel(const BoxModel& rhs);
	BoxModel& operator=(const BoxModel& rhs);
	void GenerateModel();

private:
	float boxLength = 8.0f;
};


#endif //MODEL_H