#include "OrderBook.h"
#include <stdexcept>

OrderBook::OrderBook()
{
    // Default constructor - empty order book
}

OrderBook::~OrderBook()
{
    // Destructor - clean up if needed
}

void OrderBook::add_order(Order &order)
{
    if (order.get_side() == OrderSide::BUY)
    {
        bids[order.get_price()].push_back(order);
    }
    else
    {
        asks[order.get_price()].push_back(order);
    }
}

void OrderBook::remove_order(Order &order)
{
    if (order.get_side() == OrderSide::BUY)
    {
        remove_order_from_book(order);
    }
    else
    {
        auto it = asks.find(order.get_price());
        if (it != asks.end())
        {
            auto &queue = it->second;
            for (auto queue_it = queue.begin(); queue_it != queue.end(); ++queue_it)
            {
                if (queue_it->get_id() == order.get_id())
                {
                    queue.erase(queue_it);
                    break;
                }
            }
            if (queue.empty())
            {
                asks.erase(it);
            }
        }
    }
}

void OrderBook::update_order(Order &order)
{
    remove_order(order); // Remove the old version
    add_order(order);    // Add the updated version
}

void OrderBook::cancel_order(Order &order)
{
    remove_order(order); // Reuse the remove_order logic
}

double OrderBook::get_best_bid() const
{
    if (bids.empty())
    {
        throw std::runtime_error("No bids available");
    }
    return bids.begin()->first;
}

double OrderBook::get_best_ask() const
{
    if (asks.empty())
    {
        throw std::runtime_error("No asks available");
    }
    return asks.begin()->first;
}

OrderQueue OrderBook::get_bids(double price) const
{
    auto it = bids.find(price);
    if (it != bids.end())
    {
        return it->second;
    }
    return OrderQueue();
}

OrderQueue OrderBook::get_asks(double price) const
{
    auto it = asks.find(price);
    if (it != asks.end())
    {
        return it->second;
    }
    return OrderQueue();
}

void OrderBook::add_order_to_book(Order &order)
{
    if (order.get_side() == OrderSide::BUY)
    {
        bids[order.get_price()].push_back(order);
    }
    else
    {
        asks[order.get_price()].push_back(order);
    }
}

void OrderBook::remove_order_from_book(Order &order)
{
    if (order.get_side() == OrderSide::BUY)
    {
        auto it = bids.find(order.get_price());
        if (it != bids.end())
        {
            auto &queue = it->second;
            for (auto it = queue.begin(); it != queue.end(); ++it)
            {
                if (it->get_id() == order.get_id())
                {
                    queue.erase(it);
                    break;
                }
            }

            if (queue.empty())
            {
                bids.erase(it);
            }
        }
        else
        {
            auto it = asks.find(order.get_price());
            if (it != asks.end())
            {
                auto &queue = it->second;
                for (auto it = queue.begin(); it != queue.end(); ++it)
                {
                    if (it->get_id() == order.get_id())
                    {
                        queue.erase(it);
                        break;
                    }
                }
                if (queue.empty())
                {
                    asks.erase(it);
                }
            }
        }
    }
}

void OrderBook::update_order_in_book(Order &order)
{
    if (order.get_side() == OrderSide::BUY)
    {
        remove_order_from_book(order);
        add_order_to_book(order);
    }
    else
    {
        remove_order_from_book(order);
        add_order_to_book(order);
    }
}

void OrderBook::match_orders(Order &incoming_order)
{
    if (incoming_order.get_side() == OrderSide::BUY)
    {
        for (auto it = asks.begin(); it != asks.end() && incoming_order.get_quantity() > 0;)
        {
            double ask_price = it->first;

            // limit order
            if (incoming_order.get_type() == OrderType::LIMIT && incoming_order.get_price() < ask_price)
            {
                break;
            }

            // market order
            OrderQueue &ask_queue = it->second;
            while (!ask_queue.empty() && incoming_order.get_quantity() > 0)
            {
                Order &resting_order = ask_queue.front();

                int traded_quantity = std::min(incoming_order.get_quantity(), resting_order.get_quantity());

                incoming_order.set_quantity(incoming_order.get_quantity() - traded_quantity);
                resting_order.set_quantity(resting_order.get_quantity() - traded_quantity);

                if (resting_order.get_quantity() == 0)
                {
                    ask_queue.pop_front();
                }
            }

            if (ask_queue.empty())
            {
                it = asks.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    else if (incoming_order.get_side() == OrderSide::SELL)
    {
        for (auto it = bids.begin(); it != bids.end() && incoming_order.get_quantity() > 0;)
        {
            double bid_price = it->first;

            // limit order
            if (incoming_order.get_type() == OrderType::LIMIT && incoming_order.get_price() > bid_price)
            {
                break;
            }

            // market order
            OrderQueue &bid_queue = it->second;
            while (!bid_queue.empty() && incoming_order.get_quantity() > 0)
            {
                Order &resting_order = bid_queue.front();

                int traded_quantity = std::min(incoming_order.get_quantity(), resting_order.get_quantity());

                incoming_order.set_quantity(incoming_order.get_quantity() - traded_quantity);
                resting_order.set_quantity(resting_order.get_quantity() - traded_quantity);

                if (resting_order.get_quantity() == 0)
                {
                    bid_queue.pop_front();
                }
            }

            if (bid_queue.empty())
            {
                it = bids.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // add unfilled order to book
    if (incoming_order.get_quantity() > 0 && incoming_order.get_type() == OrderType::LIMIT)
    {
        add_order(incoming_order);
    }
}
