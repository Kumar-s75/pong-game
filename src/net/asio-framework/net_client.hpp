#ifndef NET_CLIENT_HPP
#define NET_CLIENT_HPP

#include "net_common.hpp"

namespace wkd
{
    namespace net
    {
        template <typename T>
        class client_interface
        {
        public:
            // Constructor
            client_interface()
            {
                // Initialize the socket with the io context, so it can do stuff
            }

            // Destructor
            virtual ~client_interface()
            {
                // If the client is destroyed, always try and disconnect from server
                Disconnect();
            }

        public:
            // Connect to server with hostname/ip-address and port
            bool Connect(const std::string& host, const uint16_t port)
            {
                try
                {
                    // Resolve hostname/ip-address into tangible physical address
                    asio::ip::tcp::resolver resolver(m_context);
                    asio::ip::tcp::resolver::results_type m_endpoints = resolver.resolve(host, std::to_string(port));

                    // Create Connection
                    m_connection = std::make_unique<connection<T>>(
                        connection<T>::owner::client,
                        m_context,
                        asio::ip::tcp::socket(m_context),
                        m_qMessageIn
                    );

                    // Tell the connection object to connect to server
                    m_connection->ConnectToServer(m_endpoints);

                    // Start Context thread
                    thrContext = std::thread([this]() { m_context.run(); });
                }
                catch (std::exception& e) {
                    std::cerr << "Client Exception: " << e.what() << "\n";
                    return false;
                }
                return true;
            }

            // Disconnect from the server
            void Disconnect()
