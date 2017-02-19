#ifndef FSERVER_HPP
#define FSERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <boost/noncopyable.hpp>
#include "io_service_pool.hpp"
#include "connection.hpp"

namespace fserver {

class server
  : private boost::noncopyable
{
public:
  explicit server(const std::string& address, const std::string& port,
                  const std::string& dir,
                  std::size_t net_pool_size,
                  std::size_t storage_pool_size);

  void run();

private:

  void start_accept();
  void handle_accept(const boost::system::error_code& e);
  void handle_stop();

  io_service_pool io_service_pool_;
  boost::asio::signal_set signals_;
  boost::asio::ip::tcp::acceptor acceptor_;
  connection_ptr new_connection_;
  std::string directory_;

};

} // namespace fserver

#endif // FSERVER_HPP
