#pragma once

#include <string>
#include "Strategy.h"

enum class OrderType
{
    MARKET,
    LIMIT
};

enum class OrderSide
{
    BUY,
    SELL
};

enum class OrderStatus
{
    PENDING,
    FILLED,
    CANCELLED,
    REJECTED
};

class Order
{
public:
    Order(Strategy strategy, int quantity, double price, OrderSide side, OrderType type);
    ~Order();

    uint64_t get_id() const;
    Strategy get_strategy() const;
    int get_quantity() const;
    double get_price() const;
    OrderSide get_side() const;
    OrderType get_type() const;
    OrderStatus get_status() const;

    void set_quantity(int quantity);
    void set_price(double price);
    void set_type(OrderType type);
    void set_status(OrderStatus status);

private:
    uint64_t id;
    Strategy strategy;
    int quantity;
    double price;
    OrderSide side;
    OrderType type;
    OrderStatus status;
    std::chrono::system_clock::time_point created_at;
};