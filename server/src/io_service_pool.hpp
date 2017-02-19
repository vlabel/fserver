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
    net_service,
    storage_service,
};

class io_service_pool
    : private boost::noncopyable
{
public:
    explicit io_service_pool(std::size_t net_services,std::size_t storage_services);

    void run();

    void stop();

    boost::asio::io_service& get_service(service_type type = service_type::net_service);

private:

    void init_pool(std::vector<io_service_ptr> &pool, size_t size);

    std::vector<io_service_ptr> net_services_;
    std::vector<io_service_ptr> storage_services_;

    std::vector<work_ptr> work_;

    std::size_t next_net_service_;
    std::size_t next_storage_service_;
};

} // namespace fserver

#endif // FSERVICE_POOL_HPP
