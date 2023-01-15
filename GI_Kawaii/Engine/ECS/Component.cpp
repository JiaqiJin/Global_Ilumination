#include "Component.h"
#include <new>

template <class C>
void Component<C>::DestroyData(unsigned char* data) const
{
	C* dataLocation = std::launder(reinterpret_cast<C*>(data));

	dataLocation->~C();
}

template <class C>
void Component<C>::MoveData(unsigned char* source, unsigned char* destination) const
{
	new (&destination[0]) C(std::move(*reinterpret_cast<C*>(source)));
}

template <class C>
void Component<C>::ConstructData(unsigned char* data) const
{
	new (&data[0]) C();
}

template <class C>
std::size_t Component<C>::GetSize() const
{
	return sizeof(C);
}

template <class C>
static ComponentTypeID Component<C>::GetTypeID()
{
	return TypeIdGenerator<ComponentBase>::GetNewID<C>();
}