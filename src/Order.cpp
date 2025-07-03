#include "Order.h"
#include <chrono>

static uint64_t order_counter = 0;

Order::Order(Strategy strategy, int quantity, double price, OrderSide side, OrderType type)
{
    this->id = std::chrono::high_resolution_clock::now().time_since_epoch().count() + (++order_counter);
    this->strategy = strategy;
    this->quantity = quantity;
    this->price = price;
    this->side = side;
    this->type = type;
    this->status = OrderStatus::PENDING;
    this->created_at = std::chrono::system_clock::now();
}

Order::Order()
{
    // Default constructor - initialize with default values
    this->id = std::chrono::high_resolution_clock::now().time_since_epoch().count() + (++order_counter);
    this->strategy = Strategy::OTHER;
    this->quantity = 0;
    this->price = 0.0;
    this->side = OrderSide::BUY;
    this->type = OrderType::MARKET;
    this->status = OrderStatus::PENDING;
    this->created_at = std::chrono::system_clock::now();
}

Order::~Order()
{
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
    this->quantity = quantity;
}

void Order::set_price(double price)
{
    this->price = price;
}

void Order::set_type(OrderType type)
{
    this->type = type;
}

void Order::set_status(OrderStatus status)
{
    this->status = status;
}
