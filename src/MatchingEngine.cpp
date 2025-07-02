#include "MatchingEngine.h"

MatchingEngine::MatchingEngine()
{
}

MatchingEngine::~MatchingEngine()
{
    stop_matching_engine_ = true;
    order_queue_cv_.notify_all();
}

void MatchingEngine::order_producer_thread(Order &order)
{
    {
        std::lock_guard<std::mutex> lock(order_queue_mutex_);
        order_queue_.push_back(order);
    }
    order_queue_cv_.notify_one();
}

void MatchingEngine::match_orders(Order &order)
{
    while (!stop_matching_engine_)
    {
        std::unique_lock<std::mutex> lock(order_queue_mutex_);
        order_queue_cv_.wait(lock, [this]
                             { return !order_queue_.empty() || stop_matching_engine_; });
        Order order = order_queue_.front();
        order_queue_.pop_front();
        lock.unlock();

        order_book_.add_order(order);

        if (order.get_side() == OrderSide::BUY)
        {
        }
        else
        {
        }
    }
}