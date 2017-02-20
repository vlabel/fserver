#include "constants.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace fserver {

namespace po =  boost::program_options;

    void config::print_confs() {
        std::cout << "starting fserver " << std::endl;
        std::cout << "  host:            " << hostname << std::endl;
        std::cout << "  port:            " << port << std::endl;
        std::cout << "  directory:       " << directory << std::endl;
        std::cout << "  network_threads: " << net_threads << std::endl;
        std::cout << "  io_threads:      " << io_threads << std::endl;
    }
   


    void config::parse_confs(int argc, char* argv[])
    {
        po::options_description desc("./fserver --host = localhost --port = 9999 --net_threads = 4 -- io_threads = 2 --directory = ./");
        desc.add_options()
        ("help", "produce help message")
        ("host", po::value<std::string>(), "ip or hostname")
        ("port", po::value<std::string>(), "tcp port")
        ("net_threads", po::value<int>(), "threads fot networking")
        ("io_threads", po::value<int>(), "threads for io(disk)")
        ("directory", po::value<std::string>(), "root directory for storage")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return ;    
        }


        if (vm.count("host")) {
            config::hostname =  vm["host"].as<std::string>() ;
        }
        if (vm.count("port")) {
            config::port =  vm["port"].as<std::string>() ;
        }
        if (vm.count("net_threads")) {
            config::net_threads =  vm["net_threads"].as<int>() ;
        }
        if (vm.count("io_threads")) {
            config::io_threads =  vm["io_threads"].as<int>() ;
        }
        if (vm.count("directory")) {
            config::directory =  vm["directory"].as<std::string>() ;
        }
        if (!boost::filesystem::exists(config::directory)) {
                throw std::runtime_error("no such directory");
        }
    }


}
