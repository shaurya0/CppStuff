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
            const size_t remaining_space_before = BUFFER_SIZE - _offset;
            size_t remaining_space = BUFFER_SIZE - _offset;

            void *buf_ptr = reinterpret_cast<void*>( &_buffer[_offset] );
            void *ptr = std::align( alignment, size, buf_ptr, remaining_space );
            if ( remaining_space_before != remaining_space)
            {
                const int32_t delta = remaining_space_before - remaining_space;
                _offset += delta;
            }

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

