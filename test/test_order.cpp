#include <gtest/gtest.h>
#include "Order.h"

class OrderTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // This method runs before each test
    }

    void TearDown() override
    {
        // This method runs after each test
    }
};

// Test Order Constructor
TEST_F(OrderTest, ConstructorInitializesAllFields)
{
    // Arrange
    Strategy strategy = Strategy::HIGH_FREQUENCY;
    int quantity = 100;
    double price = 50.25;
    OrderSide side = OrderSide::BUY;
    OrderType type = OrderType::LIMIT;

    // Act
    Order order(strategy, quantity, price, side, type);

    // Assert
    EXPECT_EQ(order.get_strategy(), strategy);
    EXPECT_EQ(order.get_quantity(), quantity);
    EXPECT_DOUBLE_EQ(order.get_price(), price);
    EXPECT_EQ(order.get_side(), side);
    EXPECT_EQ(order.get_type(), type);
    EXPECT_EQ(order.get_status(), OrderStatus::PENDING); // Default status
    EXPECT_GT(order.get_id(), 0);                        // ID should be generated and positive
}

TEST_F(OrderTest, ConstructorGeneratesUniqueIds)
{
    // Arrange & Act
    Order order1(Strategy::QUANT_LONG_TERM, 50, 25.0, OrderSide::BUY, OrderType::MARKET);
    Order order2(Strategy::ALGORITHMIC_TRADING, 75, 30.0, OrderSide::SELL, OrderType::LIMIT);

    // Assert
    EXPECT_NE(order1.get_id(), order2.get_id()); // IDs should be unique
}

// Test Getters
TEST_F(OrderTest, GettersReturnCorrectValues)
{
    // Arrange
    Strategy strategy = Strategy::HEDGE_FUND;
    int quantity = 200;
    double price = 75.50;
    OrderSide side = OrderSide::SELL;
    OrderType type = OrderType::MARKET;

    Order order(strategy, quantity, price, side, type);

    // Act & Assert
    EXPECT_EQ(order.get_strategy(), Strategy::HEDGE_FUND);
    EXPECT_EQ(order.get_quantity(), 200);
    EXPECT_DOUBLE_EQ(order.get_price(), 75.50);
    EXPECT_EQ(order.get_side(), OrderSide::SELL);
    EXPECT_EQ(order.get_type(), OrderType::MARKET);
    EXPECT_EQ(order.get_status(), OrderStatus::PENDING);
    EXPECT_GT(order.get_id(), 0);
}

// Test Setters
TEST_F(OrderTest, SetQuantityUpdatesValue)
{
    // Arrange
    Order order(Strategy::OTHER, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);

    // Act
    order.set_quantity(150);

    // Assert
    EXPECT_EQ(order.get_quantity(), 150);
}

TEST_F(OrderTest, SetPriceUpdatesValue)
{
    // Arrange
    Order order(Strategy::INVESTMENT_BANK, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);

    // Act
    order.set_price(25.75);

    // Assert
    EXPECT_DOUBLE_EQ(order.get_price(), 25.75);
}

TEST_F(OrderTest, SetTypeUpdatesValue)
{
    // Arrange
    Order order(Strategy::PENSION_FUND, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);

    // Act
    order.set_type(OrderType::MARKET);

    // Assert
    EXPECT_EQ(order.get_type(), OrderType::MARKET);
}

TEST_F(OrderTest, SetStatusUpdatesValue)
{
    // Arrange
    Order order(Strategy::INSURANCE_COMPANY, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);

    // Act
    order.set_status(OrderStatus::FILLED);

    // Assert
    EXPECT_EQ(order.get_status(), OrderStatus::FILLED);
}

// Test Multiple Setters
TEST_F(OrderTest, MultipleSettersWork)
{
    // Arrange
    Order order(Strategy::QUANT_LONG_TERM, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);

    // Act
    order.set_quantity(250);
    order.set_price(45.25);
    order.set_type(OrderType::MARKET);
    order.set_status(OrderStatus::CANCELLED);

    // Assert
    EXPECT_EQ(order.get_quantity(), 250);
    EXPECT_DOUBLE_EQ(order.get_price(), 45.25);
    EXPECT_EQ(order.get_type(), OrderType::MARKET);
    EXPECT_EQ(order.get_status(), OrderStatus::CANCELLED);

    // Original values should remain unchanged
    EXPECT_EQ(order.get_strategy(), Strategy::QUANT_LONG_TERM);
    EXPECT_EQ(order.get_side(), OrderSide::BUY);
    EXPECT_GT(order.get_id(), 0);
}

// Test Edge Cases
TEST_F(OrderTest, ZeroQuantityOrder)
{
    // Arrange & Act
    Order order(Strategy::HIGH_FREQUENCY, 0, 10.0, OrderSide::BUY, OrderType::LIMIT);

    // Assert
    EXPECT_EQ(order.get_quantity(), 0);
}

TEST_F(OrderTest, ZeroPriceOrder)
{
    // Arrange & Act
    Order order(Strategy::HIGH_FREQUENCY, 100, 0.0, OrderSide::BUY, OrderType::MARKET);

    // Assert
    EXPECT_DOUBLE_EQ(order.get_price(), 0.0);
}

TEST_F(OrderTest, NegativeQuantity)
{
    // Arrange & Act
    Order order(Strategy::ALGORITHMIC_TRADING, -50, 10.0, OrderSide::SELL, OrderType::LIMIT);

    // Assert
    EXPECT_EQ(order.get_quantity(), -50);
}

TEST_F(OrderTest, HighPrecisionPrice)
{
    // Arrange & Act
    double precise_price = 123.456789;
    Order order(Strategy::HEDGE_FUND, 100, precise_price, OrderSide::BUY, OrderType::LIMIT);

    // Assert
    EXPECT_DOUBLE_EQ(order.get_price(), precise_price);
}

// Test All Enum Values
TEST_F(OrderTest, AllStrategyEnumsWork)
{
    Order order1(Strategy::QUANT_LONG_TERM, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);
    Order order2(Strategy::HIGH_FREQUENCY, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);
    Order order3(Strategy::HEDGE_FUND, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);
    Order order4(Strategy::ALGORITHMIC_TRADING, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);
    Order order5(Strategy::INVESTMENT_BANK, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);
    Order order6(Strategy::PENSION_FUND, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);
    Order order7(Strategy::INSURANCE_COMPANY, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);
    Order order8(Strategy::OTHER, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);

    EXPECT_EQ(order1.get_strategy(), Strategy::QUANT_LONG_TERM);
    EXPECT_EQ(order2.get_strategy(), Strategy::HIGH_FREQUENCY);
    EXPECT_EQ(order3.get_strategy(), Strategy::HEDGE_FUND);
    EXPECT_EQ(order4.get_strategy(), Strategy::ALGORITHMIC_TRADING);
    EXPECT_EQ(order5.get_strategy(), Strategy::INVESTMENT_BANK);
    EXPECT_EQ(order6.get_strategy(), Strategy::PENSION_FUND);
    EXPECT_EQ(order7.get_strategy(), Strategy::INSURANCE_COMPANY);
    EXPECT_EQ(order8.get_strategy(), Strategy::OTHER);
}

TEST_F(OrderTest, AllOrderSidesWork)
{
    Order buyOrder(Strategy::OTHER, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);
    Order sellOrder(Strategy::OTHER, 100, 10.0, OrderSide::SELL, OrderType::LIMIT);

    EXPECT_EQ(buyOrder.get_side(), OrderSide::BUY);
    EXPECT_EQ(sellOrder.get_side(), OrderSide::SELL);
}

TEST_F(OrderTest, AllOrderTypesWork)
{
    Order limitOrder(Strategy::OTHER, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);
    Order marketOrder(Strategy::OTHER, 100, 10.0, OrderSide::BUY, OrderType::MARKET);

    EXPECT_EQ(limitOrder.get_type(), OrderType::LIMIT);
    EXPECT_EQ(marketOrder.get_type(), OrderType::MARKET);
}

TEST_F(OrderTest, AllOrderStatusesWork)
{
    Order order(Strategy::OTHER, 100, 10.0, OrderSide::BUY, OrderType::LIMIT);

    order.set_status(OrderStatus::PENDING);
    EXPECT_EQ(order.get_status(), OrderStatus::PENDING);

    order.set_status(OrderStatus::FILLED);
    EXPECT_EQ(order.get_status(), OrderStatus::FILLED);

    order.set_status(OrderStatus::CANCELLED);
    EXPECT_EQ(order.get_status(), OrderStatus::CANCELLED);

    order.set_status(OrderStatus::REJECTED);
    EXPECT_EQ(order.get_status(), OrderStatus::REJECTED);
}