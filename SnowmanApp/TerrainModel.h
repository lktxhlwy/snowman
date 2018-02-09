#ifndef TERRAINMODEL_H
#define TERRAINMODEL_H


#include "ClassModel.h"
#if 0
#__declspec(align(16)) class TerrainModel :public Model
{
public:
	TerrainModel(ID3D11Device* de);
	~TerrainModel();

	void* operator new(size_t size);
	void operator delete(void *p);

	void Init();

private:
	TerrainModel(const TerrainModel& rhs);
	TerrainModel& operator=(const TerrainModel& rhs);

	//void AssembleSubModel(std::vector<Vertex::Basic32> &vertices, std::vector<UINT> &indices, GeometryGenerator::MeshData &geo, CXMMATRIX mTrans, SubModel& sub);
	void GenerateModel();

private:
	void LoadHeightMap();
	void ComputeNomal(GeometryGenerator::Vertex& v1, GeometryGenerator::Vertex& v2, GeometryGenerator::Vertex& v3);
	void ComputeTerrainData(GeometryGenerator::MeshData& geo);


	std::wstring terrainHeightMapFilename;
	float terrainWidth = 512.0f;
	float terrainHeight = 512.0f;
	UINT terrainRow = 512;
	UINT terrainCol = 512;
	float terrainHeightScale = 5.0f;

	std::vector<float> mHeightmap;

};
#endif


__declspec(align(16)) class TerrainModel :public Model
{
public:
	TerrainModel(ID3D11Device* device);
	~TerrainModel();

	void* operator new(size_t size);
	void operator delete(void* p);

	void Init();
	//void Draw(ID3D11DeviceContext* dc, ID3DX11EffectTechnique* activeTech, CXMMATRIX mWorld, const Camera& camera);

private:
	TerrainModel(const BoxModel& rhs);
	TerrainModel& operator=(const TerrainModel& rhs);
	void GenerateModel();

private:
	void LoadHeightMap();
	void ComputeTerrainData(GeometryGenerator::MeshData& geo);
	void ComputeNomal(GeometryGenerator::Vertex& v1, GeometryGenerator::Vertex& v2, GeometryGenerator::Vertex& v3);

	float boxLength = 8.0f;

	std::wstring terrainHeightMapFilename = L"Textures/terrain_512.raw";
	float terrainWidth = 512.0f;
	float terrainHeight = 512.0f;
	UINT terrainRow = 512;
	UINT terrainCol = 512;
	float terrainHeightScale = 40.0f;

	std::vector<float> mHeightmap;
};


#endif // !TERRAINMODEL_H
