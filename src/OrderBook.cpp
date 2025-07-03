#include "OrderBook.h"

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
    add_order_to_book(order, bids);
}

void OrderBook::remove_order(Order &order)
{
    remove_order_from_book(order, bids);
}

void OrderBook::update_order(Order &order)
{
    update_order_in_book(order, bids, order.get_side());
}

void OrderBook::cancel_order(Order &order)
{
    remove_order_from_book(order, bids);
}

double OrderBook::get_best_bid() const
{
    return bids.begin()->first;
}

double OrderBook::get_best_ask() const
{
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

void OrderBook::add_order_to_book(Order &order, std::map<double, OrderQueue, std::greater<double>> &book)
{
    book[order.get_price()].push_back(order);
}

void OrderBook::remove_order_from_book(Order &order, std::map<double, OrderQueue, std::greater<double>> &book)
{
    auto it = book.find(order.get_price());
    if (it != book.end())
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
            book.erase(it);
        }
    }
}

void OrderBook::update_order_in_book(Order &order, std::map<double, OrderQueue, std::greater<double>> &book, OrderSide side)
{
    remove_order_from_book(order, book);
    add_order_to_book(order, book);
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

    // add unfill order to book
    if (incoming_order.get_quantity() > 0 && incoming_order.get_type() == OrderType::LIMIT)
    {
        add_order(incoming_order);
    }
}
