#ifndef TERRAINMODEL_H
#define TERRAINMODEL_H


#include "ClassModel.h"

__declspec(align(16)) class TerrainModel :public Model
{
public:
	TerrainModel(ID3D11Device* device);
	~TerrainModel();

	void* operator new(size_t size);
	void operator delete(void* p);

	void Init();
	bool IsInModel(XMFLOAT3 pos);
	float GetHeight(XMFLOAT3 pos);

private:
	TerrainModel(const BoxModel& rhs);
	TerrainModel& operator=(const TerrainModel& rhs);
	void GenerateModel();

private:
	void LoadHeightMap();
	void ComputeTerrainData(GeometryGenerator::MeshData& geo);
	void ComputeNomal(GeometryGenerator::Vertex& v1, GeometryGenerator::Vertex& v2, GeometryGenerator::Vertex& v3);

	std::wstring terrainHeightMapFilename = L"Textures/terrain_512.raw";

	float terrainWidth;
	float terrainHeight;
	UINT terrainRow;	//每行单元数量，即分割成多少列
	UINT terrainCol;	//每列单元数量，即分割成多少行

	float terrainHeightScale;

	std::vector<float> mHeightmap;
};


#endif // !TERRAINMODEL_H
