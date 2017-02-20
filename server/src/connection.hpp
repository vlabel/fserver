#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/atomic.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "request_parser.hpp"
#include "storage_handler.hpp"
#include "constants.hpp"

namespace fserver
{

class connection
    : public boost::enable_shared_from_this<connection>,
      private boost::noncopyable
{
public:
    /**
     * @brief ctor from inconmint connection class 
     *
     * @param net_service  thread and boost::io_service for networking 
     * @param storage_service thread and boost::io_service for io 
     */
    connection(boost::asio::io_service& net_service,
                        boost::asio::io_service* storage_service);

    boost::asio::ip::tcp::socket& socket();

    void start();

    /**
     * @brief send reply to client - called by storage_handler
     *
     * @param reply::status_type
     */
    void send_reply(reply::status_type);
    std::atomic<bool> fail {false};

private:
    void handle_read(const boost::system::error_code& e,
                     std::size_t bytes_transferred);
    void handle_write(const boost::system::error_code& e);  // read & write callbacks

    boost::asio::ip::tcp::socket socket_;
    boost::array<char, BUFFER_SIZE> buffer_; // buffer for incoming data
    request request_;
    request_parser request_parser_;          // halper for parsing header
    reply reply_;
    storage_handler storage_h;               // io operations
    std::string directory_;
};

typedef boost::shared_ptr<connection> connection_ptr;

}

#endif
