#include "TerrainModel.h"

TerrainModel::TerrainModel(ID3D11Device* de)
	:Model(de) {}

TerrainModel::~TerrainModel() {}

void* TerrainModel::operator new(size_t size)
{
	void* p = _aligned_malloc(size, 16);
	if (p == 0)  throw std::bad_alloc();
	return p;
}

void TerrainModel::operator delete(void* p)
{
	TerrainModel* pc = static_cast<TerrainModel*>(p);
	_aligned_free(p);
}

void TerrainModel::Init()
{
	//terrainWidth = 512.0f;
	//terrainHeight = 512.0f;
	terrainWidth = 1024.0f;
	terrainHeight = 1024.0f;
	terrainRow = 512;
	terrainCol = 512;

	terrainHeightScale = 50.0f;

	TexMgr.Init(device);
	TexMgr.CreateTexture(L"Textures/snow.dds");
	XMMATRIX mTexTransform = XMMatrixScaling(1.0f, 1.0f, 0.0f);

	Material mMat;
	mMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 32.0f);
	mMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	//only a box
	std::vector<SubModel> temp(1);
	Subs = temp;

	Subs[0].SetTexResource(TexMgr[L"Textures/snow.dds"], mTexTransform, mMat);

	GenerateModel();
}

//判断该点是否在地形上空
bool TerrainModel::IsInModel(XMFLOAT3 pos)
{
	float delta = 0.05f;
	if ((abs(pos.x) < terrainWidth / 2 - delta) && (abs(pos.z) < terrainHeight / 2 - delta))
		return true;
	else
		return false;
}

//float TerrainModel::GetHeight(XMFLOAT3 pos)
//{
//	//计算该点在高度图里xz坐标的位置
//	float px = pos.x*static_cast<float>(terrainRow) / terrainWidth + static_cast<float>(terrainRow) / 2;
//	float pz = static_cast<float>(terrainCol) / 2 - pos.z*static_cast<float>(terrainCol) / terrainHeight;
//	//获得该点所在cell的4个顶点的index
//	//
//	//	11----------12---->x
//	//	|			|
//	//	|	px,pz	|
//	//	|			|
//	//	21----------22
//	//	|
//	//	|
//	//	z
//
//	UINT index[2][2][2];
//	index[0][0][0] = static_cast<UINT>(floor(px));
//	index[0][0][1] = static_cast<UINT>(floor(pz));
//
//	index[0][1][0] = static_cast<UINT>(ceil(px));
//	index[0][1][1] = static_cast<UINT>(floor(pz));
//
//	index[1][0][0] = static_cast<UINT>(floor(px));
//	index[1][0][1] = static_cast<UINT>(ceil(pz));
//
//	index[1][1][0] = static_cast<UINT>(ceil(px));
//	index[1][1][1] = static_cast<UINT>(ceil(pz));
//
//	float h = 0.0f;
//	for (int i = 0; i < 2; i++) {
//		for (int j = 0; j < 2; j++) {
//			h += mHeightmap[index[i][j][1]*(terrainRow+1)+index[i][j][0]];
//		}
//	}
//	h /= 4;
//
//	return h;
//}

float TerrainModel::GetHeight(XMFLOAT3 pos)
{
	if (!IsInModel(pos)) {
		return pos.y;
	}

	// Transform from terrain local space to "cell" space.
	float c = pos.x*static_cast<float>(terrainRow) / terrainWidth + static_cast<float>(terrainRow) / 2;
	float d = static_cast<float>(terrainCol) / 2 - pos.z*static_cast<float>(terrainCol) / terrainHeight;
	//float c = (pos.x + 0.5f*GetWidth()) / mInfo.CellSpacing;
	//float d = (pos.z - 0.5f*GetDepth()) / -mInfo.CellSpacing;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = mHeightmap[row*(terrainRow + 1) + col];
	float B = mHeightmap[row*(terrainRow + 1) + col + 1];
	float C = mHeightmap[(row + 1)*(terrainRow + 1) + col];
	float D = mHeightmap[(row + 1)*(terrainRow + 1) + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s * uy + t * vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}
}

void TerrainModel::GenerateModel()
{
	GeometryGenerator::MeshData terr;
	LoadHeightMap();
	ComputeTerrainData(terr);

	std::vector<Vertex::Basic32> vertices;
	std::vector<UINT> indices;

	XMMATRIX mScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX mOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMMATRIX mRotation = XMMatrixIdentity();
	XMMATRIX mTrans = mScale * mOffset * mRotation;
	AssembleSubModel(vertices, indices, terr, mTrans, Subs[0]);

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * terr.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(device->CreateBuffer(&vbd, &vinitData, &mVB));

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * terr.Indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(device->CreateBuffer(&ibd, &iinitData, &mIB));
}



void TerrainModel::LoadHeightMap()
{
	std::vector<unsigned char> in;

	std::ifstream inFile;
	inFile.open(terrainHeightMapFilename.c_str(), std::ios_base::binary);

	if (inFile)
	{
		inFile.seekg(0, std::ios::end);
		//get the size of file
		in.resize(static_cast<UINT>(inFile.tellg()), 0);
		inFile.seekg(std::ios::beg);

		inFile.read((char*)&in[0], (std::streamsize)in.size());
		inFile.close();

		// Copy the array data into a float array and scale it.
		mHeightmap.resize(in.size(), 0);
		for (UINT i = 0; i < in.size(); ++i)
		{
			mHeightmap[i] = (in[i] / 255.0f)*terrainHeightScale;
		}
	}
}

void TerrainModel::ComputeTerrainData(GeometryGenerator::MeshData& geo)
{
	UINT m_cellsPerRow = terrainRow;
	UINT m_cellsPerCol = terrainCol;
	UINT m_verticesPerRow = m_cellsPerRow + 1;
	UINT m_verticesPerCol = m_cellsPerCol + 1;
	UINT m_numsVertices = m_verticesPerRow * m_verticesPerCol;
	float m_width = terrainWidth;
	float m_height = terrainHeight;

	//起始x z坐标
	float oX = -terrainWidth * 0.5f;
	float oZ = terrainHeight * 0.5f;
	//每一格坐标变化
	float dx = terrainWidth / terrainRow;
	float dz = terrainHeight / terrainCol;

	geo.Vertices.resize(m_numsVertices);
	//计算顶点
	for (UINT i = 0; i < m_verticesPerCol; ++i)
	{
		float tempZ = oZ - dz * i;
		for (UINT j = 0; j < m_verticesPerRow; ++j)
		{
			UINT index = m_verticesPerRow * i + j;
			geo.Vertices[index].Position.x = oX + dx * j;
			geo.Vertices[index].Position.y = mHeightmap[index];
			geo.Vertices[index].Position.z = tempZ;

			geo.Vertices[index].TexC = XMFLOAT2(dx*i, dx*j);
		}
	}

	//计算索引和法线
	//总格子数量:m * n
	//因此总索引数量: 6 * m * n
	UINT nIndices = terrainRow * terrainCol * 6;
	geo.Indices.resize(nIndices);
	UINT tmp = 0;
	for (UINT i = 0; i < terrainCol; ++i)
	{
		for (UINT j = 0; j < terrainRow; ++j)
		{
			geo.Indices[tmp] = i * m_verticesPerRow + j;
			geo.Indices[tmp + 1] = i * m_verticesPerRow + j + 1;
			geo.Indices[tmp + 2] = (i + 1) * m_verticesPerRow + j;

			//计算法线
			ComputeNomal(geo.Vertices[geo.Indices[tmp]],
				geo.Vertices[geo.Indices[tmp + 1]],
				geo.Vertices[geo.Indices[tmp + 2]]);


			geo.Indices[tmp + 3] = i * m_verticesPerRow + j + 1;
			geo.Indices[tmp + 4] = (i + 1) * m_verticesPerRow + j + 1;
			geo.Indices[tmp + 5] = (i + 1) * m_verticesPerRow + j;

			ComputeNomal(geo.Vertices[geo.Indices[tmp + 3]],
				geo.Vertices[geo.Indices[tmp + 4]],
				geo.Vertices[geo.Indices[tmp + 5]]);

			tmp += 6;
		}
	}
}

void TerrainModel::ComputeNomal(GeometryGenerator::Vertex& v1, GeometryGenerator::Vertex& v2, GeometryGenerator::Vertex& v3)
{
	XMFLOAT3 f1(v2.Position.x - v1.Position.x, v2.Position.y - v1.Position.y, v2.Position.z - v1.Position.z);
	XMFLOAT3 f2(v3.Position.x - v1.Position.x, v3.Position.y - v1.Position.y, v3.Position.z - v1.Position.z);
	XMVECTOR vec1 = XMLoadFloat3(&f1);
	XMVECTOR vec2 = XMLoadFloat3(&f2);
	XMVECTOR temp = XMVector3Normalize(XMVector3Cross(vec1, vec2));

	XMStoreFloat3(&v1.Normal, temp);
	XMStoreFloat3(&v2.Normal, temp);
	XMStoreFloat3(&v3.Normal, temp);
}