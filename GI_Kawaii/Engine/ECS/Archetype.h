#pragma once

#include <vector>

#include "TypeIdGenerator.h"

typedef unsigned char* ComponentData;

struct Archetype
{
	ArchetypeID type;
	std::vector<ComponentData> componentData;
	std::vector<EntityID> entityIds;
	std::vector<std::size_t> componentDataSize;
};