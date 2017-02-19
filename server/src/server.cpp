#include "server.hpp"
#include <boost/bind.hpp>

namespace fserver
{

server::server(const std::string& address, const std::string& port,
               const std::string& dir, std::size_t net_service_pool_size,
               size_t storage_pool_size )
    : io_service_pool_(net_service_pool_size, storage_pool_size),
      signals_(io_service_pool_.get_service()),
      acceptor_(io_service_pool_.get_service()),
      new_connection_(),
      directory_(dir)
{
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    signals_.add(SIGQUIT);
    signals_.async_wait(boost::bind(&server::handle_stop, this));

    boost::asio::ip::tcp::resolver resolver(acceptor_.get_io_service());
    boost::asio::ip::tcp::resolver::query query(address, port, boost::asio::ip::resolver_query_base::numeric_service);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    start_accept();
}

void server::run()
{
    io_service_pool_.run();
}

void server::start_accept()
{
    new_connection_.reset(new connection(
                              io_service_pool_.get_service(),
                              &io_service_pool_.get_service(service_type::storage_service),
                              directory_));
    acceptor_.async_accept(new_connection_->socket(),
                           boost::bind(&server::handle_accept, this,
                                       boost::asio::placeholders::error));
}

void server::handle_accept(const boost::system::error_code& e)
{
    if (!e) {
        new_connection_->start();
    }

    start_accept();
}

void server::handle_stop()
{
    io_service_pool_.stop();
}

} // namespace fserver
