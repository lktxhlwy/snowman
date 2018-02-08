//***************************************************************************************
// Objects.h by Li Kai (C) 2018 All Rights Reserved.
//
// Defines class Objects including its generate method, vertex buffer, index buffer, 
// texMapSRVand its world matrix update method.	
//
//***************************************************************************************

#ifndef OBJECTS_H
#define OBJECTS_H
#include "d3dUtil.h"

enum GeometryType
{
	GEOMETRY_SPHERE,
	GEOMETRY_CYLINDER,
	GEOMETRY_BOX,
	GEOMETRY_GRID
};

enum MaterialType
{
	MATERIAL_GRID,
	MATERIAL_OTHER
};

enum SnowmanPartType
{
	SNOWMANPART_BODY,
	SNOWMANPART_HEAD,
	SNOWMANPART_ARM,
	SNOWMANPART_HAND
};

namespace ObjsData
{
	const float snowmanBodyRadius = 4.0f;
	const float snowmanHeadRadius = 2.0f;
	const float snowmanArmLength = 2.0f;
	const float snowmanArmRadius = 0.5f;
	const float snowmanHandRadius = 1.0f;
	const float snowmanScarfRadius = 1.0f;
	const float snowmanScarfLength = 2.0f;

	const float boxLength = 8.0f;

	const float moveRadius = 30.0f;
	const float moveSpeed = 0.5f;

	const Material mGridMat(
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	const Material mOtherMat(
		XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
}

class SubObject
{
public:
	SubObject();
	virtual ~SubObject();
	//void addResource(ID3D11ShaderResourceView* mTexMapSRV, XMFLOAT4X4 mTexTransform, Material mMat)

	void setMaterial(Material mMat);
	void setTexture(ID3D11ShaderResourceView* mInputTexMapSRV, CXMMATRIX mInputTexTransform);
	void setBufferOffsetAndCount(UINT vOffset, UINT iOffset, UINT iCount);
	void setScaleAndOffset(CXMMATRIX mInputScale, float posxW, float posyW, float poszW);
	void setMovingState(float radius, float speed);
	void mOffsetUpdate(float dt);

	ID3D11ShaderResourceView* mTexMapSRV;
	XMMATRIX mTexTransform;
	Material mMat;

	float posxW;
	float posyW;
	float poszW;

	XMMATRIX mScale;
	XMMATRIX mOffset;

	UINT vertexOffset;
	UINT indexOffset;
	UINT indexCount;

	bool isMoving;
	float moveRadius;
	float moveSpeed;
};


#endif // !OBJECTS_H
