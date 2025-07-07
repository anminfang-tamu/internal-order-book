#include "OrderBook.h"

#include <boost/lockfree/queue.hpp>
#include <thread>
#include <atomic>
#include <memory>

class MatchingEngine
{
public:
    MatchingEngine();
    ~MatchingEngine();

    void process_order(Order &order);

private:
    // Lock-free queue for order pointers (capacity must be power of 2)
    boost::lockfree::queue<Order *> order_queue_;
    std::atomic<bool> stop_matching_engine_;
    std::thread matching_engine_thread_;

    OrderBook order_book_;

    void match_loop();
};