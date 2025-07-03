#include "OrderBook.h"

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

class MatchingEngine
{
public:
    MatchingEngine();
    ~MatchingEngine();

    void process_order(Order &order);

private:
    OrderQueue order_queue_;
    std::mutex order_queue_mutex_;
    std::condition_variable order_queue_cv_;
    bool stop_matching_engine_ = false;
    std::thread matching_engine_thread_;

    OrderBook order_book_;

    void match_loop();
};