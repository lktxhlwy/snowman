#ifndef MODELINSTANCE_H
#define MODELINSTANCE_H

#include "Models.h"
class ModelInstance
{
public:
	ModelInstance();
	~ModelInstance();

	void BindModels(Model* p);

	void SetPostion(XMFLOAT3 postion);
	void SetAnimationState(XMFLOAT3 posCenter, float radius, float speed);

	void UpdatePostion(float dt);
	void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, const Camera& camera);

private:
	Model * pModel;

	bool isStatic;
	XMFLOAT3 posW;
	XMFLOAT3 rotationCenter;
	float rotationSpeed;
	float rotationRadius;
	float rotationTheta;

};
#endif