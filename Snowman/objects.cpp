#include "Objects.h"
#include "GeometryGenerator.h"
#include "Vertex.h"
#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "MathHelper.h"

SubObject::SubObject() 
	:mTexMapSRV(nullptr), vertexOffset(0), indexOffset(0), indexCount(0),
	isMoving(false), moveRadius(0.0f), moveSpeed(0.0f)
{}


SubObject::~SubObject()
{
	//ReleaseCOM(mTexMapSRV);
}


void SubObject::setMaterial(Material inputMat)
{
	mMat.Ambient = inputMat.Ambient;
	mMat.Diffuse = inputMat.Diffuse;
	mMat.Specular = inputMat.Diffuse;
}

void SubObject::setTexture(ID3D11ShaderResourceView* mInputTexMapSRV, CXMMATRIX mInputTexTransform)
{
	mTexMapSRV = mInputTexMapSRV;
	mTexTransform = mInputTexTransform;
}

void SubObject::setBufferOffsetAndCount(UINT vOffset, UINT iOffset, UINT iCount)
{
	vertexOffset = vOffset;
	indexOffset = iOffset;
	indexCount = iCount;
}

void SubObject::setScaleAndOffset(CXMMATRIX mInputScale, float x, float y, float z)
{
	mScale = mInputScale;
	posxW = x;
	posyW = y;
	poszW = z;
	mOffset = XMMatrixTranslation(posxW, posyW, poszW);
}

void SubObject::setMovingState(float radius,float speed)
{
	isMoving = true;
	moveRadius = radius;
	moveSpeed = speed;
}

static inline float arctan(float x, float z)
{
	if (x > 0) {
		return atan(z / x);
	}
	else if (x < 0) {
		return atan(z / x) + MathHelper::Pi;
	}
	else {	//x==0
		if (z >= 0)
			return MathHelper::Pi / 2;
		else
			return MathHelper::Pi / 2 * 3;
	}
}

void SubObject::mOffsetUpdate(float dt)
{
	if (isMoving) {

		float theta = arctan(posxW , poszW);
		theta += dt * moveSpeed;
		if (theta > 2 * MathHelper::Pi)
		{
			theta -= 2 * MathHelper::Pi;
		}
		posxW = moveRadius * cos(theta);
		poszW = moveRadius * sin(theta);

		mOffset = XMMatrixTranslation(posxW, posyW,poszW);
	}
}