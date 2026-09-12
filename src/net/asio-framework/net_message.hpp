#ifndef NET_MESSAGE_HPP
#define NET_MESSAGE_HPP

#include "net_common.hpp"

namespace wkd
{
    namespace net
    {
        // Message Header is sent at the start of all messages. The template Allows us
        // to use "enum class" to ensusre that the messages are valid at compile time
        template <typename T>
        struct message_header
        {
            T id{};
            uint32_t size = 0;
        };

        // Message Body contains a header and a std::vector, containing raw bytes
		// of infomation. This way the message can be variable length, but the size
		// in the header must be updated.
        template <typename T>
        struct message
        {
            message_header<T> header{};
            std::vector<uint8_t> body;

            // returns the size of entire message packet in bytes
            size_t size() const
            {
                return body.size();
            }

            // Override for std::cout compatibility - produces friendly description of message
            friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
            {
                os << "ID: " << int(msg.header.id) << " Size: " << msg.header.size;
                return os;
            }

            // Convenience Operator overloads - These allow us to add and remove stuff from
			// the body vector as if it were a stack, so First in, Last Out. These are a
			// template in itself, because we dont know what data type the user is pushing or
			// popping, so lets allow them all. NOTE: It assumes the data type is fundamentally
			// Plain Old Data (POD). TLDR: Serialise & Deserialise into/from a vector

            // Pushes data into the message buffer
            template <typename DataType>
            friend message<T>& operator << (message<T>& msg, const DataType& data)
            {
                // Check that the type of data being pushed is trivially copyable
                static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

                // cache current size of vector, as this will be the point we insert the data
                size_t i = msg.body.size();

                // Resize the vector by the size of the data being pushed
                msg.body.resize(msg.body.size() + sizeof(DataType));
