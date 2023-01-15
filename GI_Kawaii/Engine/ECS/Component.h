#pragma once

#include <cstdio>
#include "TypeIdGenerator.h"

class ComponentBase
{
public:
	virtual ~ComponentBase() {}

    virtual void DestroyData(unsigned char* data) const = 0;
    virtual void MoveData(unsigned char* source, unsigned char* destination) const = 0;
    virtual void ConstructData(unsigned char* data) const = 0;

    virtual std::size_t GetSize() const = 0;
};

template <class C>
class Component : public ComponentBase
{
public:
    virtual void DestroyData(unsigned char* data) const override;

    virtual void MoveData(unsigned char* source, unsigned char* destination) const override;

    virtual void ConstructData(unsigned char* data) const override;

    virtual std::size_t GetSize() const override;

    static ComponentTypeID GetTypeID();
};