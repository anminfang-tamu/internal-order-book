#include "MatchingEngine.h"

MatchingEngine::MatchingEngine()
{
    matching_engine_thread_ = std::thread(&MatchingEngine::match_loop, this);
}

MatchingEngine::~MatchingEngine()
{
    {
        std::lock_guard<std::mutex> lock(order_queue_mutex_);
        stop_matching_engine_ = true;
    }
    order_queue_cv_.notify_one();
    if (matching_engine_thread_.joinable())
    {
        matching_engine_thread_.join();
    }
}

void MatchingEngine::process_order(Order &order)
{
    {
        std::lock_guard<std::mutex> lock(order_queue_mutex_);
        order_queue_.push_back(std::move(order));
    }
    order_queue_cv_.notify_one();
}

void MatchingEngine::match_loop()
{
    while (true)
    {
        Order current_order;

        {
            std::unique_lock<std::mutex> lock(order_queue_mutex_);
            order_queue_cv_.wait(lock, [this]
                                 { return !order_queue_.empty() || stop_matching_engine_; });

            if (stop_matching_engine_ || order_queue_.empty())
            {
                break;
            }

            current_order = order_queue_.front();
            order_queue_.pop_front();
        }

        order_book_.match_orders(current_order);
    }
}