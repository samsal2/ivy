#include "IvyMemoryAllocator.h"

#include "IvyDeclarations.h"
#include "IvyDummyMemoryAllocator.h"

#define IVY_MEMORY_ALLOCATOR_MAGIC 0xA50A6AAA

void ivySetupMemoryAllocatorBase(IvyMemoryAllocatorDispatch const *dispatch,
    IvyMemoryAllocatorBase *base) {
  base->magic = IVY_MEMORY_ALLOCATOR_MAGIC;
  base->dispatch = dispatch;
}

void *ivyAllocateMemory(IvyAnyMemoryAllocator allocator, uint64_t size) {
  IvyMemoryAllocatorBase *base = allocator;
  IVY_ASSERT(base);
  IVY_ASSERT(IVY_MEMORY_ALLOCATOR_MAGIC == base->magic);
  IVY_ASSERT(base->dispatch);
  IVY_ASSERT(base->dispatch->allocate);
  return base->dispatch->allocate(allocator, size);
}

void *ivyAllocateAndZeroMemory(IvyAnyMemoryAllocator allocator, uint64_t count,
    uint64_t elementSize) {
  IvyMemoryAllocatorBase *base = allocator;
  IVY_ASSERT(base);
  IVY_ASSERT(IVY_MEMORY_ALLOCATOR_MAGIC == base->magic);
  IVY_ASSERT(base->dispatch);
  IVY_ASSERT(base->dispatch->allocateAndZero);
  return base->dispatch->allocateAndZero(allocator, count, elementSize);
}

void *ivyReallocateMemory(IvyAnyMemoryAllocator allocator, void *data,
    uint64_t newSize) {
  IvyMemoryAllocatorBase *base = allocator;
  IVY_ASSERT(base);
  IVY_ASSERT(IVY_MEMORY_ALLOCATOR_MAGIC == base->magic);
  IVY_ASSERT(base->dispatch);
  IVY_ASSERT(base->dispatch->allocateAndZero);
  return base->dispatch->reallocate(allocator, data, newSize);
}

void ivyFreeMemory(IvyAnyMemoryAllocator allocator, void *data) {
  IvyMemoryAllocatorBase *base = allocator;
  IVY_ASSERT(base);
  IVY_ASSERT(IVY_MEMORY_ALLOCATOR_MAGIC == base->magic);
  IVY_ASSERT(base->dispatch);
  IVY_ASSERT(base->dispatch->free);
  base->dispatch->free(allocator, data);
}

void ivyDestroyMemoryAllocator(IvyAnyMemoryAllocator allocator) {
  IvyMemoryAllocatorBase *base = allocator;
  IVY_ASSERT(base);
  IVY_ASSERT(IVY_MEMORY_ALLOCATOR_MAGIC == base->magic);
  IVY_ASSERT(base->dispatch);
  IVY_ASSERT(base->dispatch->destroy);
  base->dispatch->destroy(allocator);
}

static IvyBool createdDefaultAllocator = 0;
static IvyAnyMemoryAllocator globalMemoryAllocator = NULL;

IvyCode ivySetGlobalMemoryAllocator(IvyAnyMemoryAllocator allocator) {
  IvyMemoryAllocatorBase *base = allocator;
  if (IVY_MEMORY_ALLOCATOR_MAGIC != base->magic) {
    return IVY_INVALID_VALUE;
  }

  globalMemoryAllocator = allocator;
  return IVY_OK;
}

IvyAnyMemoryAllocator ivyGetGlobalMemoryAllocator(void) {
  static IvyDummyMemoryAllocator defaultMemoryAllocator;

  if (!globalMemoryAllocator && !createdDefaultAllocator) {
    ivyCreateDummyMemoryAllocator(&defaultMemoryAllocator);
    globalMemoryAllocator = &defaultMemoryAllocator;
    createdDefaultAllocator = 1;
  }

  return globalMemoryAllocator;
}

void ivyDestroyGlobalMemoryAllocator(void) {
  ivyDestroyMemoryAllocator(ivyGetGlobalMemoryAllocator());
}
