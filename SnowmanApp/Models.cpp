#include "Models.h"


//SnowmanModel* Models::SnowmanModelData = nullptr;
//void Models::InitAll(ID3D11Device* device)
//{
//	//SnowmanModelData = new SnowmanModel(device);
//	//由于SnowmanModel类里使用了XMMATRIX,要求地址16对齐，
//	//直接使用new的话只有8位对齐，会导致向量运算出错。
//	//也可以在类里重构operator new和operator delete.
//	SnowmanModelData = (SnowmanModel*)_aligned_malloc(sizeof(SnowmanModel),16);
//	SnowmanModelData->SnowmanModel::SnowmanModel(device);
//
//	//BoxModelData = new BoxModel(device);
//}
//
//void Models::DestroyAll()
//{
//	//SafeDelete(SnowmanModelData);
//	SnowmanModelData->~SnowmanModel();
//	_aligned_free(SnowmanModelData);
//	//SafeDelete(BoxModelData);
//}

SnowmanModel* Models::SnowmanModelData = nullptr;
BoxModel* Models::BoxModelData = nullptr;
TerrainModel* Models::TerrainModelData = nullptr;

void Models::InitAll(ID3D11Device* device)
{
	SnowmanModelData = new SnowmanModel(device);
	SnowmanModelData->Init();

	BoxModelData = new BoxModel(device);
	BoxModelData->Init();

	TerrainModelData = new TerrainModel(device);
	TerrainModelData->Init();
}

void Models::DestroyAll()
{
	SafeDelete(SnowmanModelData);
	SafeDelete(BoxModelData);
	SafeDelete(TerrainModelData);

}