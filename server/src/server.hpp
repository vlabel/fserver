#ifndef FSERVER_HPP
#define FSERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <boost/noncopyable.hpp>
#include "io_service_pool.hpp"
#include "connection.hpp"

namespace fserver
{

class server
    : private boost::noncopyable
{
public:
    server();

    /**
     * @brief run all threads
     */
    void run();

private:

    /**
     * @brief start accept incoming connections
     */
    void start_accept();

    void handle_accept(const boost::system::error_code& e);
    void handle_stop();

    io_service_pool io_service_pool_;  //poll of networking and io(disk) threads
    boost::asio::signal_set signals_;
    boost::asio::ip::tcp::acceptor acceptor_; // acceptor for connections
    connection_ptr new_connection_; // ptr for new connection (later connection manages itself)

};

} // namespace fserver

#endif // FSERVER_HPP
