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
    // Create a copy of the order on the heap
    Order *order_ptr = new Order(order);

    // Lock-free push - if queue is full, this will return false
    while (!order_queue_.push(order_ptr))
    {
        // Queue is full, could either:
        // 1. Busy wait (current approach)
        // 2. Drop the order
        // 3. Expand queue capacity
        std::this_thread::yield(); // Give other threads a chance
    }
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