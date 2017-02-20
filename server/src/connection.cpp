#include <vector>
#include <algorithm>
#include <boost/bind.hpp>

#include <iostream>
#include "connection.hpp"
#include "constants.hpp"


namespace fserver
{
using namespace std;

connection::connection(boost::asio::io_service& net_srv,
                       boost::asio::io_service* storage_srv
                       )
    : socket_(net_srv),
      storage_h(storage_srv),
      directory_(config::inst().directory)
{
}

boost::asio::ip::tcp::socket& connection::socket()
{
    return socket_;
}

void connection::start()
{
    socket_.async_read_some(boost::asio::buffer(buffer_),
                            boost::bind(&connection::handle_read, shared_from_this(),
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
}

void connection::handle_read(const boost::system::error_code& e,
                             std::size_t bytes_transferred)
{
    if (!e) {
        cout << "[connection] handle_read" <<  endl;
        if (fail) return;          // check atomic fail flag, to skin async opetation in case of error
        boost::tribool result;     // true -> parsed,  false - error,  undeterm - need more data
        result = request_parser_.parse(
                     request_, buffer_.data(), buffer_.data() + bytes_transferred);

        if ((request_.file_size != 0) and (request_.data )) {
            std::shared_ptr<std::vector<char>> sh(request_.data.release());   // shared from uniq and later send in to io_thread
            if (!storage_h.is_inited())  storage_h.init(request_.file_size,request_.file_name,directory_);
            storage_h.write(shared_from_this(),sh);
            if (request_.bytes_received == request_.file_size) {
            cout << "[connection] handle_read2" <<  endl;
                result = true; // while file received
            }
        }
        if (result) {
            return;
        } else if (!result) {
            send_reply(reply::status_type::error);  // header parsing error
        } else {
            async_read(socket_,boost::asio::buffer(buffer_,
                       std::min(request_.file_size - request_.bytes_received,(size_t)BUFFER_SIZE) ),
                       boost::bind(&connection::handle_read, shared_from_this(),
                                   boost::asio::placeholders::error,
                                   boost::asio::placeholders::bytes_transferred)); // read another pack of data
        }
    } else {
        cout << "[connection] ERROR " << e.message() << endl;
        storage_h.fail = true;
    }


}

void connection::send_reply(reply::status_type type)
{
    if (fail) return;          // check atomic fail flag, to skin async opetation in case of error
    cout << "[connection] write reply " <<  endl;
    reply_.status = type;
    boost::asio::async_write(socket_, reply_.to_buffers(),
                             boost::bind(&connection::handle_write, shared_from_this(),
                                         boost::asio::placeholders::error));
}

/**
 * @brief write only in case of error or all OK
 *
 * @param e
 */
void connection::handle_write(const boost::system::error_code& e)
{
    if (!e) {
        boost::system::error_code ignored_ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    } else {
        cout << "[connection] ERROR " << e.message() << endl;
        storage_h.fail = true;
    }
        socket_.close();
}

} // namespace fserver
