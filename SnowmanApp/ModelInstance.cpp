#include"ModelInstance.h"

ModelInstance::ModelInstance()
	:isStatic(true),rotationSpeed(0.0f)
{
	posW = XMFLOAT3(0.0f,0.0f,0.0f);
	rotationCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

ModelInstance::~ModelInstance()
{

}

void ModelInstance::BindModels(Model* p)
{
	pModel = p;
}

void ModelInstance::SetPostion(XMFLOAT3 postion)
{
	posW = postion;
}

void ModelInstance::SetAnimationState(XMFLOAT3 posCenter, float radius, float speed)
{
	isStatic = false;
	rotationCenter = posCenter;
	rotationSpeed = speed;
	rotationRadius = radius;

	//compute the origin angle
	float x = posW.x - rotationCenter.x;
	float z = posW.z - rotationCenter.z;
	if (x > 0) {
		rotationTheta = atan(z / x);
	}
	else if (x < 0) {
		rotationTheta = atan(z / x) + MathHelper::Pi;
	}
	else {	//x==0
		if (z >= 0)
			rotationTheta = MathHelper::Pi / 2;
		else
			rotationTheta = MathHelper::Pi / 2 * 3;
	}
}

void ModelInstance::UpdatePostion(float dt)
{
	if (isStatic)
		return;

	rotationTheta += dt * rotationSpeed;
	if (rotationTheta > 2 * MathHelper::Pi)
	{
		rotationTheta -= 2 * MathHelper::Pi;
	}
	posW.x = rotationRadius * cos(rotationTheta) + rotationCenter.x;
	posW.z = rotationRadius * sin(rotationTheta) + rotationCenter.z;
}

void ModelInstance::Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, const Camera& camera)
{
	XMMATRIX mWorld = XMMatrixTranslation(posW.x,posW.y,posW.z);
	pModel->Draw(dc, activeTech, mWorld, camera);
}