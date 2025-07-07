#include "OrderBookServiceImpl.h"
#include <iostream>
#include <stdexcept>

OrderBookServiceImpl::OrderBookServiceImpl()
    : matching_engine_(std::make_unique<MatchingEngine>()),
      total_orders_processed_(0),
      total_requests_received_(0),
      service_start_time_(std::chrono::steady_clock::now()),
      current_orders_per_second_(0.0),
      peak_orders_per_second_(0.0)
{
    std::cout << "OrderBook gRPC Service initialized" << std::endl;
}

OrderBookServiceImpl::~OrderBookServiceImpl()
{
    std::cout << "OrderBook gRPC Service shutting down" << std::endl;
}

grpc::Status OrderBookServiceImpl::SubmitOrder(grpc::ServerContext *context,
                                               const orderbook::SubmitOrderRequest *request,
                                               orderbook::SubmitOrderResponse *response)
{
    total_requests_received_.fetch_add(1);

    try
    {
        // Convert protobuf types to internal types
        Strategy strategy = convertStrategy(request->strategy());
        OrderSide side = convertOrderSide(request->side());
        OrderType type = convertOrderType(request->type());

        // Create order
        Order order(strategy, request->quantity(), request->price(), side, type);

        // Submit to matching engine (lock-free!)
        matching_engine_->process_order(order);

        // Update statistics
        total_orders_processed_.fetch_add(1);
        updatePerformanceMetrics();

        // Set response
        response->set_success(true);
        response->set_message("Order submitted successfully");
        response->set_order_id(order.get_id());

        return grpc::Status::OK;
    }
    catch (const std::exception &e)
    {
        response->set_success(false);
        response->set_message(std::string("Error submitting order: ") + e.what());
        response->set_order_id(0);
        return grpc::Status::OK; // Return OK but with error in response
    }
}

grpc::Status OrderBookServiceImpl::GetBestBid(grpc::ServerContext *context,
                                              const orderbook::GetBestBidRequest *request,
                                              orderbook::GetBestBidResponse *response)
{
    total_requests_received_.fetch_add(1);

    try
    {
        // Access the OrderBook directly (we need to expose it in MatchingEngine)
        // For now, we'll return a placeholder - this needs MatchingEngine API extension
        response->set_success(false);
        response->set_message("GetBestBid not yet implemented - requires OrderBook API exposure");
        response->set_price(0.0);

        return grpc::Status::OK;
    }
    catch (const std::exception &e)
    {
        response->set_success(false);
        response->set_message(std::string("Error getting best bid: ") + e.what());
        response->set_price(0.0);
        return grpc::Status::OK;
    }
}

grpc::Status OrderBookServiceImpl::GetBestAsk(grpc::ServerContext *context,
                                              const orderbook::GetBestAskRequest *request,
                                              orderbook::GetBestAskResponse *response)
{
    total_requests_received_.fetch_add(1);

    try
    {
        // Similar to GetBestBid - needs OrderBook API exposure
        response->set_success(false);
        response->set_message("GetBestAsk not yet implemented - requires OrderBook API exposure");
        response->set_price(0.0);

        return grpc::Status::OK;
    }
    catch (const std::exception &e)
    {
        response->set_success(false);
        response->set_message(std::string("Error getting best ask: ") + e.what());
        response->set_price(0.0);
        return grpc::Status::OK;
    }
}

grpc::Status OrderBookServiceImpl::GetOrdersAtPrice(grpc::ServerContext *context,
                                                    const orderbook::GetOrdersAtPriceRequest *request,
                                                    orderbook::GetOrdersAtPriceResponse *response)
{
    total_requests_received_.fetch_add(1);

    try
    {
        response->set_success(false);
        response->set_message("GetOrdersAtPrice not yet implemented - requires OrderBook API exposure");

        return grpc::Status::OK;
    }
    catch (const std::exception &e)
    {
        response->set_success(false);
        response->set_message(std::string("Error getting orders at price: ") + e.what());
        return grpc::Status::OK;
    }
}

grpc::Status OrderBookServiceImpl::CancelOrder(grpc::ServerContext *context,
                                               const orderbook::CancelOrderRequest *request,
                                               orderbook::CancelOrderResponse *response)
{
    total_requests_received_.fetch_add(1);

    try
    {
        response->set_success(false);
        response->set_message("CancelOrder not yet implemented - requires OrderBook API exposure");

        return grpc::Status::OK;
    }
    catch (const std::exception &e)
    {
        response->set_success(false);
        response->set_message(std::string("Error cancelling order: ") + e.what());
        return grpc::Status::OK;
    }
}

grpc::Status OrderBookServiceImpl::HealthCheck(grpc::ServerContext *context,
                                               const orderbook::HealthCheckRequest *request,
                                               orderbook::HealthCheckResponse *response)
{
    total_requests_received_.fetch_add(1);

    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::steady_clock::now() - service_start_time_)
                      .count();

    response->set_healthy(true);
    response->set_status("Service is running");
    response->set_uptime_seconds(uptime);
    response->set_active_orders(0); // TODO: Implement active order tracking
    response->set_total_orders_processed(total_orders_processed_.load());

    return grpc::Status::OK;
}

grpc::Status OrderBookServiceImpl::GetPerformanceStats(grpc::ServerContext *context,
                                                       const orderbook::GetPerformanceStatsRequest *request,
                                                       orderbook::GetPerformanceStatsResponse *response)
{
    total_requests_received_.fetch_add(1);

    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::steady_clock::now() - service_start_time_)
                      .count();

    response->set_success(true);
    response->set_total_orders_processed(total_orders_processed_.load());
    response->set_orders_per_second_current(current_orders_per_second_.load());
    response->set_orders_per_second_peak(peak_orders_per_second_.load());
    response->set_queue_depth_current(0); // TODO: Implement queue depth monitoring
    response->set_queue_depth_max(1024);  // Known queue capacity
    response->set_uptime_seconds(uptime);

    return grpc::Status::OK;
}

// Conversion functions: Protobuf -> Internal
Strategy OrderBookServiceImpl::convertStrategy(orderbook::Strategy proto_strategy)
{
    switch (proto_strategy)
    {
    case orderbook::STRATEGY_QUANT_LONG_TERM:
        return Strategy::QUANT_LONG_TERM;
    case orderbook::STRATEGY_HIGH_FREQUENCY:
        return Strategy::HIGH_FREQUENCY;
    case orderbook::STRATEGY_HEDGE_FUND:
        return Strategy::HEDGE_FUND;
    case orderbook::STRATEGY_ALGORITHMIC_TRADING:
        return Strategy::ALGORITHMIC_TRADING;
    case orderbook::STRATEGY_INVESTMENT_BANK:
        return Strategy::INVESTMENT_BANK;
    case orderbook::STRATEGY_PENSION_FUND:
        return Strategy::PENSION_FUND;
    case orderbook::STRATEGY_INSURANCE_COMPANY:
        return Strategy::INSURANCE_COMPANY;
    case orderbook::STRATEGY_OTHER:
        return Strategy::OTHER;
    default:
        return Strategy::OTHER;
    }
}

OrderSide OrderBookServiceImpl::convertOrderSide(orderbook::OrderSide proto_side)
{
    switch (proto_side)
    {
    case orderbook::ORDER_SIDE_BUY:
        return OrderSide::BUY;
    case orderbook::ORDER_SIDE_SELL:
        return OrderSide::SELL;
    default:
        return OrderSide::BUY;
    }
}

OrderType OrderBookServiceImpl::convertOrderType(orderbook::OrderType proto_type)
{
    switch (proto_type)
    {
    case orderbook::ORDER_TYPE_MARKET:
        return OrderType::MARKET;
    case orderbook::ORDER_TYPE_LIMIT:
        return OrderType::LIMIT;
    default:
        return OrderType::LIMIT;
    }
}

// Conversion functions: Internal -> Protobuf
orderbook::Strategy OrderBookServiceImpl::convertStrategy(Strategy internal_strategy)
{
    switch (internal_strategy)
    {
    case Strategy::QUANT_LONG_TERM:
        return orderbook::STRATEGY_QUANT_LONG_TERM;
    case Strategy::HIGH_FREQUENCY:
        return orderbook::STRATEGY_HIGH_FREQUENCY;
    case Strategy::HEDGE_FUND:
        return orderbook::STRATEGY_HEDGE_FUND;
    case Strategy::ALGORITHMIC_TRADING:
        return orderbook::STRATEGY_ALGORITHMIC_TRADING;
    case Strategy::INVESTMENT_BANK:
        return orderbook::STRATEGY_INVESTMENT_BANK;
    case Strategy::PENSION_FUND:
        return orderbook::STRATEGY_PENSION_FUND;
    case Strategy::INSURANCE_COMPANY:
        return orderbook::STRATEGY_INSURANCE_COMPANY;
    case Strategy::OTHER:
        return orderbook::STRATEGY_OTHER;
    default:
        return orderbook::STRATEGY_OTHER;
    }
}

orderbook::OrderSide OrderBookServiceImpl::convertOrderSide(OrderSide internal_side)
{
    switch (internal_side)
    {
    case OrderSide::BUY:
        return orderbook::ORDER_SIDE_BUY;
    case OrderSide::SELL:
        return orderbook::ORDER_SIDE_SELL;
    default:
        return orderbook::ORDER_SIDE_BUY;
    }
}

orderbook::OrderType OrderBookServiceImpl::convertOrderType(OrderType internal_type)
{
    switch (internal_type)
    {
    case OrderType::MARKET:
        return orderbook::ORDER_TYPE_MARKET;
    case OrderType::LIMIT:
        return orderbook::ORDER_TYPE_LIMIT;
    default:
        return orderbook::ORDER_TYPE_LIMIT;
    }
}

orderbook::OrderStatus OrderBookServiceImpl::convertOrderStatus(OrderStatus internal_status)
{
    switch (internal_status)
    {
    case OrderStatus::PENDING:
        return orderbook::ORDER_STATUS_PENDING;
    case OrderStatus::FILLED:
        return orderbook::ORDER_STATUS_FILLED;
    case OrderStatus::CANCELLED:
        return orderbook::ORDER_STATUS_CANCELLED;
    case OrderStatus::REJECTED:
        return orderbook::ORDER_STATUS_REJECTED;
    default:
        return orderbook::ORDER_STATUS_PENDING;
    }
}

void OrderBookServiceImpl::updatePerformanceMetrics()
{
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration_cast<std::chrono::seconds>(now - service_start_time_).count();

    if (uptime > 0)
    {
        double current_rate = static_cast<double>(total_orders_processed_.load()) / uptime;
        current_orders_per_second_.store(current_rate);

        double peak = peak_orders_per_second_.load();
        if (current_rate > peak)
        {
            peak_orders_per_second_.store(current_rate);
        }
    }
}