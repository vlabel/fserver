#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <boost/logic/tribool.hpp>
#include <boost/array.hpp>
#include "request.hpp"
#include "constants.hpp"

#include <iostream>
using namespace std;


namespace fserver
{


class request_parser
{
public:
    /**
     * @brief ctor   parse message header or send file_read state
     */
    request_parser();

    void reset();

    template <typename InputIterator>
    boost::tribool parse(request& req,
                         InputIterator begin, InputIterator end)
    {
        while (begin < end) {
            if (state_ != file_read) {
                boost::tribool result = consume(req, *begin++);  //  parse header
                if (result || !result)
                    return result;
            } else if ((req.file_size != 0) and (req.bytes_received < req.file_size)) {
                req.data.reset(new std::vector<char>());    // create vector to store data
                std::copy(begin,end,back_inserter(*req.data)); // copy into it -> possible optimize here
                req.bytes_received += std::distance(begin,end);
                begin = end;
            }
        }
        boost::tribool result = boost::indeterminate;
        return result;
    }

private:
    boost::tribool consume(request& req, char input);

    static bool is_char(int c);
    static bool is_ctl(int c);
    static bool is_tspecial(int c);
    static bool is_digit(int c);

    /// The current state of the parser.
    enum state {
        filename,   // next symbol is filename
        size,
        file_read   //receive  data -> file body
    } state_;
};

} // namespace fserver

#endif
