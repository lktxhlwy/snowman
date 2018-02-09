#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "camera.h"
#include "Effects.h"
#include "Vertex.h"
#include "ModelInstance.h"
#include "Sky.h"
#include "Models.h"
#include "RenderStates.h"

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
	//float GetHillHeight(float x, float z)const;
	//XMFLOAT3 GetHillNormal(float x, float z)const;
	//void BuildLandGeometryBuffers();
	//void BuildSnowmanGeometryBuffers();
	//void BuildGeometryBuffers();
	//void BindSourceToSubObjs();
	//void BuildWaveGeometryBuffers();

private:

	DirectionalLight mDirLights[3];

	//XMFLOAT4X4 mView;
	//XMFLOAT4X4 mProj;

	//XMFLOAT3 mEyePosW;

	//float mTheta;
	//float mPhi;
	//float mRadius;

	Camera mCam;
	Sky* mSky;

	POINT mLastMousePos;

	ModelInstance snowman;
	ModelInstance snowmanStatic;
	ModelInstance box;
	ModelInstance terrain;
};