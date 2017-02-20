#include "server.hpp"
#include "constants.hpp"

int main(int argc, char* argv[])
{
    namespace fs = fserver;
    fs::config::inst().parse_confs(argc, argv);
    fs::server s;
    fs::config::inst().print_confs();
    
    s.run();
    return 0;
}
