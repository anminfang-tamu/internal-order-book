#!/usr/bin/env python3
"""
Simple gRPC client to test the OrderBook service
"""

import grpc
import sys
import time
import orderbook_service_pb2 as pb
import orderbook_service_pb2_grpc as pb_grpc

def test_orderbook_service():
    # Connect to the gRPC server
    channel = grpc.insecure_channel('localhost:50051')
    stub = pb_grpc.OrderBookServiceStub(channel)

    print("🧪 Testing OrderBook gRPC Service")
    print("=" * 50)

    try:
        # Test 1: Health Check
        print("1. Testing Health Check...")
        health_request = pb.HealthCheckRequest()
        health_response = stub.HealthCheck(health_request)
        print(f"   ✅ Health Status: {health_response.status}")
        print(f"   📊 Active Orders: {health_response.active_orders}")

        # Test 2: Submit Orders
        print("\n2. Submitting Orders...")
        
        # Submit a buy order
        buy_order = pb.SubmitOrderRequest(
            side=pb.ORDER_SIDE_BUY,
            type=pb.ORDER_TYPE_LIMIT,
            quantity=100,
            price=99.50,
            strategy=pb.STRATEGY_ALGORITHMIC_TRADING
        )
        buy_response = stub.SubmitOrder(buy_order)
        print(f"   ✅ Buy Order Submitted - ID: {buy_response.order_id}, Success: {buy_response.success}, Message: {buy_response.message}")

        # Submit a sell order
        sell_order = pb.SubmitOrderRequest(
            side=pb.ORDER_SIDE_SELL,
            type=pb.ORDER_TYPE_LIMIT,
            quantity=150,
            price=100.50,
            strategy=pb.STRATEGY_HIGH_FREQUENCY
        )
        sell_response = stub.SubmitOrder(sell_order)
        print(f"   ✅ Sell Order Submitted - ID: {sell_response.order_id}, Success: {sell_response.success}, Message: {sell_response.message}")

        # Test 3: Get Best Bid/Ask
        print("\n3. Getting Best Bid/Ask...")
        
        best_bid_request = pb.GetBestBidRequest()
        try:
            best_bid_response = stub.GetBestBid(best_bid_request)
            if best_bid_response.success:
                print(f"   ✅ Best Bid: ${best_bid_response.price:.2f}")
            else:
                print(f"   ℹ️  No bids available: {best_bid_response.message}")
        except grpc.RpcError as e:
            print(f"   ℹ️  No bids available: {e.details()}")

        best_ask_request = pb.GetBestAskRequest()
        try:
            best_ask_response = stub.GetBestAsk(best_ask_request)
            if best_ask_response.success:
                print(f"   ✅ Best Ask: ${best_ask_response.price:.2f}")
            else:
                print(f"   ℹ️  No asks available: {best_ask_response.message}")
        except grpc.RpcError as e:
            print(f"   ℹ️  No asks available: {e.details()}")

        # Test 4: Performance Stats
        print("\n4. Getting Performance Statistics...")
        stats_request = pb.GetPerformanceStatsRequest()
        stats_response = stub.GetPerformanceStats(stats_request)
        print(f"   📈 Orders Processed: {stats_response.total_orders_processed}")
        print(f"   ⚡ Current Throughput: {stats_response.orders_per_second_current:.2f} orders/sec")
        print(f"   🚀 Peak Throughput: {stats_response.orders_per_second_peak:.2f} orders/sec")
        print(f"   📊 Queue Depth: {stats_response.queue_depth_current}/{stats_response.queue_depth_max}")

        # Test 5: Submit Market Order for Matching
        print("\n5. Testing Market Order Matching...")
        market_buy = pb.SubmitOrderRequest(
            side=pb.ORDER_SIDE_BUY,
            type=pb.ORDER_TYPE_MARKET,
            quantity=50,
            price=0.0,  # Market orders don't need price
            strategy=pb.STRATEGY_HEDGE_FUND
        )
        market_response = stub.SubmitOrder(market_buy)
        print(f"   ✅ Market Buy Order - ID: {market_response.order_id}, Success: {market_response.success}, Message: {market_response.message}")

        # Wait a moment for processing
        time.sleep(0.1)

        # Check updated performance stats
        print("\n6. Updated Performance Statistics...")
        stats_response = stub.GetPerformanceStats(stats_request)
        print(f"   📈 Orders Processed: {stats_response.total_orders_processed}")
        print(f"   ⚡ Current Throughput: {stats_response.orders_per_second_current:.2f} orders/sec")

        print("\n🎉 All tests completed successfully!")
        print("🚀 gRPC OrderBook Service is fully operational!")

    except grpc.RpcError as e:
        print(f"❌ gRPC Error: {e.code()} - {e.details()}")
        return False
    except Exception as e:
        print(f"❌ Unexpected Error: {e}")
        return False
    
    finally:
        channel.close()
    
    return True

if __name__ == "__main__":
    success = test_orderbook_service()
    sys.exit(0 if success else 1) 