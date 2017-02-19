#include "storage_handler.hpp"
#include <vector>
#include <boost/bind.hpp>
#include "reply.hpp"

#include <iostream>
#include <utility>
#include "connection.hpp"
using namespace std;


namespace fserver
{

storage_handler::storage_handler(boost::asio::io_service* srv)
    : io_service(srv)
{
}


bool storage_handler::open_or_create()
{
    if (o_stream_.is_open()) return true;
    o_stream_.open(filename_.c_str(),ios_base::out | ios_base::in) ;
    if (o_stream_.is_open()) {
        cout << "[storage_handler] "  << "ERROR Filename busy" << endl;
        inited_ = false;
        return false;
    }

    o_stream_.open(filename_.c_str(),ios_base::out ) ;
    if (o_stream_.good() and o_stream_.is_open()) {
        cout << "[storage_handler] "  << "File: " << filename_ << " created and opened" << endl;
        return true;
    }
}

void storage_handler::handle_write(connection_ptr connection, data_array_sh_ptr data )
{
    if (fail) return;
    if (! is_inited()) {
        connection->send_reply(reply::status_type::error);
        connection->fail = true;
        //errr
        return;
    }

    if (open_or_create()) {
        o_stream_.write(data->data(),data->size());
        if (o_stream_.bad()) {
            cout << "[storage_handler] write ERROR" << endl;
            inited_ = false;
            connection->fail = true;
            connection->send_reply(reply::status_type::error);
            return;
        }
        bytes_written_+=data->size();
        if (bytes_written_ == file_size_) {
            cout << "[storage_hangler] " << "Whole File written. size: "
                 << file_size_ << " File: " << filename_ << endl  ;
            o_stream_.close();
            connection->send_reply(reply::status_type::ok);
        }
    } else {
        cout << "[storage_handler] send status file_exist" << endl;
        connection->fail = true;
        connection->send_reply(reply::status_type::exist);
    }

}

void storage_handler::write(connection_ptr connection, data_array_sh_ptr data )
{
    if (fail) return;
    if (! is_inited()) {
        return;
    }
    io_service->post(std::bind(&storage_handler::handle_write,this,connection,data));
}

} // namespace fserver
