#include <thread>
#include <iostream>
#include <mutex>
#include <vector>
#include <functional>
#include <condition_variable>
#include <algorithm>
#include <queue>
#include <assert.h>
#include "CancellationSource.h"
#include "ConcurrentQueue.h"

namespace Concurrency
{
    class ThreadPool
    {
    public:
        using task_t = std::function<void()>;
    private:
        CancellationSource &_cs;
        std::vector<std::thread> _thread_pool;
        ConcurrentQueue<task_t> _task_queue;
        bool _joined;

        void worker_loop()
        {
            task_t task;
            while( false == _cs.cancelled() )
            {
                _task_queue.pop( task );
                if (false == _cs.cancelled())
                {
                    task();
                }
            }
        }


    public:
        ThreadPool( CancellationSource &cs, size_t pool_size )
         : _cs( cs )
         , _joined( false )
         , _task_queue( cs )

        {
            _thread_pool.reserve( pool_size );
            for (size_t i = 0; i < pool_size; ++i)
            {
                _thread_pool.emplace_back( std::move( std::thread( &ThreadPool::worker_loop, this ) ) );
            }
        }

        ~ThreadPool(  )
        {
            join();
        }

        void add_task( task_t && task )
        {
            _task_queue.enqueue( task );
        }

        void join()
        {
            if (false == _joined)
            {
                assert( true == _cs.cancelled() );
                _task_queue.cancel();
                for(auto&& thread : _thread_pool)
                {
                    thread.join();
                }
                _joined = true;
            }
        }
    };

} // Concurrency
