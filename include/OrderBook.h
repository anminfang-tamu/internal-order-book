#pragma once

#include "Order.h"

#include <map>
#include <deque>

using OrderQueue = std::deque<Order>;

class OrderBook
{
public:
    OrderBook();
    ~OrderBook();

    void add_order(Order &order);
    void remove_order(Order &order);
    void update_order(Order &order);
    void cancel_order(Order &order);

    double get_best_bid() const;
    double get_best_ask() const;

    OrderQueue get_bids(double price) const;
    OrderQueue get_asks(double price) const;

    void match_orders(Order &order);

private:
    std::map<double, OrderQueue, std::greater<double>> bids;
    std::map<double, OrderQueue, std::less<double>> asks;

    void add_order_to_book(Order &order);
    void remove_order_from_book(Order &order);
    void update_order_in_book(Order &order);
};