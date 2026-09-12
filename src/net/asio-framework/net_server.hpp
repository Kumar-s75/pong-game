#ifndef NET_SERVER_HPP
#define NET_SERVER_HPP

#include "net_common.hpp"
#include "net_thread_safe_queue.hpp"
#include "net_message.hpp"
#include "net_connection.hpp"

#include <cstdint>
#include <exception>
#include <memory>
#include <system_error>

namespace wkd
{
    namespace net
    {
        template <typename T>
        class server_interface
        {
        public:

            server_interface(uint16_t port)
                : m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
            {

            }

            virtual ~server_interface()
            {
                Stop();
            }

            bool Start()
            {
                try
                {
                    // some work for the asio context
                    WaitForClientConnection();

                    m_thrContext = std::thread([this]() { m_asioContext.run(); });
                }
                catch (std::exception& e)
                {
                    // Something prohibited the server from listening
                    std::cerr << "[SERVER] Exception: " << e.what() << std::endl;
                    return false;
                }

                std::cout << "[SERVER] Started!\n";
                return true;
            }

            void Stop()
            {
                // Request the context to close
                m_asioContext.stop();

                // tidy up the context thread
                if(m_thrContext.joinable()) m_thrContext.join();

                std::cout << "[SERVER] Stopped!\n";
            }

            // ASYNC - Instruct asio to wait for connection
            void WaitForClientConnection()
            {
                m_asioAcceptor.async_accept(
                    [this](std::error_code ec, asio::ip::tcp::socket socket)
                    {
                        if (!ec)
                        {
                            std::cout << "[SERVER] New Connection: " << socket.remote_endpoint() << std::endl;

                            std::shared_ptr<connection<T>> newconn =
                                std::make_shared<connection<T>>(connection<T>::owner::server,
                                    m_asioContext, std::move(socket), m_qMessageIn);

                            // Give the user a chance to deny connection
                            if(OnClientConnect(newconn))
                            {
                                // Connection allowed, add to the contianer of new connection
                                m_deqConnections.push_back(std::move(newconn));
                                // Assign ID to connections
                                m_deqConnections.back()->ConnectToClient(this, nIDCounter++);

                                std::cout << "[" << m_deqConnections.back()->GetID() << "] Connection Approved\n";
                            }
                            else
                            {
                                std::cout << "[-----] Connection Denied\n";
                            }
                        }
                        else
                        {
                            std::cerr <<"[SERVER] New Connection Error: " << ec.message() << std::endl;
                        }

                        // Wait for another connection..
                        WaitForClientConnection();
                    });
            }

            // Send a message to a specific client
            void MessageClient(std::shared_ptr<connection<T>> client, const message<T>& msg)
            {
                if(client && client->IsConnected())
