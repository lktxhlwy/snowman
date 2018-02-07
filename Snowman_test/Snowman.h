#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "Objects.h"

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
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	UINT mBoxVertexOffset;
	UINT mGridVertexOffset;
	UINT mSphereVertexOffset;
	UINT mCylinderVertexOffset;

	UINT mBoxIndexCount;
	UINT mGridIndexCount;
	UINT mSphereIndexCount;
	UINT mCylinderIndexCount;

	UINT mBoxIndexOffset;
	UINT mGridIndexOffset;
	UINT mSphereIndexOffset;
	UINT mCylinderIndexOffset;

	ID3D11ShaderResourceView* mGrassMapSRV;
	ID3D11ShaderResourceView* mCrateMapSRV;
	ID3D11ShaderResourceView* mSnowmanBodyMapSRV;
	ID3D11ShaderResourceView* mSnowmanHeadMapSRV;
	ID3D11ShaderResourceView* mSnowmanArmMapSRV;
	ID3D11ShaderResourceView* mSnowmanHandMapSRV;
	ID3D11ShaderResourceView* mSnowmanScarfMapSRV;

private:
	float GetHillHeight(float x, float z)const;
	XMFLOAT3 GetHillNormal(float x, float z)const;
	//void BuildLandGeometryBuffers();
	//void BuildSnowmanGeometryBuffers();
	void BuildGeometryBuffers();
	void BindSourceToSubObjs();
	//void BuildWaveGeometryBuffers();

private:
	std::vector<SubObject> subObjs;
	UINT subObjsCount;

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