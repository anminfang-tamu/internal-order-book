#include <gtest/gtest.h>
#include "OrderBook.h"

class OrderBookTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // This method runs before each test
        orderbook = new OrderBook();

        // Create some sample orders for testing
        buy_order_1 = new Order(Strategy::HIGH_FREQUENCY, 100, 50.0, OrderSide::BUY, OrderType::LIMIT);
        buy_order_2 = new Order(Strategy::ALGORITHMIC_TRADING, 200, 49.0, OrderSide::BUY, OrderType::LIMIT);
        sell_order_1 = new Order(Strategy::HEDGE_FUND, 150, 51.0, OrderSide::SELL, OrderType::LIMIT);
        sell_order_2 = new Order(Strategy::OTHER, 75, 52.0, OrderSide::SELL, OrderType::LIMIT);
    }

    void TearDown() override
    {
        // This method runs after each test
        delete orderbook;
        delete buy_order_1;
        delete buy_order_2;
        delete sell_order_1;
        delete sell_order_2;
    }

    // Test fixtures - available to all tests
    OrderBook *orderbook;
    Order *buy_order_1;
    Order *buy_order_2;
    Order *sell_order_1;
    Order *sell_order_2;
};

// Test empty orderbook behavior
TEST_F(OrderBookTest, EmptyOrderBookThrowsOnBestBid)
{
    // Empty orderbook should handle get_best_bid gracefully
    EXPECT_THROW(orderbook->get_best_bid(), std::exception);
}

TEST_F(OrderBookTest, EmptyOrderBookThrowsOnBestAsk)
{
    // Empty orderbook should handle get_best_ask gracefully
    EXPECT_THROW(orderbook->get_best_ask(), std::exception);
}

// Test adding orders
TEST_F(OrderBookTest, AddSingleBuyOrder)
{
    EXPECT_NO_THROW(orderbook->add_order(*buy_order_1));

    // Should be able to get best bid now
    EXPECT_NO_THROW(orderbook->get_best_bid());
}

TEST_F(OrderBookTest, AddSingleSellOrder)
{
    EXPECT_NO_THROW(orderbook->add_order(*sell_order_1));

    // Should be able to get best ask now
    EXPECT_NO_THROW(orderbook->get_best_ask());
}

// Test best bid/ask functionality
TEST_F(OrderBookTest, BestBidIsHighestPrice)
{
    orderbook->add_order(*buy_order_1); // 50.0
    orderbook->add_order(*buy_order_2); // 49.0

    // Best bid should be the highest price (50.0)
    EXPECT_DOUBLE_EQ(orderbook->get_best_bid(), 50.0);
}

TEST_F(OrderBookTest, BestAskIsLowestPrice)
{
    orderbook->add_order(*sell_order_1); // 51.0
    orderbook->add_order(*sell_order_2); // 52.0

    // Best ask should be the lowest price (51.0)
    EXPECT_DOUBLE_EQ(orderbook->get_best_ask(), 51.0);
}

// Test order retrieval
TEST_F(OrderBookTest, GetBidsReturnsCorrectOrders)
{
    orderbook->add_order(*buy_order_1); // 50.0

    OrderQueue bids_at_50 = orderbook->get_bids(50.0);
    EXPECT_EQ(bids_at_50.size(), 1);
    EXPECT_EQ(bids_at_50.front().get_price(), 50.0);
    EXPECT_EQ(bids_at_50.front().get_quantity(), 100);
}

TEST_F(OrderBookTest, GetAsksReturnsCorrectOrders)
{
    orderbook->add_order(*sell_order_1); // 51.0

    OrderQueue asks_at_51 = orderbook->get_asks(51.0);
    EXPECT_EQ(asks_at_51.size(), 1);
    EXPECT_EQ(asks_at_51.front().get_price(), 51.0);
    EXPECT_EQ(asks_at_51.front().get_quantity(), 150);
}

// Test order matching
TEST_F(OrderBookTest, MarketBuyOrderMatchesWithBestAsk)
{
    // Add a sell order first
    orderbook->add_order(*sell_order_1); // Sell 150 @ 51.0

    // Create a market buy order
    Order market_buy(Strategy::OTHER, 100, 0.0, OrderSide::BUY, OrderType::MARKET);

    // This should match and reduce the sell order quantity
    orderbook->match_orders(market_buy);

    // The market order should be fully filled (quantity = 0)
    EXPECT_EQ(market_buy.get_quantity(), 0);
}

// Test multiple orders at same price
TEST_F(OrderBookTest, MultipleOrdersAtSamePrice)
{
    // Create two buy orders at the same price
    Order buy_a(Strategy::OTHER, 100, 50.0, OrderSide::BUY, OrderType::LIMIT);
    Order buy_b(Strategy::OTHER, 200, 50.0, OrderSide::BUY, OrderType::LIMIT);

    orderbook->add_order(buy_a);
    orderbook->add_order(buy_b);

    OrderQueue bids_at_50 = orderbook->get_bids(50.0);
    EXPECT_EQ(bids_at_50.size(), 2);

    // Should still return 50.0 as best bid
    EXPECT_DOUBLE_EQ(orderbook->get_best_bid(), 50.0);
}

// Test order removal
TEST_F(OrderBookTest, RemoveOrderReducesQueueSize)
{
    orderbook->add_order(*buy_order_1);

    // Verify it was added
    OrderQueue bids_before = orderbook->get_bids(50.0);
    EXPECT_EQ(bids_before.size(), 1);

    // Remove the order
    orderbook->remove_order(*buy_order_1);

    // Should be empty now
    OrderQueue bids_after = orderbook->get_bids(50.0);
    EXPECT_EQ(bids_after.size(), 0);
}

// Test order cancellation
TEST_F(OrderBookTest, CancelOrderRemovesFromBook)
{
    orderbook->add_order(*buy_order_1);

    // Cancel should remove the order
    EXPECT_NO_THROW(orderbook->cancel_order(*buy_order_1));

    OrderQueue bids_after_cancel = orderbook->get_bids(50.0);
    EXPECT_EQ(bids_after_cancel.size(), 0);
}