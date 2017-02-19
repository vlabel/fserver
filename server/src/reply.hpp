#ifndef FSERVER_REPLY_HPP
#define FSERVER_REPLY_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>

namespace fserver
{

struct reply {
    enum status_type {
        ok = 1,
        exist = 2,
        create_error = 3,
        write_error= 4,
        error= 5,
    } status;

    std::vector<boost::asio::const_buffer> to_buffers();

    static reply stock_reply(status_type status);
};

} // namespace fserver

#endif // FSERVER_REPLY_HPP
