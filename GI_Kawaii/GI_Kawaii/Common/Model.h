#pragma once

#include "Geometry.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

enum ModelType
{ 
	TEMPLATE_MODEL = 0, 
	ASSIMP_MODEL = 1, 
	QUAD_MODEL = 2, GRID_MODEL = 3, 
	CUBE_MODEL = 4, 
	SPHERE_MODEL = 5, 
	CYLINDER_MODEL = 6 
};

class Model
{
public:
	Model();
	Model(ModelType _type, std::string _matName = "");

	Model(const Model& rhs) = delete;
	Model& operator=(const Model& rhs) = delete;
};