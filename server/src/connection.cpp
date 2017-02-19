#include <vector>
#include <algorithm>
#include <boost/bind.hpp>

#include <iostream>
#include "connection.hpp"
#include "constants.hpp"


namespace fserver {
using namespace std;

connection::connection(boost::asio::io_service& net_srv,
                       boost::asio::io_service* storage_srv,
                       const std::string & dir)
  : socket_(net_srv),
    storage_h(storage_srv),
    directory_(dir)
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
  if (!e)
  {
    if (fail) return;
    boost::tribool result;
    result = request_parser_.parse(
        request_, buffer_.data(), buffer_.data() + bytes_transferred);

    if ((request_.file_size != 0) and (request_.data )) {
          std::shared_ptr<std::vector<char>> sh(request_.data.release());
          if (!storage_h.is_inited())  storage_h.init(request_.file_size,request_.file_name,directory_);
          storage_h.write(shared_from_this(),sh);
          if (request_.bytes_received == request_.file_size) {
              result = true;
          }
    }
    if (result) {
        return;
    } 
    else if (!result)
    {
        send_reply(reply::status_type::error);
    }
    else
    {
      async_read(socket_,boost::asio::buffer(buffer_,std::min(request_.file_size - request_.bytes_received,(size_t)BUFFER_SIZE) ),
          boost::bind(&connection::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
  } else {
    cout << "[connection] ERROR " << e.message() << endl;
    storage_h.fail = true;
  }


}

void connection::send_reply(reply::status_type type) {
     cout << "[connection] write reply " <<  endl;
      reply_.status = type;
      boost::asio::async_write(socket_, reply_.to_buffers(),
          boost::bind(&connection::handle_write, shared_from_this(),
            boost::asio::placeholders::error)); 
}

void connection::handle_write(const boost::system::error_code& e)
{
  if (!e)
  {
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    socket_.close();
  } else {
    cout << "[connection] ERROR " << e.message() << endl;
    storage_h.fail = true;
    socket_.close();
  } 
}

} // namespace fserver
