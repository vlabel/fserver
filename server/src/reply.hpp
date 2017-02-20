#ifndef FSERVER_REPLY_HPP
#define FSERVER_REPLY_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>

namespace fserver
{

struct reply {
    enum status_type {
        ok ,
        exist ,
        create_error ,
        write_error ,
        error ,
    } status;

    std::vector<boost::asio::const_buffer> to_buffers();

    static reply stock_reply(status_type status);
};

} // namespace fserver

#endif // FSERVER_REPLY_HPP
