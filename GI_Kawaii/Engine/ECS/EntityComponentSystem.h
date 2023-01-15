#pragma once

#include <unordered_map>

#include "Component.h"
#include "TypeIdGenerator.h"

class ECS
{
public:
	ECS();
	~ECS();

	EntityID GetNewID();

	template<class C>
	void RegisterComponent();

	template<class C>
	bool IsComponentRegistered();

	void RegisterEntity(const EntityID entityId);

	template<class C, typename... Args>
	C* AddComponent(const EntityID& entityId, Args&&... args);

	template<class C>
	void RemoveComponent(const EntityID& entityId);

	template<class C>
	C* GetComponent(const EntityID& entityId);

	template<class C>
	bool HasComponent(const EntityID& entityId);

	void RemoveEntity(const EntityID& entityId);
private:
	typedef std::unordered_map<ComponentTypeID, ComponentBase*> ComponentTypeIDBaseMap;

	struct Record
	{

	};

	// Variables
	EntityID m_entityIdCounter;
};

