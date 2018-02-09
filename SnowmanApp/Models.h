#ifndef MODELS_H
#define MODELS_H


#include "ClassModel.h"
#include "TerrainModel.h"
//class to save each unique model data
class Models
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static SnowmanModel* SnowmanModelData;
	static BoxModel* BoxModelData;
	static TerrainModel* TerrainModelData;
	//static BoxModel* BoxModelData;
};


#endif //MODELS_H