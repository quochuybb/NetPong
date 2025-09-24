#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <system_error>
#include <string>
#include <chrono>
#include <iostream>
#include "UDPSocket.h"
namespace network {
    class UDPClient{
        public:
        UDPClient()  : sockfd(AF_INET){
            sockfd.setNonBlocking(true);
            x = 0.0f;
            y = 0.0f;
            std::cout << "UDP Client initialized.\n";
        }
        void set_connect_to(const std::string& ip, uint16_t port){
            server_addr = UDPSocket::makeSockAddrIn(ip, port);

        }
        void run_client(){
            while(true){
                const char* msg = "Hello from UDP Client";


            }
        };
        private:
        UDPSocket sockfd ;
        sockaddr_in server_addr;
        float x;
        float y;

    };
}