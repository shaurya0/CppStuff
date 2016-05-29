#include <cstring>
#include <type_traits>
#include <array>
#include <stdint.h>
namespace samples
{


    template<typename T, size_t MAX_FORMAT_OUTPUT = 10>
    struct PrintFormat
    {
        enum class FormatFlags {FMT_INT, FMT_STR};
        std::array<uint16_t, MAX_FORMAT_OUTPUT> format_args;

    };

    template<typename T, typename... Args>
    int snprintf( char *str, size_t n, const char *format, T first, Args&&... args )
    {
        if (nullptr == str || nullptr == format)
        {
            return -1;
        }


        const size_t N = strlen( format );
        const size_t M = std::min( N, n );

        std::copy( format, format + M, str );
    }

    int snprintf( char *str, size_t n, const char *format )
    {
        if (nullptr == str || nullptr == format)
        {
            return -1;
        }


        const size_t N = strlen( format );
        const size_t M = std::min( N, n );

        std::copy( format, format + M, str );
    }
} // samples