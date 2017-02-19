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
    explicit connection(boost::asio::io_service& net_service,
                        boost::asio::io_service* storage_service,
                        const std::string & directory );

    boost::asio::ip::tcp::socket& socket();

    void start();
    void send_reply(reply::status_type);
    std::atomic<bool> fail {false};

private:
    void handle_read(const boost::system::error_code& e,
                     std::size_t bytes_transferred);
    void handle_write(const boost::system::error_code& e);

    boost::asio::ip::tcp::socket socket_;
    boost::array<char, BUFFER_SIZE> buffer_;
    request request_;
    request_parser request_parser_;
    reply reply_;
    storage_handler storage_h;
    std::string directory_;
};

typedef boost::shared_ptr<connection> connection_ptr;

}

#endif
