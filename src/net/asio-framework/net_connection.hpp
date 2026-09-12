#ifndef NET_CONNECTION_HPP
#define NET_CONNECTION_HPP

#include "net_common.hpp"
#include "net_thread_safe_queue.hpp"
#include "net_message.hpp"

namespace wkd
{
    namespace net
    {
        // Forward declare
        template<typename T>
        class server_interface;

        template <typename T>
        class connection : public std::enable_shared_from_this<connection<T>>
        {
        public:

            // Owner Type
            enum class owner
            {
                server,
                client
            };

            // Constructor: Specify Owner, connect to context, transfer the socket
			//				Provide reference to incoming message queue
            connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, thread_safe_queue<owned_message<T>>& qIn)
                : m_asioContext(asioContext), m_socket(std::move(socket)), m_qMessageIn(qIn)
            {
                m_nOwnerType = parent;

                // Construct validation check data
                if(m_nOwnerType == owner::server)
                {
                    // Server -> Client Connection, constructing random data
                    m_nHandshakeOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());

                    // Precompute the scrambled data for checking
                    m_nHandshakeCheck = Scramble(m_nHandshakeOut);
                }
                else
                {
                    // Client -> Server Connection
                    m_nHandshakeIn = 0;
                    m_nHandshakeOut = 0;
                }
            }

            // Destructor
            virtual ~connection() {}

            // returns the ID of the connection
            uint32_t GetID() const
            {
                return m_id;
            }

        public:

            // Connect to client (Only Applicable for Servers)
            void ConnectToClient(wkd::net::server_interface<T>* server, uint32_t uid = 0)
            {
                if(m_nOwnerType == owner::server)
                {
                    if(m_socket.is_open())
                    {
                        m_id = uid;
                        // Client attempts to connect to server, Validate the connection
                        WriteValidation();

                        // Issue an asynchronus task and wait for the client to respond to the validation
                        ReadValidation(server);
                    }
                }
            }

            // Connect to server
            void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
            {
                // Only clients can connect to servers
                if(m_nOwnerType == owner::client)
                {
                    // Request asio attempts to connect to an endpoint
                    asio::async_connect(m_socket, endpoints,
                    [this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
                    {
                        if(!ec)
                        {
                            // Verify the authenticity of the client
                            ReadValidation();
                        }
                    });
                }
            }

            void Disconnect()
            {
                if(IsConnected())
                {
                    asio::post(m_asioContext, [this]() { m_socket.close(); });
                }
            }

            // retuns true if connected
            bool IsConnected() const
            {
                return m_socket.is_open();
            }

            // ASYNC - Send a message, connections are one-to-one so no need to specifiy
			// the target, for a client, the target is the server and vice versa
            void Send(const message<T>& msg)
            {
                asio::post(m_asioContext,
                [this, msg]()
                {
                    bool bWritingMessage = !m_qMessageOut.empty();
                    m_qMessageOut.push_back(msg);
                    if(!bWritingMessage)
                    {
                        WriteHeader();
                    }
                });
            }

        private:
            // ASYNC - Prime context ready to read a message header
            void ReadHeader()
            {
                asio::async_read(m_socket, asio::buffer(&m_msgTemporaryIn.header, sizeof(message_header<T>)),
                [this](std::error_code ec, std::size_t length)
                {
                    if (!ec)
                    {
                        if(m_msgTemporaryIn.header.size > 0)
                        {
                            m_msgTemporaryIn.body.resize(m_msgTemporaryIn.header.size);
                            ReadBody();
                        }
                        else
                        {
                            AddToIncomingMessageQueue();
                        }
                    }
                    else
                    {
                        std::cout << "[" << m_id << "] Read Header Failed\n";
                        m_socket.close();
                    }
                });
            }

            // ASYNC - Prime context ready to read a message body
            void ReadBody()
            {
                asio::async_read(m_socket, asio::buffer(m_msgTemporaryIn.body.data(), m_msgTemporaryIn.body.size()),
                [this](std::error_code ec, std::size_t length)
                {
                    if(!ec)
                    {
                        AddToIncomingMessageQueue();
                    }
                    else
                    {
                        std::cout << "[" << m_id << "] Read Body Fail." << std::endl;
                        m_socket.close();
                    }
                });
            }
