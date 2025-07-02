#include "Order.h"

Order::Order(Strategy strategy, int quantity, double price, OrderSide side, OrderType type)
{
    id = std::chrono::system_clock::now().time_since_epoch().count();
    strategy = strategy;
    quantity = quantity;
    price = price;
    side = side;
    type = type;
    status = OrderStatus::PENDING;
    created_at = std::chrono::system_clock::now();
}

uint64_t Order::get_id() const
{
    return id;
}

Strategy Order::get_strategy() const
{
    return strategy;
}

int Order::get_quantity() const
{
    return quantity;
}

double Order::get_price() const
{
    return price;
}

OrderSide Order::get_side() const
{
    return side;
}

OrderType Order::get_type() const
{
    return type;
}

OrderStatus Order::get_status() const
{
    return status;
}

void Order::set_quantity(int quantity)
{
    quantity = quantity;
}

void Order::set_price(double price)
{
    price = price;
}

void Order::set_type(OrderType type)
{
    type = type;
}

void Order::set_status(OrderStatus status)
{
    status = status;
}
