#include "server.hpp"
#include <boost/bind.hpp>

namespace fserver
{

server::server()
    : io_service_pool_(),
      signals_(io_service_pool_.get_service()),
      acceptor_(io_service_pool_.get_service()),
      new_connection_()
{
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
    signals_.add(SIGQUIT);
    signals_.async_wait(boost::bind(&server::handle_stop, this));

    boost::asio::ip::tcp::resolver resolver(acceptor_.get_io_service());
    boost::asio::ip::tcp::resolver::query query(config::inst().hostname,
           config::inst().port, boost::asio::ip::resolver_query_base::numeric_service);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    start_accept();
}

void server::run()
{
    try {
        io_service_pool_.run();
    } catch (std::exception& e) {

        std::cerr << "[server] ERROR" << e.what() << endl;
    }
}

void server::start_accept()
{
    new_connection_.reset(new connection(
                              io_service_pool_.get_service(),
                              &io_service_pool_.get_service(service_type::storage_service)
                              ));
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
        std::cerr << "[server] ERROR" << endl;
    io_service_pool_.stop();
}

} // namespace fserver
