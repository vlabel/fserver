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
    /**
     * @brief server instance constructor 
     *
     * @param address        ip address or hostname 
     * @param port           tcp port
     * @param dir            directory for received files
     * @param net_pool_size  io_services poll size
     * @param storage_pool_size  io_services poll size
     */
    explicit server(const std::string& address, const std::string& port,
                    const std::string& dir,
                    std::size_t net_pool_size,
                    std::size_t storage_pool_size);

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
    std::string directory_; 

};

} // namespace fserver

#endif // FSERVER_HPP
