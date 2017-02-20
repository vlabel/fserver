#include "server.hpp"
#include "constants.hpp"

int main(int argc, char* argv[])
{
    namespace fs = fserver;
    try {
        fs::config::inst().parse_confs(argc, argv);
        fs::server s;
        fs::config::inst().print_confs();
        s.run();
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << endl;
    }
    return 0;
}
