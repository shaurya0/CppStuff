#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <iostream>

namespace Concurrency
{
    struct CancellationSource
    {
    private:
        std::atomic<bool> _cancelled;
    public:
        CancellationSource(  ) : _cancelled( false ){}
        void cancel()
        {
            _cancelled = true;
        }

        bool cancelled()
        {
            return _cancelled;
        }
    };

} // Concurrency
