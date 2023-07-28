#include <http_tcpServer_linux.h>

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <netinet/in.h>

namespace
{
    const int BUFFER_SIZE = 30720;

    void log(const std::string &message)
    {
        std::cout << message << std::endl;
    }

    void exitWithError(const std::string &errorMessage)
    {
        log("ERROR " + errorMessage);
        exit(1);
    }

}

namespace http
{
    TcpServer::TcpServer(const char* ip_address, int port)
    : m_destination_ip_address(ip_address), m_main_port(port), m_destination_port(8088), 
    m_main_socket(), m_dest_socket(), m_client_socket(), m_incomingMessage(), m_main_socketAddress(), 
    m_main_socketAddress_len(sizeof(m_main_socketAddress)), 
    m_serverMessage(buildResponse())
    {
        m_main_socketAddress.sin_family = AF_INET;
        m_main_socketAddress.sin_port = htons(m_main_port);
        m_main_socketAddress.sin_addr.s_addr = INADDR_ANY;
        if (startServer() != 0)
        {
            std::ostringstream ss;
            ss << "Failed to start server with PORT: " <<
            ntohs(m_main_socketAddress.sin_port);
            log(ss.str());
        }
    }
    TcpServer::~TcpServer()
    {
        closeServer();
    }

    int TcpServer::startServer()
    {
        m_main_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_main_socket < 0)
        {
            exitWithError("Cannot create main socket");
            return 1;
        }
        if (bind(m_main_socket, (sockaddr *)&m_main_socketAddress, m_main_socketAddress_len) < 0)
        {
            exitWithError("Cannot connect socket to address");
            return 1;
        }
        return 0;
    }

    void TcpServer::closeServer()
    {
        close(m_main_socket);
        exit(0);
    }

    void TcpServer::startListen()
    {
        if (listen(m_main_socket, SOMAXCONN) < 0)
        {
            exitWithError("Socket listen failed");
        }
        std::ostringstream ss;
        ss << "\n*** Listening on ADDRESS "
        << inet_ntoa(m_main_socketAddress.sin_addr)
        << " PORT: " << ntohs(m_main_socketAddress.sin_port)
        << " ***\n\n";
        log(ss.str());

        int bytesReceived;

        while (true)
        {
            log("====== Waiting for a new connection ======\n\n\n");
            acceptConnection();

            m_dest_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (m_dest_socket < 0)
            {
                exitWithError("Cannot create destination socket");
            }

            struct sockaddr_in destinationAddress;
            memset(&destinationAddress, 0, sizeof(destinationAddress));
            destinationAddress.sin_family = AF_INET;
            destinationAddress.sin_addr.s_addr = inet_addr(m_destination_ip_address);
            destinationAddress.sin_port = htons(m_destination_port);

            if (connect(m_dest_socket, (struct sockaddr*)&destinationAddress, sizeof(destinationAddress)) == -1) {
                std::cerr << "Error connecting to destination server" << std::endl;
                continue;
            }

            char buffer[BUFFER_SIZE] = {0};
            bytesReceived = read(m_client_socket, buffer, BUFFER_SIZE);
            if (bytesReceived < 0)
            {
                exitWithError("Failed to read bytes from client socket connection");
            }

            std::ostringstream ss;
            ss << "------ Received request from client ------ \n\n";
            log(ss.str());

            sendResponse();
            close(m_client_socket);
            close(m_dest_socket);
        }
    }

    void TcpServer::acceptConnection()
    {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        m_client_socket = accept(m_main_socket, (sockaddr *)&clientAddress, &clientAddressLength);
        if (m_client_socket < 0)
        {
            std::ostringstream ss;
            ss << "Server failed to accept incoming connection from ADDRESS: " <<
            inet_ntoa(clientAddress.sin_addr) << "; PORT: " <<
            ntohs(clientAddress.sin_port);
            exitWithError(ss.str());
        }
    }

    std::string TcpServer::buildResponse()
    {
        std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> Hey! </h1><p> Hi! </p></body></html>";
        std::ostringstream ss;
        ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
           << htmlFile;

        return ss.str();
    }

    void TcpServer::sendResponse()
    {
        long bytesSent;
        bytesSent = write(m_dest_socket, m_serverMessage.c_str(), m_serverMessage.size());

        if (bytesSent == m_serverMessage.size())
        {
            log("------ Server Response sent to client ------\n\n");
        }
        else
        {
            log("Error sending response to client");
        }
    }
}
