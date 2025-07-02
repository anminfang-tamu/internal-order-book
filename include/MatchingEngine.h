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

    void process_order(Order &order)
    {
        match_orders(order);
    }

private:
    OrderQueue order_queue_;
    std::mutex order_queue_mutex_;
    std::condition_variable order_queue_cv_;
    bool stop_matching_engine_ = false;

    OrderBook order_book_;

    void order_producer_thread(Order &order);

    void match_orders(Order &order);
};