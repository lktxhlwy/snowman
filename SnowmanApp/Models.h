//***************************************************************************************
// Models.h by Li Kai.
//   
// 
//***************************************************************************************

#ifndef MODELS_H
#define MODELS_H

#include "d3dUtil.h"

class Camera;

class Model
{
public:
	virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, CXMMATRIX mWorld, const Camera& camera) = 0;;
};

__declspec(align(16)) class SnowmanModel:public Model
{
public:
	SnowmanModel(ID3D11Device* device);
	~SnowmanModel();

	void* operator new(size_t size);
	void operator delete(void *p);

	void Init();
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, CXMMATRIX mWorld,const Camera& camera);

private:
	SnowmanModel(const SnowmanModel& rhs);
	SnowmanModel& operator=(const SnowmanModel& rhs);

	void GenerateModel();

private:
	ID3D11Device * device;
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	UINT mTotalIndexCount;

	UINT mBodyVertexOffset;
	UINT mHeadVertexOffset;
	UINT mLeftArmVertexOffset;
	UINT mRightArmVertexOffset;
	UINT mLeftHandVertexOffset;
	UINT mRightHandVertexOffset;
	UINT mScarfVertexOffset;

	UINT mBodyIndexCount;
	UINT mHeadIndexCount;
	UINT mLeftArmIndexCount;
	UINT mRightArmIndexCount;
	UINT mLeftHandIndexCount;
	UINT mRightHandIndexCount;
	UINT mScarfIndexCount;

	UINT mBodyIndexOffset;
	UINT mHeadIndexOffset;
	UINT mLeftArmIndexOffset;
	UINT mRightArmIndexOffset;
	UINT mLeftHandIndexOffset;
	UINT mRightHandIndexOffset;
	UINT mScarfIndexOffset;

	float bodyRadius = 4.0f;
	float headRadius = 2.0f;
	float armRadius = 0.2f;
	float armLength = 8.0f;
	float armAngle = 0.25f*MathHelper::Pi; 
	float handRadius = 0.5f;
	float scarfRadius = 1.2f;
	float scarfLength = 2.0f;

private:
	ID3D11ShaderResourceView* mBodyMapSRV;
	ID3D11ShaderResourceView* mHeadMapSRV;
	ID3D11ShaderResourceView* mArmMapSRV;
	ID3D11ShaderResourceView* mHandMapSRV;
	ID3D11ShaderResourceView* mScarfMapSRV;

	XMMATRIX mBodyTexTransform;
	XMMATRIX mHeadTexTransform;
	XMMATRIX mArmTexTransform;
	XMMATRIX mHandTexTransform;
	XMMATRIX mScarfTexTransform;

	Material mMat;
};

//__declspec(align(16)) class BoxModel:public Model
//{
//public:
//	BoxModel(ID3D11Device* device);
//	~BoxModel();
//
//	void Init();
//	void Draw(ID3D11DeviceContext* dc, const Camera& camera, CXMMATRIX mWorld);
//
//private:
//	BoxModel(const BoxModel& rhs);
//	BoxModel& operator=(const BoxModel& rhs);
//	void GenerateModel();
//
//private:
//	ID3D11Device * device;
//
//	ID3D11Buffer* mVB;
//	ID3D11Buffer* mIB;
//
//	UINT mTotalIndexCount;	
//	float boxLength = 8.0f;
//
//private:
//	ID3D11Device * device;
//	ID3D11Buffer* mVB;
//	ID3D11Buffer* mIB;
//	ID3D11ShaderResourceView * mCrateMapSRV;
//	Material mMat;
//};

class Models
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static SnowmanModel* SnowmanModelData;
	//static BoxModel* BoxModelData;
};

#endif //MODELS_H