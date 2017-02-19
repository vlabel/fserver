#ifndef STORAGEHANDLER_HPP
#define STORAGEHANDLER_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "request.hpp"
#include <fstream>
#include <atomic>

namespace fserver {
class connection;
typedef boost::shared_ptr<connection> connection_ptr;
typedef std::unique_ptr<std::vector<char>> data_array_ptr;
typedef std::shared_ptr<std::vector<char>> data_array_sh_ptr;

class storage_handler 
  : private boost::noncopyable
{
public:
  explicit storage_handler(boost::asio::io_service* io_service);
  void write(connection_ptr connection, data_array_sh_ptr );
  void handle_write(connection_ptr connection, data_array_sh_ptr );
  bool is_inited() { return inited_; }
  void init(size_t size, const std::string& filename, 
          const std::string& dir) {
    file_size_ = size;
    filename_ = dir+"/"+filename;
    inited_ = true;
  }
  std::atomic<bool> fail{false};
private:
  bool inited_ = false;
  size_t bytes_written_ = 0;
  size_t file_size_ = 0;
  std::string filename_ = "";
  boost::asio::io_service *io_service;
  std::ofstream o_stream_; 
  bool open_or_create();
  void on_error();
};

} 

#endif 
