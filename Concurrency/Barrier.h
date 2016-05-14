#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <cstdint>
#include <chrono>
#include <type_traits>
#include <iostream>

namespace Concurrency
{
    class Barrier
    {
    private:
        std::mutex _mutex;
        std::condition_variable _all_threads_synced;

        uint32_t _signal_count;
        const uint32_t _num_threads;
        uint32_t _ping;

        template<typename FUNC, typename... Args>
        auto signal_and_wait(FUNC&& func, Args&&... args)
        -> typename std::result_of<decltype(func)( Args... )>::type
        {
            _signal_count++;

            if( _signal_count == _num_threads )
            {
                _signal_count = 0;
                ++_ping;
                _all_threads_synced.notify_all();
            }
            else
            {
                return func(args...);
            }
        }

    public:
        explicit Barrier(uint32_t num_threads) :
        _num_threads( num_threads ),
        _signal_count( 0 ),
        _ping( 0 )
        {};


        void signal_and_wait(  )
        {
            std::unique_lock<std::mutex> lock( _mutex );
            const uint32_t ping = _ping;

            signal_and_wait( [&, ping]() -> void
            {
                _all_threads_synced.wait( lock, [&](){ return ping != _ping;} );
            } );
        }

        template<typename TIME_TYPE>
        std::cv_status signal_and_wait( const TIME_TYPE &timeout )
        {
            std::unique_lock<std::mutex> lock( _mutex );
            const uint32_t ping = _ping;

            auto result = signal_and_wait( [&, ping]()
            {
                if (_all_threads_synced.wait_for( lock, timeout, [&](){ return ping != _ping;} ))
                {
                    return std::cv_status::no_timeout;
                }
                std::cout << "timeout" <<std::endl;
                return std::cv_status::timeout;
            } );
            return result;
        }

        ~Barrier() {}
    };
}

