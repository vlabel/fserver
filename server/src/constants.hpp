#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

namespace fserver
{


static const int BUFFER_SIZE = 40960;
class config {
    /**
     * @brief server instance constructor 
     *
     * @param address        ip address or hostname 
     * @param port           tcp port
     * @param dir            directory for received files
     * @param net_pool_size  io_services poll size
     * @param storage_pool_size  io_services poll size
     */
    config() {  };

    public:

   static config & inst() {
      static config config;
      return config;
    }
    void print_confs(); 
    bool parse_confs(int argc, char* argv[]);

    std::string hostname = "localhost";
    std::string port = "9999";
    int net_threads = 1;
    int io_threads = 1;
    std::string directory  = "./";

     

};

}

#endif
