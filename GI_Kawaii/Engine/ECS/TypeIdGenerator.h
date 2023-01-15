#pragma once

#include <cstdint>
#include <vector>

typedef std::uint32_t IDType;
typedef IDType EntityID;
typedef IDType ComponentTypeID;
const IDType NULL_ENTITY = 0;

typedef std::vector<ComponentTypeID> ArchetypeID;

// We can use the TypeIDGenerator  to generate different IDs for different types.

template <class T>
class TypeIdGenerator
{
public:

	template<class U>
	static const IDType GetNewID()
	{
		static const IDType idCounter = m_count++;
		return idCounter;
	}
private:
	static IDType m_count;
};

template <class T> 
IDType TypeIdGenerator<T>::m_count = 0;

// Example 
// TypeIdGenerator<Component>::m_count
// TypeIdGenerator<Entity>::m_count