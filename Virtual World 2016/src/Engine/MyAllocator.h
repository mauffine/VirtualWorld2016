#pragma once
#include <PxPhysicsAPI.h>

using namespace physx;
class MyAllocator : public PxAllocatorCallback
{
public:
	virtual ~MyAllocator() {}
	virtual void* Allocate(size_t a_size, const char* a_typeName)
	{
		void* pointer = _aligned_malloc(a_size, 16);
		return pointer;
	}
	virtual void Deallocate(void* a_ptr)
	{
		_aligned_free(a_ptr);
	}
private:

};