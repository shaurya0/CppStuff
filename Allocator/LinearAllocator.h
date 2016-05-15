#include <iostream>
#include <iomanip>
#include <array>
#include <memory>

namespace allocator
{
    template<size_t BUFFER_SIZE>
    class LinearAllocator
    {
    private:
        std::array<uint8_t, BUFFER_SIZE> _buffer;
        size_t _offset;
    public:
        LinearAllocator() : _offset( 0 ){};
        void* allocate( size_t size, size_t alignment )
        {
            size_t remaining_space = BUFFER_SIZE - alignment;
            void *buf_ptr = reinterpret_cast<void*>( &_buffer[_offset] );
            void *ptr = std::align( alignment, size, buf_ptr, remaining_space );
            if ( nullptr == ptr )
            {
                std::cout << "failed to allocate" << std::endl;
                return nullptr;
            }
            _offset += size;

            return ptr;
        }

        void deallocate(void *p, size_t size)
        {
            _offset = 0;
        }

    };
} // allocator

