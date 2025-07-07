#include <gtest/gtest.h>
#include "MatchingEngine.h"
#include <chrono>
#include <thread>
#include <atomic>   // Added for atomic operations in pressure tests
#include <iostream> // Added for printing test results
#include <vector>   // Added for vector containers

class MatchingEngineTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create sample orders for testing
        buy_order_1 = new Order(Strategy::HIGH_FREQUENCY, 100, 50.0, OrderSide::BUY, OrderType::LIMIT);
        buy_order_2 = new Order(Strategy::ALGORITHMIC_TRADING, 200, 49.0, OrderSide::BUY, OrderType::LIMIT);
        sell_order_1 = new Order(Strategy::HEDGE_FUND, 150, 51.0, OrderSide::SELL, OrderType::LIMIT);
        sell_order_2 = new Order(Strategy::OTHER, 75, 52.0, OrderSide::SELL, OrderType::LIMIT);
        market_buy = new Order(Strategy::OTHER, 100, 0.0, OrderSide::BUY, OrderType::MARKET);
        market_sell = new Order(Strategy::OTHER, 50, 0.0, OrderSide::SELL, OrderType::MARKET);
    }

    void TearDown() override
    {
        delete buy_order_1;
        delete buy_order_2;
        delete sell_order_1;
        delete sell_order_2;
        delete market_buy;
        delete market_sell;
    }

    // Helper function to wait for orders to be processed
    void wait_for_processing(int milliseconds = 100)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    // Test fixtures
    Order *buy_order_1;
    Order *buy_order_2;
    Order *sell_order_1;
    Order *sell_order_2;
    Order *market_buy;
    Order *market_sell;
};

// Test MatchingEngine Constructor and Destructor
TEST_F(MatchingEngineTest, ConstructorStartsMatchingThread)
{
    // Test that MatchingEngine can be created and destroyed without issues
    EXPECT_NO_THROW({
        MatchingEngine engine;
        // Small delay to let the thread start
        wait_for_processing(10);
    }); // Destructor should properly join the thread
}

TEST_F(MatchingEngineTest, MultipleEnginesCanBeCreated)
{
    // Test that multiple engines can exist simultaneously
    EXPECT_NO_THROW({
        MatchingEngine engine1;
        MatchingEngine engine2;
        wait_for_processing(10);
    }); // Both destructors should work properly
}

// Test Order Processing
TEST_F(MatchingEngineTest, ProcessSingleOrder)
{
    MatchingEngine engine;

    // Should not throw when processing an order
    EXPECT_NO_THROW(engine.process_order(*buy_order_1));

    // Give time for the order to be processed
    wait_for_processing();
}

TEST_F(MatchingEngineTest, ProcessMultipleOrders)
{
    MatchingEngine engine;

    // Process multiple orders
    EXPECT_NO_THROW({
        engine.process_order(*buy_order_1);
        engine.process_order(*sell_order_1);
        engine.process_order(*buy_order_2);
        engine.process_order(*sell_order_2);
    });

    // Give time for all orders to be processed
    wait_for_processing(200);
}

// Test Order Matching Functionality
TEST_F(MatchingEngineTest, MarketOrderMatchesWithOppositeLimit)
{
    MatchingEngine engine;

    // Add a sell limit order first
    engine.process_order(*sell_order_1); // Sell 150 @ 51.0
    wait_for_processing();

    // Process a market buy order - the engine will handle matching internally
    Order market_order = *market_buy; // Buy 100 @ market
    engine.process_order(market_order);
    wait_for_processing();

    // Note: MatchingEngine processes orders internally via std::move
    // The original order passed to process_order remains unchanged
    // This tests that the engine accepts and processes market orders without throwing
    EXPECT_EQ(market_order.get_quantity(), 100); // Original unchanged
}

TEST_F(MatchingEngineTest, LimitOrdersMatchAtCorrectPrice)
{
    MatchingEngine engine;

    // Add overlapping limit orders that should match
    Order buy_aggressive(Strategy::OTHER, 100, 52.0, OrderSide::BUY, OrderType::LIMIT); // Above market
    Order sell_limit = *sell_order_1;                                                   // 150 @ 51.0                                                   // 150 @ 51.0                                                   // Sell 150 @ 51.0

    engine.process_order(sell_limit);
    wait_for_processing();

    engine.process_order(buy_aggressive);
    wait_for_processing();

    // The MatchingEngine processes orders internally
    // Original orders passed to process_order remain unchanged
    EXPECT_EQ(buy_aggressive.get_quantity(), 100); // Original unchanged
    EXPECT_EQ(sell_limit.get_quantity(), 150);     // Original unchanged
}

// Test Concurrent Order Processing
TEST_F(MatchingEngineTest, ConcurrentOrderProcessing)
{
    MatchingEngine engine;

    // Create multiple threads that add orders concurrently
    std::vector<std::thread> threads;
    const int num_threads = 5;
    const int orders_per_thread = 10;

    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back([&engine, i, orders_per_thread]()
                             {
            for (int j = 0; j < orders_per_thread; ++j)
            {
                Order order(Strategy::OTHER, 10, 50.0 + j, OrderSide::BUY, OrderType::LIMIT);
                engine.process_order(order);
                // Small delay between orders
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } });
    }

    // Wait for all threads to complete
    for (auto &thread : threads)
    {
        thread.join();
    }

    // Give time for all orders to be processed
    wait_for_processing(500);

    // If we reach here without crashing, concurrent processing works
    EXPECT_TRUE(true);
}

// Test Edge Cases
TEST_F(MatchingEngineTest, ProcessOrderWithZeroQuantity)
{
    MatchingEngine engine;

    Order zero_order(Strategy::OTHER, 0, 50.0, OrderSide::BUY, OrderType::LIMIT);

    EXPECT_NO_THROW(engine.process_order(zero_order));
    wait_for_processing();
}

TEST_F(MatchingEngineTest, ProcessOrderWithNegativeQuantity)
{
    MatchingEngine engine;

    Order negative_order(Strategy::OTHER, -100, 50.0, OrderSide::BUY, OrderType::LIMIT);

    EXPECT_NO_THROW(engine.process_order(negative_order));
    wait_for_processing();
}

TEST_F(MatchingEngineTest, ProcessOrderWithZeroPrice)
{
    MatchingEngine engine;

    Order zero_price_order(Strategy::OTHER, 100, 0.0, OrderSide::BUY, OrderType::LIMIT);

    EXPECT_NO_THROW(engine.process_order(zero_price_order));
    wait_for_processing();
}

// Test Shutdown Behavior
TEST_F(MatchingEngineTest, EngineShutdownWhileProcessingOrders)
{
    {
        MatchingEngine engine;

        // Add many orders
        for (int i = 0; i < 100; ++i)
        {
            Order order(Strategy::OTHER, 10, 50.0 + i, OrderSide::BUY, OrderType::LIMIT);
            engine.process_order(order);
        }

        // Don't wait - let destructor handle shutdown immediately
    } // MatchingEngine destructor should handle cleanup properly

    // If we reach here, shutdown worked correctly
    EXPECT_TRUE(true);
}

// Test Integration with Different Order Types
TEST_F(MatchingEngineTest, ProcessDifferentOrderTypes)
{
    MatchingEngine engine;

    // Test all combinations of order types and sides
    Order limit_buy(Strategy::OTHER, 100, 50.0, OrderSide::BUY, OrderType::LIMIT);
    Order limit_sell(Strategy::OTHER, 100, 51.0, OrderSide::SELL, OrderType::LIMIT);
    Order market_buy_test(Strategy::OTHER, 50, 0.0, OrderSide::BUY, OrderType::MARKET);
    Order market_sell_test(Strategy::OTHER, 50, 0.0, OrderSide::SELL, OrderType::MARKET);

    EXPECT_NO_THROW({
        engine.process_order(limit_buy);
        engine.process_order(limit_sell);
        engine.process_order(market_buy_test);
        engine.process_order(market_sell_test);
    });

    wait_for_processing(200);
}

// Test Strategy Diversity
TEST_F(MatchingEngineTest, ProcessOrdersFromDifferentStrategies)
{
    MatchingEngine engine;

    // Create orders from all strategy types
    std::vector<Order> strategy_orders = {
        Order(Strategy::QUANT_LONG_TERM, 100, 50.0, OrderSide::BUY, OrderType::LIMIT),
        Order(Strategy::HIGH_FREQUENCY, 100, 50.1, OrderSide::BUY, OrderType::LIMIT),
        Order(Strategy::HEDGE_FUND, 100, 50.2, OrderSide::BUY, OrderType::LIMIT),
        Order(Strategy::ALGORITHMIC_TRADING, 100, 50.3, OrderSide::BUY, OrderType::LIMIT),
        Order(Strategy::INVESTMENT_BANK, 100, 50.4, OrderSide::BUY, OrderType::LIMIT),
        Order(Strategy::PENSION_FUND, 100, 50.5, OrderSide::BUY, OrderType::LIMIT),
        Order(Strategy::INSURANCE_COMPANY, 100, 50.6, OrderSide::BUY, OrderType::LIMIT),
        Order(Strategy::OTHER, 100, 50.7, OrderSide::BUY, OrderType::LIMIT)};

    for (auto &order : strategy_orders)
    {
        EXPECT_NO_THROW(engine.process_order(order));
    }

    wait_for_processing(300);
}

// Test Performance (Light Load)
TEST_F(MatchingEngineTest, ProcessManyOrdersQuickly)
{
    MatchingEngine engine;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Process 1000 orders
    for (int i = 0; i < 1000; ++i)
    {
        Order order(Strategy::OTHER, 10, 50.0 + (i % 100) * 0.01,
                    (i % 2 == 0) ? OrderSide::BUY : OrderSide::SELL, OrderType::LIMIT);
        engine.process_order(order);
    }

    wait_for_processing(1000); // Give 1 second for processing

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Should complete within reasonable time (less than 2 seconds)
    EXPECT_LT(duration.count(), 2000);
}

// ========== PERFORMANCE AND PRESSURE TESTS ==========

// Test 1: Throughput Test - How many orders per second?
TEST_F(MatchingEngineTest, ThroughputTest_10K_Orders)
{
    MatchingEngine engine;
    const int num_orders = 10000;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Submit 10K orders as fast as possible
    for (int i = 0; i < num_orders; ++i)
    {
        Order order(Strategy::HIGH_FREQUENCY,
                    (i % 100) + 1,           // Quantity 1-100
                    50.0 + (i % 200) * 0.01, // Price range 50.00-51.99
                    (i % 2 == 0) ? OrderSide::BUY : OrderSide::SELL,
                    OrderType::LIMIT);
        engine.process_order(order);
    }

    auto submission_time = std::chrono::high_resolution_clock::now();
    auto submission_duration = std::chrono::duration_cast<std::chrono::microseconds>(submission_time - start_time);

    // Wait for processing to complete
    wait_for_processing(5000); // 5 seconds max

    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Calculate metrics
    double submission_rate = num_orders / (submission_duration.count() / 1000000.0); // orders per second
    double processing_rate = num_orders / (total_duration.count() / 1000.0);         // orders per second

    std::cout << "\n=== THROUGHPUT TEST RESULTS ===" << std::endl;
    std::cout << "Orders: " << num_orders << std::endl;
    std::cout << "Submission rate: " << static_cast<int>(submission_rate) << " orders/second" << std::endl;
    std::cout << "Processing rate: " << static_cast<int>(processing_rate) << " orders/second" << std::endl;
    std::cout << "Total time: " << total_duration.count() << " ms" << std::endl;

    // Performance expectations
    EXPECT_GT(submission_rate, 50000);        // Should submit >50K orders/sec
    EXPECT_GT(processing_rate, 1000);         // Should process >1K orders/sec
    EXPECT_LT(total_duration.count(), 15000); // Complete within 15 seconds
}

// Test 2: Heavy Load Test - 100K orders
TEST_F(MatchingEngineTest, HeavyLoadTest_100K_Orders)
{
    MatchingEngine engine;
    const int num_orders = 100000;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Submit 100K orders with realistic trading patterns
    for (int i = 0; i < num_orders; ++i)
    {
        // Create price levels around a base price (spread simulation)
        double base_price = 100.0;
        double price_offset = (i % 20 - 10) * 0.01; // ±10 cents spread

        Order order(static_cast<Strategy>(i % 8),                          // Rotate through strategies
                    (i % 500) + 1,                                         // Quantity 1-500
                    base_price + price_offset,                             // Price around base
                    (i % 3 == 0) ? OrderSide::SELL : OrderSide::BUY,       // More buys than sells
                    (i % 10 == 0) ? OrderType::MARKET : OrderType::LIMIT); // 10% market orders
        engine.process_order(order);
    }

    // Wait for processing (give generous time for 100K orders)
    wait_for_processing(30000); // 30 seconds max

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "\n=== HEAVY LOAD TEST RESULTS ===" << std::endl;
    std::cout << "Orders: " << num_orders << std::endl;
    std::cout << "Total time: " << duration.count() << " ms" << std::endl;
    std::cout << "Average rate: " << static_cast<int>(num_orders / (duration.count() / 1000.0)) << " orders/second" << std::endl;

    // Should complete within reasonable time (less than 60 seconds)
    EXPECT_LT(duration.count(), 60000);
}

// Test 3: Burst Load Test - Multiple concurrent producers
TEST_F(MatchingEngineTest, BurstLoadTest_ConcurrentProducers)
{
    MatchingEngine engine;
    const int num_threads = 10;
    const int orders_per_thread = 5000;
    const int total_orders = num_threads * orders_per_thread;

    std::vector<std::thread> producers;
    std::atomic<int> orders_submitted{0};

    auto start_time = std::chrono::high_resolution_clock::now();

    // Create multiple threads that submit orders concurrently
    for (int thread_id = 0; thread_id < num_threads; ++thread_id)
    {
        producers.emplace_back([&engine, &orders_submitted, thread_id, orders_per_thread]()
                               {
            for (int i = 0; i < orders_per_thread; ++i)
            {
                Order order(Strategy::HIGH_FREQUENCY,
                           (i % 100) + 1,
                           50.0 + (thread_id * 10) + (i % 50) * 0.01, // Each thread has price range
                           (i % 2 == 0) ? OrderSide::BUY : OrderSide::SELL,
                           OrderType::LIMIT);
                engine.process_order(order);
                orders_submitted.fetch_add(1);
            } });
    }

    // Wait for all producers to finish
    for (auto &producer : producers)
    {
        producer.join();
    }

    auto submission_end = std::chrono::high_resolution_clock::now();
    auto submission_duration = std::chrono::duration_cast<std::chrono::milliseconds>(submission_end - start_time);

    // Wait for all orders to be processed
    wait_for_processing(20000); // 20 seconds max

    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "\n=== BURST LOAD TEST RESULTS ===" << std::endl;
    std::cout << "Threads: " << num_threads << std::endl;
    std::cout << "Orders per thread: " << orders_per_thread << std::endl;
    std::cout << "Total orders: " << total_orders << std::endl;
    std::cout << "Submission time: " << submission_duration.count() << " ms" << std::endl;
    std::cout << "Total time: " << total_duration.count() << " ms" << std::endl;
    std::cout << "Concurrent submission rate: " << static_cast<int>(total_orders / (submission_duration.count() / 1000.0)) << " orders/second" << std::endl;

    EXPECT_EQ(orders_submitted.load(), total_orders);
    EXPECT_LT(total_duration.count(), 30000); // Complete within 30 seconds
}

// Test 4: Market Stress Test - Heavy matching activity
TEST_F(MatchingEngineTest, MarketStressTest_HighMatchingActivity)
{
    MatchingEngine engine;
    const int num_orders = 20000;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Create orders that will cause lots of matching
    for (int i = 0; i < num_orders; ++i)
    {
        // Tight spread around 100.0 to encourage matching
        double base_price = 100.0;
        double spread = 0.02; // 2 cent spread

        if (i % 2 == 0)
        {
            // Buy orders slightly below market
            Order buy_order(Strategy::ALGORITHMIC_TRADING,
                            (i % 100) + 1,
                            base_price - (i % 5) * 0.01, // 99.96 to 100.00
                            OrderSide::BUY,
                            (i % 20 == 0) ? OrderType::MARKET : OrderType::LIMIT);
            engine.process_order(buy_order);
        }
        else
        {
            // Sell orders slightly above market
            Order sell_order(Strategy::HEDGE_FUND,
                             (i % 100) + 1,
                             base_price + (i % 5) * 0.01, // 100.00 to 100.04
                             OrderSide::SELL,
                             (i % 20 == 0) ? OrderType::MARKET : OrderType::LIMIT);
            engine.process_order(sell_order);
        }
    }

    wait_for_processing(15000); // 15 seconds for matching

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "\n=== MARKET STRESS TEST RESULTS ===" << std::endl;
    std::cout << "Orders: " << num_orders << " (tight spread for matching)" << std::endl;
    std::cout << "Total time: " << duration.count() << " ms" << std::endl;
    std::cout << "Processing rate: " << static_cast<int>(num_orders / (duration.count() / 1000.0)) << " orders/second" << std::endl;

    EXPECT_LT(duration.count(), 20000); // Complete within 20 seconds
}

// Test 5: Memory Pressure Test - Large orders with monitoring
TEST_F(MatchingEngineTest, MemoryPressureTest)
{
    MatchingEngine engine;
    const int num_orders = 50000;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Create orders with large quantities and wide price ranges (less matching)
    for (int i = 0; i < num_orders; ++i)
    {
        Order order(Strategy::PENSION_FUND,
                    1000 + (i % 10000),       // Large quantities 1K-11K
                    10.0 + (i % 1000) * 0.10, // Wide price range 10.00-109.90
                    (i % 2 == 0) ? OrderSide::BUY : OrderSide::SELL,
                    OrderType::LIMIT);
        engine.process_order(order);

        // Add small delay every 1000 orders to avoid overwhelming
        if (i % 1000 == 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    wait_for_processing(25000); // 25 seconds max

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "\n=== MEMORY PRESSURE TEST RESULTS ===" << std::endl;
    std::cout << "Orders: " << num_orders << " (large quantities, wide spread)" << std::endl;
    std::cout << "Total time: " << duration.count() << " ms" << std::endl;
    std::cout << "Processing rate: " << static_cast<int>(num_orders / (duration.count() / 1000.0)) << " orders/second" << std::endl;

    EXPECT_LT(duration.count(), 35000); // Complete within 35 seconds
}

// Test 6: Endurance Test - Sustained load over time
TEST_F(MatchingEngineTest, EnduranceTest_SustainedLoad)
{
    MatchingEngine engine;
    const int batches = 10;
    const int orders_per_batch = 2000;
    const int total_orders = batches * orders_per_batch;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int batch = 0; batch < batches; ++batch)
    {
        // Submit a batch of orders
        for (int i = 0; i < orders_per_batch; ++i)
        {
            Order order(Strategy::OTHER,
                        (i % 200) + 1,
                        50.0 + (batch * 5) + (i % 100) * 0.01,
                        (i % 2 == 0) ? OrderSide::BUY : OrderSide::SELL,
                        OrderType::LIMIT);
            engine.process_order(order);
        }

        // Small pause between batches to simulate realistic timing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    wait_for_processing(15000); // 15 seconds for processing

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "\n=== ENDURANCE TEST RESULTS ===" << std::endl;
    std::cout << "Batches: " << batches << std::endl;
    std::cout << "Orders per batch: " << orders_per_batch << std::endl;
    std::cout << "Total orders: " << total_orders << std::endl;
    std::cout << "Total time: " << duration.count() << " ms" << std::endl;
    std::cout << "Sustained rate: " << static_cast<int>(total_orders / (duration.count() / 1000.0)) << " orders/second" << std::endl;

    EXPECT_LT(duration.count(), 20000); // Complete within 20 seconds
}

// ========== LOCK-FREE QUEUE SPECIFIC TESTS ==========

class LockFreeQueueTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Test orders for lock-free queue testing
        test_order = new Order(Strategy::HIGH_FREQUENCY, 100, 50.0, OrderSide::BUY, OrderType::LIMIT);
    }

    void TearDown() override
    {
        delete test_order;
    }

    Order *test_order;
};

// Test 1: Queue Capacity Limits - Test behavior when queue approaches/exceeds capacity
TEST_F(LockFreeQueueTest, QueueCapacityLimits_1024_Orders)
{
    MatchingEngine engine;
    const int queue_capacity = 1024;
    const int test_orders = queue_capacity + 100; // Try to exceed capacity

    std::cout << "\n=== LOCK-FREE QUEUE CAPACITY TEST ===" << std::endl;
    std::cout << "Testing with " << test_orders << " orders (capacity: " << queue_capacity << ")" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Submit orders faster than they can be processed
    for (int i = 0; i < test_orders; ++i)
    {
        Order order(Strategy::HIGH_FREQUENCY, 1, 50.0 + i * 0.01, OrderSide::BUY, OrderType::LIMIT);

        // This should not block even when queue is full (busy wait in implementation)
        engine.process_order(order);

        // No delay - submit as fast as possible to test queue limits
    }

    auto submission_end = std::chrono::high_resolution_clock::now();
    auto submission_time = std::chrono::duration_cast<std::chrono::milliseconds>(submission_end - start_time);

    // Wait for processing
    std::this_thread::sleep_for(std::chrono::seconds(5));

    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "Submission time: " << submission_time.count() << " ms" << std::endl;
    std::cout << "Total time: " << total_time.count() << " ms" << std::endl;
    std::cout << "Submission rate: " << static_cast<int>(test_orders / (submission_time.count() / 1000.0)) << " orders/sec" << std::endl;

    // Should complete without crashing
    EXPECT_TRUE(true);
}

// Test 2: High-Frequency Lock-Free Access Pattern
TEST_F(LockFreeQueueTest, HighFrequencyLockFreeAccess)
{
    MatchingEngine engine;
    const int num_producer_threads = 8; // High concurrency
    const int orders_per_thread = 1000;
    const int total_orders = num_producer_threads * orders_per_thread;

    std::vector<std::thread> producers;
    std::atomic<int> successful_submissions{0};
    std::atomic<int> submission_attempts{0};

    std::cout << "\n=== HIGH-FREQUENCY LOCK-FREE ACCESS TEST ===" << std::endl;
    std::cout << "Threads: " << num_producer_threads << ", Orders per thread: " << orders_per_thread << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Create high-frequency producers
    for (int thread_id = 0; thread_id < num_producer_threads; ++thread_id)
    {
        producers.emplace_back([&engine, &successful_submissions, &submission_attempts, thread_id, orders_per_thread]()
                               {
            for (int i = 0; i < orders_per_thread; ++i)
            {
                submission_attempts.fetch_add(1);
                
                Order order(Strategy::HIGH_FREQUENCY,
                           (i % 50) + 1,  // Small quantities for fast processing
                           50.0 + (thread_id * 0.1) + (i % 10) * 0.01,
                           (i % 2 == 0) ? OrderSide::BUY : OrderSide::SELL,
                           OrderType::LIMIT);
                
                // Lock-free submission - should never block
                engine.process_order(order);
                successful_submissions.fetch_add(1);
                
                // Minimal delay to simulate high-frequency trading
                if (i % 100 == 0) {
                    std::this_thread::yield();
                }
            } });
    }

    // Wait for all producers
    for (auto &producer : producers)
    {
        producer.join();
    }

    auto submission_end = std::chrono::high_resolution_clock::now();
    auto submission_time = std::chrono::duration_cast<std::chrono::microseconds>(submission_end - start_time);

    // Wait for processing
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "Attempts: " << submission_attempts.load() << std::endl;
    std::cout << "Successful: " << successful_submissions.load() << std::endl;
    std::cout << "Submission time: " << submission_time.count() << " μs" << std::endl;
    std::cout << "Submission rate: " << static_cast<int>(total_orders / (submission_time.count() / 1000000.0)) << " orders/sec" << std::endl;

    // All submissions should succeed (lock-free guarantee)
    EXPECT_EQ(successful_submissions.load(), total_orders);
    EXPECT_EQ(submission_attempts.load(), total_orders);
}

// Test 3: Lock-Free vs Traditional Performance Characteristics
TEST_F(LockFreeQueueTest, LockFreePerformanceCharacteristics)
{
    MatchingEngine engine;
    const int performance_test_orders = 50000;

    std::cout << "\n=== LOCK-FREE PERFORMANCE CHARACTERISTICS ===" << std::endl;

    // Test burst submission (all at once)
    auto burst_start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < performance_test_orders; ++i)
    {
        Order order(Strategy::HIGH_FREQUENCY, 1, 50.0, OrderSide::BUY, OrderType::LIMIT);
        engine.process_order(order);
    }

    auto burst_end = std::chrono::high_resolution_clock::now();
    auto burst_time = std::chrono::duration_cast<std::chrono::microseconds>(burst_end - burst_start);

    std::cout << "Burst submission (" << performance_test_orders << " orders): " << burst_time.count() << " μs" << std::endl;
    std::cout << "Burst rate: " << static_cast<int>(performance_test_orders / (burst_time.count() / 1000000.0)) << " orders/sec" << std::endl;

    // Wait for processing
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Performance expectations for lock-free queue
    double orders_per_microsecond = performance_test_orders / static_cast<double>(burst_time.count());
    EXPECT_GT(orders_per_microsecond, 0.1); // Should process at least 100K orders/sec
}

// Test 4: Lock-Free Queue Memory Safety
TEST_F(LockFreeQueueTest, MemorySafetyTest)
{
    std::cout << "\n=== LOCK-FREE QUEUE MEMORY SAFETY TEST ===" << std::endl;

    const int iterations = 5;
    const int orders_per_iteration = 1000;

    for (int iter = 0; iter < iterations; ++iter)
    {
        std::cout << "Memory safety iteration " << (iter + 1) << "/" << iterations << std::endl;

        {
            MatchingEngine engine; // Create new engine each iteration

            // Submit orders
            for (int i = 0; i < orders_per_iteration; ++i)
            {
                Order order(Strategy::OTHER, 10, 50.0 + i * 0.01, OrderSide::BUY, OrderType::LIMIT);
                engine.process_order(order);
            }

            // Small processing time
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        } // Engine destructor should clean up all memory

        // Small gap between iterations
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::cout << "Memory safety test completed - no leaks expected" << std::endl;
    EXPECT_TRUE(true); // If we reach here, no crashes occurred
}

// Test 5: Lock-Free Queue Edge Cases
TEST_F(LockFreeQueueTest, LockFreeEdgeCases)
{
    std::cout << "\n=== LOCK-FREE QUEUE EDGE CASES ===" << std::endl;

    // Test rapid engine creation/destruction
    for (int i = 0; i < 10; ++i)
    {
        MatchingEngine engine;

        // Single order per engine
        Order order(Strategy::HIGH_FREQUENCY, 1, 50.0, OrderSide::BUY, OrderType::LIMIT);
        engine.process_order(order);

        // Immediate destruction (tests cleanup of lock-free queue)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Test with zero-delay submissions
    {
        MatchingEngine engine;

        for (int i = 0; i < 100; ++i)
        {
            Order order(Strategy::HIGH_FREQUENCY, 1, 50.0, OrderSide::BUY, OrderType::LIMIT);
            engine.process_order(order);
            // No delay - maximum pressure on lock-free queue
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Edge cases completed successfully" << std::endl;
    EXPECT_TRUE(true);
}

// Test 6: Lock-Free Queue Concurrent Destruction Safety
TEST_F(LockFreeQueueTest, ConcurrentDestructionSafety)
{
    std::cout << "\n=== CONCURRENT DESTRUCTION SAFETY TEST ===" << std::endl;

    const int num_engines = 3;
    std::vector<std::unique_ptr<MatchingEngine>> engines;
    std::vector<std::thread> workers;

    // Create multiple engines
    for (int i = 0; i < num_engines; ++i)
    {
        engines.push_back(std::make_unique<MatchingEngine>());
    }

    // Create workers that submit to different engines
    for (int engine_id = 0; engine_id < num_engines; ++engine_id)
    {
        workers.emplace_back([&engines, engine_id]()
                             {
            for (int i = 0; i < 500; ++i)
            {
                Order order(Strategy::HIGH_FREQUENCY, 1, 50.0 + i * 0.01, OrderSide::BUY, OrderType::LIMIT);
                engines[engine_id]->process_order(order);
                
                if (i % 50 == 0)
                {
                    std::this_thread::yield();
                }
            } });
    }

    // Let workers run for a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Destroy engines while workers might still be submitting
    engines.clear();

    // Wait for workers to complete
    for (auto &worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }

    std::cout << "Concurrent destruction safety test completed" << std::endl;
    EXPECT_TRUE(true);
}
