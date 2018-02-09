#include "Models.h"


//SnowmanModel* Models::SnowmanModelData = nullptr;
//void Models::InitAll(ID3D11Device* device)
//{
//	//SnowmanModelData = new SnowmanModel(device);
//	//����SnowmanModel����ʹ����XMMATRIX,Ҫ���ַ16���룬
//	//ֱ��ʹ��new�Ļ�ֻ��8λ���룬�ᵼ�������������
//	//Ҳ�����������ع�operator new��operator delete.
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