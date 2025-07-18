# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
"""Client and server classes corresponding to protobuf-defined services."""
import grpc
import warnings

import orderbook_service_pb2 as orderbook__service__pb2

GRPC_GENERATED_VERSION = '1.73.1'
GRPC_VERSION = grpc.__version__
_version_not_supported = False

try:
    from grpc._utilities import first_version_is_lower
    _version_not_supported = first_version_is_lower(GRPC_VERSION, GRPC_GENERATED_VERSION)
except ImportError:
    _version_not_supported = True

if _version_not_supported:
    raise RuntimeError(
        f'The grpc package installed is at version {GRPC_VERSION},'
        + f' but the generated code in orderbook_service_pb2_grpc.py depends on'
        + f' grpcio>={GRPC_GENERATED_VERSION}.'
        + f' Please upgrade your grpc module to grpcio>={GRPC_GENERATED_VERSION}'
        + f' or downgrade your generated code using grpcio-tools<={GRPC_VERSION}.'
    )


class OrderBookServiceStub(object):
    """OrderBook gRPC Service Definition
    """

    def __init__(self, channel):
        """Constructor.

        Args:
            channel: A grpc.Channel.
        """
        self.SubmitOrder = channel.unary_unary(
                '/orderbook.OrderBookService/SubmitOrder',
                request_serializer=orderbook__service__pb2.SubmitOrderRequest.SerializeToString,
                response_deserializer=orderbook__service__pb2.SubmitOrderResponse.FromString,
                _registered_method=True)
        self.GetBestBid = channel.unary_unary(
                '/orderbook.OrderBookService/GetBestBid',
                request_serializer=orderbook__service__pb2.GetBestBidRequest.SerializeToString,
                response_deserializer=orderbook__service__pb2.GetBestBidResponse.FromString,
                _registered_method=True)
        self.GetBestAsk = channel.unary_unary(
                '/orderbook.OrderBookService/GetBestAsk',
                request_serializer=orderbook__service__pb2.GetBestAskRequest.SerializeToString,
                response_deserializer=orderbook__service__pb2.GetBestAskResponse.FromString,
                _registered_method=True)
        self.GetOrdersAtPrice = channel.unary_unary(
                '/orderbook.OrderBookService/GetOrdersAtPrice',
                request_serializer=orderbook__service__pb2.GetOrdersAtPriceRequest.SerializeToString,
                response_deserializer=orderbook__service__pb2.GetOrdersAtPriceResponse.FromString,
                _registered_method=True)
        self.CancelOrder = channel.unary_unary(
                '/orderbook.OrderBookService/CancelOrder',
                request_serializer=orderbook__service__pb2.CancelOrderRequest.SerializeToString,
                response_deserializer=orderbook__service__pb2.CancelOrderResponse.FromString,
                _registered_method=True)
        self.HealthCheck = channel.unary_unary(
                '/orderbook.OrderBookService/HealthCheck',
                request_serializer=orderbook__service__pb2.HealthCheckRequest.SerializeToString,
                response_deserializer=orderbook__service__pb2.HealthCheckResponse.FromString,
                _registered_method=True)
        self.GetPerformanceStats = channel.unary_unary(
                '/orderbook.OrderBookService/GetPerformanceStats',
                request_serializer=orderbook__service__pb2.GetPerformanceStatsRequest.SerializeToString,
                response_deserializer=orderbook__service__pb2.GetPerformanceStatsResponse.FromString,
                _registered_method=True)


class OrderBookServiceServicer(object):
    """OrderBook gRPC Service Definition
    """

    def SubmitOrder(self, request, context):
        """Submit a new order to the order book
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def GetBestBid(self, request, context):
        """Get the best bid price
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def GetBestAsk(self, request, context):
        """Get the best ask price
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def GetOrdersAtPrice(self, request, context):
        """Get all orders at a specific price level
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def CancelOrder(self, request, context):
        """Cancel an order by ID
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def HealthCheck(self, request, context):
        """Health check endpoint
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def GetPerformanceStats(self, request, context):
        """Get performance statistics
        """
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def add_OrderBookServiceServicer_to_server(servicer, server):
    rpc_method_handlers = {
            'SubmitOrder': grpc.unary_unary_rpc_method_handler(
                    servicer.SubmitOrder,
                    request_deserializer=orderbook__service__pb2.SubmitOrderRequest.FromString,
                    response_serializer=orderbook__service__pb2.SubmitOrderResponse.SerializeToString,
            ),
            'GetBestBid': grpc.unary_unary_rpc_method_handler(
                    servicer.GetBestBid,
                    request_deserializer=orderbook__service__pb2.GetBestBidRequest.FromString,
                    response_serializer=orderbook__service__pb2.GetBestBidResponse.SerializeToString,
            ),
            'GetBestAsk': grpc.unary_unary_rpc_method_handler(
                    servicer.GetBestAsk,
                    request_deserializer=orderbook__service__pb2.GetBestAskRequest.FromString,
                    response_serializer=orderbook__service__pb2.GetBestAskResponse.SerializeToString,
            ),
            'GetOrdersAtPrice': grpc.unary_unary_rpc_method_handler(
                    servicer.GetOrdersAtPrice,
                    request_deserializer=orderbook__service__pb2.GetOrdersAtPriceRequest.FromString,
                    response_serializer=orderbook__service__pb2.GetOrdersAtPriceResponse.SerializeToString,
            ),
            'CancelOrder': grpc.unary_unary_rpc_method_handler(
                    servicer.CancelOrder,
                    request_deserializer=orderbook__service__pb2.CancelOrderRequest.FromString,
                    response_serializer=orderbook__service__pb2.CancelOrderResponse.SerializeToString,
            ),
            'HealthCheck': grpc.unary_unary_rpc_method_handler(
                    servicer.HealthCheck,
                    request_deserializer=orderbook__service__pb2.HealthCheckRequest.FromString,
                    response_serializer=orderbook__service__pb2.HealthCheckResponse.SerializeToString,
            ),
            'GetPerformanceStats': grpc.unary_unary_rpc_method_handler(
                    servicer.GetPerformanceStats,
                    request_deserializer=orderbook__service__pb2.GetPerformanceStatsRequest.FromString,
                    response_serializer=orderbook__service__pb2.GetPerformanceStatsResponse.SerializeToString,
            ),
    }
    generic_handler = grpc.method_handlers_generic_handler(
            'orderbook.OrderBookService', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))
    server.add_registered_method_handlers('orderbook.OrderBookService', rpc_method_handlers)


 # This class is part of an EXPERIMENTAL API.
class OrderBookService(object):
    """OrderBook gRPC Service Definition
    """

    @staticmethod
    def SubmitOrder(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/orderbook.OrderBookService/SubmitOrder',
            orderbook__service__pb2.SubmitOrderRequest.SerializeToString,
            orderbook__service__pb2.SubmitOrderResponse.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)

    @staticmethod
    def GetBestBid(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/orderbook.OrderBookService/GetBestBid',
            orderbook__service__pb2.GetBestBidRequest.SerializeToString,
            orderbook__service__pb2.GetBestBidResponse.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)

    @staticmethod
    def GetBestAsk(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/orderbook.OrderBookService/GetBestAsk',
            orderbook__service__pb2.GetBestAskRequest.SerializeToString,
            orderbook__service__pb2.GetBestAskResponse.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)

    @staticmethod
    def GetOrdersAtPrice(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/orderbook.OrderBookService/GetOrdersAtPrice',
            orderbook__service__pb2.GetOrdersAtPriceRequest.SerializeToString,
            orderbook__service__pb2.GetOrdersAtPriceResponse.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)

    @staticmethod
    def CancelOrder(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/orderbook.OrderBookService/CancelOrder',
            orderbook__service__pb2.CancelOrderRequest.SerializeToString,
            orderbook__service__pb2.CancelOrderResponse.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)

    @staticmethod
    def HealthCheck(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/orderbook.OrderBookService/HealthCheck',
            orderbook__service__pb2.HealthCheckRequest.SerializeToString,
            orderbook__service__pb2.HealthCheckResponse.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)

    @staticmethod
    def GetPerformanceStats(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(
            request,
            target,
            '/orderbook.OrderBookService/GetPerformanceStats',
            orderbook__service__pb2.GetPerformanceStatsRequest.SerializeToString,
            orderbook__service__pb2.GetPerformanceStatsResponse.FromString,
            options,
            channel_credentials,
            insecure,
            call_credentials,
            compression,
            wait_for_ready,
            timeout,
            metadata,
            _registered_method=True)
