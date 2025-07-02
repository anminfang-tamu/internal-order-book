#include "OrderBook.h"

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
