#ifndef INCLUDED_HTTP_TCPSERVER_LINUX
#define INCLUDED_HTTP_TCPSERVER_LINUX

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>

namespace http 
{
    class TcpServer
    {
        public:
            TcpServer(const char* ip_address, int port);
            ~TcpServer();
            void startListen();

        private:
            const char* m_destination_ip_address;
            int m_main_port;
            int m_destination_port;
            int m_main_socket;
            int m_dest_socket;
            int m_client_socket;
            long m_incomingMessage;
            struct sockaddr_in m_main_socketAddress;
            unsigned int m_main_socketAddress_len;
            std::string m_serverMessage;

            int startServer();
            void closeServer();
            void acceptConnection();
            std::string buildResponse();
            void sendResponse();
    };

} 

#endif