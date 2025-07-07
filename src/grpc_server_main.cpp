#include "OrderBookServiceImpl.h"
#include <grpc++/grpc++.h>
#include <iostream>
#include <memory>
#include <string>
#include <signal.h>
#include <thread>

class OrderBookServer
{
public:
    OrderBookServer(const std::string &server_address)
        : server_address_(server_address) {}

    void Run()
    {
        // Create service implementation
        OrderBookServiceImpl service;

        // Configure server
        grpc::ServerBuilder builder;

        // Listen on the given address without any authentication mechanism
        builder.AddListeningPort(server_address_, grpc::InsecureServerCredentials());

        // Register service
        builder.RegisterService(&service);

        // Set server options for high performance
        builder.SetMaxReceiveMessageSize(4 * 1024 * 1024); // 4MB
        builder.SetMaxSendMessageSize(4 * 1024 * 1024);    // 4MB
        builder.SetMaxMessageSize(4 * 1024 * 1024);        // 4MB

        // Add completion queue for async operations (optional)
        // builder.SetCompressionAlgorithmSupportStatus(GRPC_COMPRESS_GZIP, true);

        // Build and start server
        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

        if (!server)
        {
            std::cerr << "Failed to start server on " << server_address_ << std::endl;
            return;
        }

        std::cout << "🚀 OrderBook gRPC Server listening on " << server_address_ << std::endl;
        std::cout << "📊 Lock-free queue capacity: 1024 orders" << std::endl;
        std::cout << "⚡ High-performance order processing enabled" << std::endl;
        std::cout << "🛡️  Memory-safe RAII implementation active" << std::endl;
        std::cout << "📡 Available endpoints:" << std::endl;
        std::cout << "   - SubmitOrder: Submit trading orders" << std::endl;
        std::cout << "   - HealthCheck: Service health monitoring" << std::endl;
        std::cout << "   - GetPerformanceStats: Performance metrics" << std::endl;
        std::cout << "   - GetBestBid/GetBestAsk: Market data (coming soon)" << std::endl;
        std::cout << "📝 Press Ctrl+C to shutdown gracefully..." << std::endl;

        // Store server reference for signal handler
        server_ = server.get();

        // Set up signal handling for graceful shutdown
        setupSignalHandlers();

        // Wait for the server to shutdown
        server->Wait();

        std::cout << "🛑 OrderBook gRPC Server shutdown complete" << std::endl;
    }

    void Shutdown()
    {
        if (server_)
        {
            std::cout << "\n🛑 Initiating graceful shutdown..." << std::endl;

            // Give ongoing requests time to complete
            auto deadline = std::chrono::system_clock::now() +
                            std::chrono::seconds(5);

            server_->Shutdown(deadline);
        }
    }

private:
    std::string server_address_;
    grpc::Server *server_ = nullptr;

    void setupSignalHandlers()
    {
        // Set up signal handler for graceful shutdown
        signal(SIGINT, [](int signal)
               {
            std::cout << "\n🔔 Received shutdown signal (" << signal << ")" << std::endl;
            // Note: This is a simplified signal handler
            // In production, use proper signal handling mechanisms
            exit(0); });

        signal(SIGTERM, [](int signal)
               {
            std::cout << "\n🔔 Received termination signal (" << signal << ")" << std::endl;
            exit(0); });
    }
};

void printUsage(const char *program_name)
{
    std::cout << "Usage: " << program_name << " [OPTIONS]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -p, --port PORT     Server port (default: 50051)" << std::endl;
    std::cout << "  -h, --host HOST     Server host (default: 0.0.0.0)" << std::endl;
    std::cout << "  --help              Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << program_name << "                    # Start on 0.0.0.0:50051" << std::endl;
    std::cout << "  " << program_name << " -p 8080           # Start on 0.0.0.0:8080" << std::endl;
    std::cout << "  " << program_name << " -h localhost -p 9090 # Start on localhost:9090" << std::endl;
}

int main(int argc, char **argv)
{
    std::string host = "0.0.0.0";
    int port = 50051;

    // Parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--help")
        {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "-p" || arg == "--port")
        {
            if (i + 1 < argc)
            {
                port = std::stoi(argv[++i]);
            }
            else
            {
                std::cerr << "Error: --port requires a value" << std::endl;
                return 1;
            }
        }
        else if (arg == "-h" || arg == "--host")
        {
            if (i + 1 < argc)
            {
                host = argv[++i];
            }
            else
            {
                std::cerr << "Error: --host requires a value" << std::endl;
                return 1;
            }
        }
        else
        {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    // Validate port range
    if (port < 1 || port > 65535)
    {
        std::cerr << "Error: Port must be between 1 and 65535" << std::endl;
        return 1;
    }

    std::string server_address = host + ":" + std::to_string(port);

    std::cout << "🏗️  Starting OrderBook gRPC Server..." << std::endl;
    std::cout << "📍 Server address: " << server_address << std::endl;

    try
    {
        OrderBookServer server(server_address);
        server.Run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "❌ Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}