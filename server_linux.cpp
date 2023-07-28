#include <http_tcpServer_linux.h>

int main()
{
    using namespace http;
    TcpServer server = TcpServer("", 8088);
    server.startListen(); 
    return 0;
}