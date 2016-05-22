#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <condition_variable>
#include <queue>
#include <atomic>

namespace Concurrency
{
    template<typename T>
    class ConcurrentQueue
    {
    public:
        using value_type = T;
    private:
        std::mutex _mutex;
        CancellationSource &_cs;
        std::condition_variable _not_empty;
        std::queue<T> _queue;
        bool _discarded;
    public:
        ConcurrentQueue(CancellationSource &cs)
        : _cs( cs )
        , _discarded( false ) {}


        void pop(T& item)
        {
            std::unique_lock<std::mutex> lock( _mutex );
            if (false == _cs.cancelled())
            {
                while(_queue.empty() && (false == _cs.cancelled()))
                {
                    // std::cout << "going into wait" << std::endl;
                    _not_empty.wait( lock );
                }
            }

            if ( false == _cs.cancelled() )
            {
                item = _queue.front();
                _queue.pop();
            }
        }

        template<typename U>
        void enqueue( U&& item )
        {
            std::lock_guard<std::mutex> lock( _mutex );
            _queue.push( std::forward<U>( item ) );
            _not_empty.notify_one();
        }

        size_t count()
        {
            std::lock_guard<std::mutex> lock( _mutex );
            return _queue.size();
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lock( _mutex );
            return (0 == _queue.count());
        }

        void cancel()
        {
            std::lock_guard<std::mutex> lock( _mutex );
            if( false == _discarded )
            {
                assert( true == _cs.cancelled() );
                std::queue<T> empty_queue;
                std::swap( _queue, empty_queue );
                _not_empty.notify_all();
                _discarded = true;
            }
        }

        ~ConcurrentQueue()
        {
            cancel();
        }
    };
} // Concurrency
