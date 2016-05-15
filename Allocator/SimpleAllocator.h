#include <iostream>
#include <iomanip>

namespace allocator
{
    template <class T>
    class SimpleAllocator
    {
    public:
      using size_type=size_t;
      using difference_type=ptrdiff_t;
      using pointer=T*;
      using const_pointer=const T*;
      using reference=T&;
      using const_reference=const T&;
      using value_type=T;

      SimpleAllocator() {}
      SimpleAllocator(const SimpleAllocator&) {}



    pointer allocate(size_type n, const void * = 0)
    {
        const size_type size = n * sizeof(T);
        T* t = (T*) malloc(size);
        std::cout << "used SimpleAllocator to allocate at address " <<
        std::hex << (size_type)t << " : " << std::dec << size << std::endl;
        return t;
    }

    void deallocate(void* p, size_type size)
    {
        if (p)
        {
            free(p);
            std::cout << "used SimpleAllocator to deallocate at address " <<
            std::hex << (size_type)p << ": " << std::dec << size << std::endl;
        }
    }

      pointer           address(reference x) const { return &x; }
      const_pointer     address(const_reference x) const { return &x; }
      SimpleAllocator<T>&  operator=(const SimpleAllocator&) { return *this; }
      void              construct(pointer p, const T& val)
                        { new ((T*) p) T(val); }
      void              destroy(pointer p) { p->~T(); }

      size_type         max_size() const { return size_t(-1); }

      template <class U>
      struct rebind { typedef SimpleAllocator<U> other; };

      template <class U>
      SimpleAllocator(const SimpleAllocator<U>&) {}

      template <class U>
      SimpleAllocator& operator=(const SimpleAllocator<U>&) { return *this; }
    };
} // allocator

