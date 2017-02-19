#include "reply.hpp"
#include <string>
#include <boost/lexical_cast.hpp>

namespace fserver {

namespace status_strings {

const std::string ok =
  "OK";
const std::string exist=
  "File Already Exist";
const std::string create_error=
  "Can't create File";
const std::string write_error=
  "Write ERROR";
const std::string error=
  "ERROR";

boost::asio::const_buffer to_buffer(reply::status_type status)
{
  switch (status)
  {
  case reply::ok:
    return boost::asio::buffer(ok);
  case reply::exist:
    return boost::asio::buffer(exist);
  case reply::create_error:
    return boost::asio::buffer(create_error);
  case reply::write_error:
    return boost::asio::buffer(write_error);
  case reply::error:
    return boost::asio::buffer(error);
  default:
    return boost::asio::buffer(error);
  }
}

} // namespace status_strings

std::vector<boost::asio::const_buffer> reply::to_buffers()
{
  std::vector<boost::asio::const_buffer> buffers;
  buffers.push_back(status_strings::to_buffer(status));
  return buffers;
}

reply reply::stock_reply(reply::status_type status)
{
  reply rep;
  rep.status = status;
  return rep;
}

} // namespace fserver
