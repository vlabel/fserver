#include <boost/program_options.hpp>
#include "server.hpp"


namespace po =  boost::program_options;

int main(int argc, char* argv[])
{

    po::options_description desc("./fserver --host = localhost --port = 9999 --net_threads = 4 -- io_threads = 2 --directory = ./");
    desc.add_options()
    ("help", "produce help message")
    ("host", po::value<string>(), "ip or hostname")
    ("port", po::value<string>(), "tcp port")
    ("net_threads", po::value<int>(), "threads fot networking")
    ("io_threads", po::value<int>(), "threads for io(disk)")
    ("directory", po::value<string>(), "root directory for storage")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }


    string hostname {"localhost"};
    string port {"9999"};
    int net_threads {1};
    int io_threads = 1;
    string directory = "./";
    try {
        if (vm.count("host")) {
            hostname =  vm["host"].as<string>() ;
        }
        if (vm.count("port")) {
            port =  vm["port"].as<string>() ;
        }
        if (vm.count("net_threads")) {
            net_threads =  vm["net_threads"].as<int>() ;
        }
        if (vm.count("io_threads")) {
            io_threads =  vm["io_threads"].as<int>() ;
        }
        if (vm.count("directory")) {
            directory =  vm["directory"].as<string>() ;
        }
        fserver::server s(hostname, port, directory, net_threads, io_threads );
        std::cout << "starting fserver " << endl;
        std::cout << "  host:            " << hostname << endl;
        std::cout << "  port:            " << port << endl;
        std::cout << "  directory:       " << directory << endl;
        std::cout << "  network_threads: " << net_threads << endl;
        std::cout << "  io_threads:      " << io_threads << endl;

        s.run();
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
    }

    return 0;
}
