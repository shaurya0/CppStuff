#include <cstring>
#include <type_traits>
#include <array>
#include <stdint.h>
#include <string>
namespace samples
{
    enum class FMT_TYPE
    {
        INTEGER = 1<<0,
        STRING = 1<<1,
        CHARACTER = 1<<2,
        FLOAT = 1<<3,
        MAX_FMT_TYPES = 5ULL
    };

    struct PrintfFMTMetadata
    {
        FMT_TYPE type;
        size_t position;
    };

    //template<enum FMT_TYPE> struct fmt_type_to_type{};
    //template<> struct fmt_type_to_type<samples::FMT_TYPE::INTEGER>{using type=int;};
    //template<> struct fmt_type_to_type<samples::FMT_TYPE::STRING>{using type=std::string;};
    //template<> struct fmt_type_to_type<samples::FMT_TYPE::CHARACTER>{using type=char;};
    //template<> struct fmt_type_to_type<samples::FMT_TYPE::FLOAT>{using type=float;};

    //template<typename TYPE> struct type_to_fmt_type{};
    //template<> struct type_to_fmt_type<int>{static constexpr auto value=samples::FMT_TYPE::INTEGER;};
    //template<> struct type_to_fmt_type<std::string>{static constexpr auto value=samples::FMT_TYPE::STRING;};
    //template<> struct type_to_fmt_type<char>{static constexpr auto value=samples::FMT_TYPE::CHARACTER;};
    //template<> struct type_to_fmt_type<float>{static constexpr auto value=samples::FMT_TYPE::FLOAT;};



    void parse_printf_format( const char *fmt, std::vector<PrintfFMTMetadata> &fmt_data )
    {
        auto previous_char = fmt[0];
        size_t i = 1;
        auto current_char = fmt[i];
        while( '\0' != current_char )
        {
            if (current_char != '%' && previous_char == '%')
            {
                switch( current_char )
                {
                    case 'd':
                    {
                        fmt_data.emplace_back(PrintfFMTMetadata{FMT_TYPE::INTEGER, i - 1});
                        break;
                    }
                    case 's':
                    {
                        fmt_data.emplace_back(PrintfFMTMetadata{FMT_TYPE::STRING, i - 1});
                        break;
                    }
                    case 'c':
                    {
                        fmt_data.emplace_back(PrintfFMTMetadata{FMT_TYPE::CHARACTER, i - 1});
                        break;
                    }
                    case 'f':
                    {
                        fmt_data.emplace_back(PrintfFMTMetadata{FMT_TYPE::FLOAT, i - 1});
                        break;
                    }
                    default:
                    {
                        assert( false );
                    }
                }

            }
            previous_char = current_char;
            current_char = fmt[++i];
        }
    }

    struct formatter_state
    {
        auto &get_buffer()
        {
            static bool reserved = false;
            static std::vector<PrintfFMTMetadata> fmt_metadata;
            if( !reserved )
            {
                fmt_metadata.reserve( 10 );
            }
            reserved = true;
            return fmt_metadata;
        }
        ~formatter_state()
        {
            get_buffer().clear();
        }
    };

	//template<typename ITEM>
	//std::string to_string(Item &&item)
	//{
	//	return std::to_string(item);
	//}

	//template<>
	//std::string to_string<char>(char &&item)
	//{
	//	return item;
	//}


    template<typename ITEM_TYPE>
    void snprintf_helper( char *buffer, const size_t MAX_BUFFER_LEN, size_t &buffer_offset, const char *fmt, size_t &fmt_offset, const std::vector<PrintfFMTMetadata> &fmt_metadata, size_t &idx, ITEM_TYPE &&item)
    {
        auto&& metadatum = fmt_metadata[idx];
        const size_t position = metadatum.position;
        assert( position > fmt_offset );

		const size_t num_chars = position - fmt_offset;
		size_t buffer_end = std::min(buffer_offset + num_chars, MAX_BUFFER_LEN);

        // copy raw text
        std::copy( fmt + fmt_offset, fmt + position, buffer + buffer_offset );
        buffer_offset += position - fmt_offset;
        fmt_offset = position + 2;
        const size_t remaining_buffer = MAX_BUFFER_LEN - buffer_offset;

        const std::string value_as_string = std::to_string( item );
        auto str_begin = value_as_string.cbegin();
        auto str_end = value_as_string.cend();
        const auto str_length = std::distance( str_begin, str_end );
        assert( str_length + position < remaining_buffer );
        std::copy( str_begin, str_end, buffer + buffer_offset);
        buffer_offset += str_length;
        ++idx;
    }

    template<typename ITEM_TYPE>
    void snprintf_recurse( char *buffer, const size_t MAX_BUFFER_LEN, size_t &buffer_offset, const char *fmt, size_t &fmt_offset, const std::vector<PrintfFMTMetadata> &fmt_metadata, size_t &idx, ITEM_TYPE &&item)
    {
        snprintf_helper( buffer, MAX_BUFFER_LEN, buffer_offset, fmt, fmt_offset, fmt_metadata, idx, item);
    }



    template<typename ITEM_TYPE, typename... Args>
    void snprintf_recurse( char *buffer, const size_t MAX_BUFFER_LEN, size_t &buffer_offset, const char *fmt, size_t &fmt_offset, const std::vector<PrintfFMTMetadata> &fmt_metadata, size_t &idx, ITEM_TYPE &&item, Args&&... args)
    {
        snprintf_helper( buffer, MAX_BUFFER_LEN, buffer_offset, fmt, fmt_offset, fmt_metadata, idx, item);
        snprintf_recurse( buffer, MAX_BUFFER_LEN, buffer_offset, fmt, fmt_offset, fmt_metadata, idx, args... );
    }


    template<typename... Args>
    int snprintf( char *buffer, size_t n, const char *format, Args&&... args )
    {
        if (nullptr == buffer || nullptr == format)
        {
            return -1;
        }

        formatter_state fmt_state;
        auto&& fmt_metadata = fmt_state.get_buffer();
        parse_printf_format( format, fmt_metadata );
        assert( fmt_metadata.size() == sizeof...( args ) );

        size_t fmt_offset = 0;
        size_t buffer_offset = 0;
        size_t metadata_idx = 0;
        const size_t MAX_BUFFER_LEN = n - 1; // reserve for null termination
        snprintf_recurse( buffer, n, buffer_offset, format, fmt_offset, fmt_metadata, metadata_idx, args... );

        const size_t N = strlen( format );
        const size_t M = std::min( N, n );

        std::copy( format + fmt_offset, format + M, buffer + buffer_offset );
        return 0;
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
        return static_cast<int>( M );
    }
} // samples
