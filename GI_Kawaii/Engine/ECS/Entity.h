#pragma once

#include "TypeIdGenerator.h"

class ECS;

class Entity
{
public:
	explicit Entity(ECS& ecs) : m_id(ecs.GetNewID()), m_ecs(ecs)
	{
		m_ecs.RegisterEntity(m_id);
	}

	template <class C, typename... Args>
	C* Add(Args&&... args)
	{
		return m_ecs.AddComponent<C>(m_id, std::forward<Args>(args)...);
	}

	template<class C>
	C* Add(C&& c)
	{
		return m_ecs.AddComponent<C>(m_id, std::forward<C>(c));
	}

	EntityID GetID() const
	{
		return m_id;
	}
private:
	EntityID m_id;
	ECS& m_ecs;
};