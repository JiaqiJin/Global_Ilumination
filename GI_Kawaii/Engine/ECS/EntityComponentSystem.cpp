#include "EntityComponentSystem.h"

ECS::ECS()
{

}

ECS::~ECS()
{

}

EntityID ECS::GetNewID()
{
	return m_entityIdCounter++;
}

template<class C>
void ECS::RegisterComponent()
{

}

template<class C>
bool ECS::IsComponentRegistered()
{
	return false;
}

void ECS::RegisterEntity(const EntityID entityId)
{

}

template<class C, typename... Args>
C* ECS::AddComponent(const EntityID& entityId, Args&&... args)
{
	
}

template<class C>
void ECS::RemoveComponent(const EntityID& entityId)
{

}

template<class C>
C* ECS::GetComponent(const EntityID& entityId)
{

}

template<class C>
bool ECS::HasComponent(const EntityID& entityId)
{

}

void ECS::RemoveEntity(const EntityID& entityId)
{

}
