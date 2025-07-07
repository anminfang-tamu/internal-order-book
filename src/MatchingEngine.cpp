#include "MatchingEngine.h"
#include <chrono>

MatchingEngine::MatchingEngine()
    : order_queue_(1024), // Initialize with capacity of 1024 (power of 2)
      stop_matching_engine_(false)
{
    matching_engine_thread_ = std::thread(&MatchingEngine::match_loop, this);
}

MatchingEngine::~MatchingEngine()
{
    stop_matching_engine_.store(true);
    if (matching_engine_thread_.joinable())
    {
        matching_engine_thread_.join();
    }

    // Clean up any remaining orders in the queue
    Order *remaining_order;
    while (order_queue_.pop(remaining_order))
    {
        delete remaining_order;
    }
}

void MatchingEngine::process_order(Order &order)
{
    // Create smart pointer for RAII and automatic cleanup
    std::unique_ptr<Order> order_ptr = std::make_unique<Order>(order);

    // Extract raw pointer for the lock-free queue
    Order *raw_ptr = order_ptr.get();

    while (!order_queue_.push(raw_ptr))
    {
        std::this_thread::yield();
    }

    order_ptr.release();
}

void MatchingEngine::match_loop()
{
    while (!stop_matching_engine_.load())
    {
        Order *current_order_ptr;

        // Lock-free pop - returns false if queue is empty
        if (order_queue_.pop(current_order_ptr))
        {
            order_book_.match_orders(*current_order_ptr);
            delete current_order_ptr; // Clean up after processing
        }
        else
        {
            // Queue is empty, small sleep to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}