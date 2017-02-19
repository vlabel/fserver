#include "server.hpp"
#include <stdexcept>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include "io_service_pool.hpp"


namespace fserver {

io_service_pool::io_service_pool(std::size_t net_size,
                                                    std::size_t storage_size)
  : next_net_service_(0),
    next_storage_service_(0)
{
  if ((net_size == 0) or (storage_size == 0))
    throw std::runtime_error("service_pool size is 0");
    init_pool(net_services_, net_size);
    init_pool(storage_services_, storage_size);
}

void io_service_pool::init_pool(std::vector<io_service_ptr> &pool, size_t size) { for (std::size_t i = 0; i < size; ++i)
  {
    io_service_ptr io_service(new boost::asio::io_service);
    work_ptr work(new boost::asio::io_service::work(*io_service));
    pool.push_back(io_service);
    work_.push_back(work);
  }
}

void io_service_pool::run()
{
  std::vector<boost::shared_ptr<boost::thread> > threads;
  auto create = [&](io_service_ptr &srv) {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, srv)));
    threads.push_back(thread);
  };
  std::for_each(net_services_.begin(),net_services_.end(),create);
  std::for_each(storage_services_.begin(),storage_services_.end(),create);

  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
}

void io_service_pool::stop()
{
  for (std::size_t i = 0; i < net_services_.size(); ++i)
    net_services_[i]->stop();
  for (std::size_t i = 0; i < storage_services_.size(); ++i)
    storage_services_[i]->stop();
}

boost::asio::io_service& io_service_pool::get_service(service_type type)
{
  auto get = [&](std::vector<io_service_ptr> &pool, size_t &counter)-> boost::asio::io_service&
  {
      boost::asio::io_service& service = *pool[counter];
      ++counter;
      if (counter == pool.size())
        counter = 0;
      return service;
  };

  if (type == service_type::net_service) {
      return get(net_services_, next_net_service_);
  } else {
      return get(storage_services_,next_storage_service_);
  }
}

} // namespace fserver
