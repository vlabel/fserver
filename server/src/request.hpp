
#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <memory>
#include "boost/array.hpp"

namespace fserver
{

struct request {
    std::string file_name;
    size_t file_size = 0;
    std::unique_ptr<std::vector<char> > data;
    size_t bytes_received = 0;
};

} // namespace fserver

#endif // REQUEST_HPP
