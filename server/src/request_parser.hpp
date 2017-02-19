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
    request_parser();

    void reset();

    template <typename InputIterator>
    boost::tribool parse(request& req,
                         InputIterator begin, InputIterator end)
    {
        while (begin < end) {
            if (state_ != file_read) {
//        cout << "Consume " << *begin;
                boost::tribool result = consume(req, *begin++);
                if (result || !result)
                    return result;
            } else if ((req.file_size != 0) and (req.bytes_received < req.file_size)) {
                req.data.reset(new std::vector<char>());
                std::copy(begin,end,back_inserter(*req.data));
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
        filename,
        size,
        file_read
    } state_;
};

} // namespace fserver

#endif
