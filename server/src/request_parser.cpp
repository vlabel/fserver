#include "request_parser.hpp"
#include "request.hpp"

#include <iostream>

using namespace std;
namespace fserver {

request_parser::request_parser()
  : state_(filename)
{
}

void request_parser::reset()
{
  state_ = filename;
}

boost::tribool request_parser::consume(request& req, char input)
{ 
  switch (state_)
  {
  case filename:
    if (!is_char(input) ||  is_tspecial(input)) {
      return false;
    }
    else if (input  != '\n')  {
      req.file_name += std::string(&input);
      return boost::indeterminate;
    } else {
      state_ = size;
      return boost::indeterminate;
    }
  case size:
    if (is_digit(input)) {
      req.file_size = req.file_size * 10 + input - '0';
      return boost::indeterminate;
    }
    else if (input == '\n')
    {
      state_ = file_read;
      return boost::indeterminate;
    }
    else if (!is_char(input) || is_ctl(input) || is_tspecial(input))
    {
      return false;
    }
  default:
    return false;
  }
}

bool request_parser::is_char(int c)
{
  return c >= 0 && c <= 127;
}

bool request_parser::is_ctl(int c)
{
  return (c >= 0 && c <= 31) || (c == 127);
}

bool request_parser::is_tspecial(int c)
{
  switch (c)
  {
  case '(': case ')': case '<': case '>': case '@':
  case ',': case ';': case ':': case '\\': case '"':
  case '/': case '[': case ']': case '?': case '=':
  case '{': case '}': case ' ': case '\t':
    return true;
  default:
    return false;
  }
}

bool request_parser::is_digit(int c)
{
  return c >= '0' && c <= '9';
}

} 
