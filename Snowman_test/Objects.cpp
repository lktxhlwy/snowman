#include "Objects.h"
#include "GeometryGenerator.h"
#include "Vertex.h"

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

void SubObject::setScaleAndOffset(CXMMATRIX mInputScale, CXMMATRIX mInputOffset)
{
	mScale = mInputScale;
	mOffset = mInputOffset;
}

void SubObject::setMovingState(float radius,float speed)
{
	isMoving = true;
	moveRadius = radius;
	moveSpeed = speed;
}