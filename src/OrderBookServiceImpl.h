#pragma once

#include "orderbook_service.grpc.pb.h"
#include "MatchingEngine.h"
#include <grpc++/grpc++.h>
#include <memory>
#include <atomic>
#include <chrono>

class OrderBookServiceImpl final : public orderbook::OrderBookService::Service
{
public:
    OrderBookServiceImpl();
    ~OrderBookServiceImpl();

    // gRPC service method implementations
    grpc::Status SubmitOrder(grpc::ServerContext *context,
                             const orderbook::SubmitOrderRequest *request,
                             orderbook::SubmitOrderResponse *response) override;

    grpc::Status GetBestBid(grpc::ServerContext *context,
                            const orderbook::GetBestBidRequest *request,
                            orderbook::GetBestBidResponse *response) override;

    grpc::Status GetBestAsk(grpc::ServerContext *context,
                            const orderbook::GetBestAskRequest *request,
                            orderbook::GetBestAskResponse *response) override;

    grpc::Status GetOrdersAtPrice(grpc::ServerContext *context,
                                  const orderbook::GetOrdersAtPriceRequest *request,
                                  orderbook::GetOrdersAtPriceResponse *response) override;

    grpc::Status CancelOrder(grpc::ServerContext *context,
                             const orderbook::CancelOrderRequest *request,
                             orderbook::CancelOrderResponse *response) override;

    grpc::Status HealthCheck(grpc::ServerContext *context,
                             const orderbook::HealthCheckRequest *request,
                             orderbook::HealthCheckResponse *response) override;

    grpc::Status GetPerformanceStats(grpc::ServerContext *context,
                                     const orderbook::GetPerformanceStatsRequest *request,
                                     orderbook::GetPerformanceStatsResponse *response) override;

private:
    // Core order book engine
    std::unique_ptr<MatchingEngine> matching_engine_;

    // Service statistics
    std::atomic<uint64_t> total_orders_processed_;
    std::atomic<uint64_t> total_requests_received_;
    std::chrono::steady_clock::time_point service_start_time_;

    // Performance tracking
    std::atomic<double> current_orders_per_second_;
    std::atomic<double> peak_orders_per_second_;

    // Helper methods for conversion between protobuf and internal types
    Strategy convertStrategy(orderbook::Strategy proto_strategy);
    OrderSide convertOrderSide(orderbook::OrderSide proto_side);
    OrderType convertOrderType(orderbook::OrderType proto_type);

    orderbook::Strategy convertStrategy(Strategy internal_strategy);
    orderbook::OrderSide convertOrderSide(OrderSide internal_side);
    orderbook::OrderType convertOrderType(OrderType internal_type);
    orderbook::OrderStatus convertOrderStatus(OrderStatus internal_status);

    // Performance monitoring
    void updatePerformanceMetrics();
};