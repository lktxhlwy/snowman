#ifndef MODELINSTANCE_H
#define MODELINSTANCE_H

#include "ClassModel.h"
#include "TerrainModel.h"
class ModelInstance
{
public:
	ModelInstance();
	~ModelInstance();

	virtual void BindModels(Model* p);

	void SetPostion(XMFLOAT3 postion);
	XMFLOAT3 GetPostion();
	XMFLOAT3 GetRotationCenter();
	void SetAnimationState(XMFLOAT3 posCenter, float radius, float speed);

	void UpdatePostion(float dt);
	virtual void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, const Camera& camera);

	bool IsInInstance(XMFLOAT3 pos);
protected:
	Model * pModel;
	bool isStatic;
	XMFLOAT3 posW;
	XMFLOAT3 rotationCenter;
	float rotationSpeed;
	float rotationRadius;
	float rotationTheta;
};

class TarrainInstance :public ModelInstance
{
public:
	float GetHeight(XMFLOAT3 pos);
	void BindModels(TerrainModel* p);
	//void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, const Camera& camera);
private:
	TerrainModel* pTerrainModel;	
};



#endif //MODELINSTANCE_H