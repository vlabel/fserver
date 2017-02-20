#ifndef FSERVICE_POOL_HPP
#define FSERVICE_POOL_HPP

#include <boost/asio.hpp>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace fserver
{
typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

enum service_type {
    net_service,     // separeted thread and boost::io for networking
    storage_service, // separeted thread and boost::io for disk io
};

class io_service_pool
    : private boost::noncopyable
{
public:
    io_service_pool();

    //thread are really created here
    void run();

    void stop();

    /**
     * @brief get io_service -> scaled by round robin
     *
     * @param type  io or networking
     *
     * @return reference to io_service
     */
    boost::asio::io_service& get_service(service_type type = service_type::net_service);

private:

    /**
     * @brief assing io_service::work
     *
     * @param pool - ref to pool
     * @param size
     */
    void init_pool(std::vector<io_service_ptr> &pool, size_t size);

    std::vector<io_service_ptr> net_services_;
    std::vector<io_service_ptr> storage_services_;

    std::vector<work_ptr> work_;

    std::size_t next_net_service_;  // for round robin 
    std::size_t next_storage_service_;
};

} // namespace fserver

#endif // FSERVICE_POOL_HPP
