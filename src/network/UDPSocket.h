#pragma once
#include <sys/socket.h>
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
namespace network{
    class UDPSocket {
        public:
            UDPSocket() noexcept :  socket_fd(-1) {};
            explicit UDPSocket(int domain = AF_INET) {
                open(domain);
            }
            explicit UDPSocket(int fd, bool takeOwnership) noexcept : socket_fd(-1){
                if (takeOwnership){
                    socket_fd = fd;
                }
            }
            UDPSocket(const UDPSocket&) = delete;
            UDPSocket& operator=(const UDPSocket&) = delete;
            UDPSocket(UDPSocket&& other) noexcept : socket_fd(other.socket_fd){
                other.socket_fd = -1;
            }
            UDPSocket& operator=(UDPSocket&& other) noexcept {
                if (this != &other){
                    closeIsValid();
                    socket_fd = other.socket_fd;
                    other.socket_fd = -1;
                }
                return *this;
            }
            void open(int domain = AF_INET){
                closeIsValid();
                int fd = ::socket(domain, SOCK_DGRAM, 0);
                if (socket_fd < 0){
                    throw std::system_error(errno, std::generic_category(), "Failed to create UDP socket");
                }
                socket_fd = fd;
            }
            bool open_ec(int domain = AF_INET,std::error_code& ec){
                closeIsValid();
                int fd = ::socket(domain, SOCK_DGRAM, 0);
                if (fd < 0){
                    ec = std::error_code(errno, std::generic_category());
                    return false;
                } else {
                    ec.clear();
                    socket_fd = fd;
                    return true;
                }
            }
            void bind(const std::string& ip, uint16_t port){
                if (socket_fd < 0){
                    open();
                }
                sockaddr_in addr{};
                addr.sin_family = AF_INET;
                addr.sin_port = htons(port);
                if (ip.empty()){
                    addr.sin_addr.s_addr = INADDR_ANY;
                } else {
                    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0){
                        throw std::system_error(errno, std::generic_category(), "Invalid IP address");
                    }
                }
                if (::bind(socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0){
                    throw std::system_error(errno, std::generic_category(), "Failed to bind UDP socket");
                }
            }
            bool bind_ec (const std::string& ip, uint16_t port, std::error_code& ec){
                if (socket_fd < 0){
                    if (!open_ec(AF_INET, ec)){
                        return false;
                    }
                }
                sockaddr_in addr{};
                addr.sin_family = AF_INET;
                addr.sin_port = htons(port);
                if (ip.empty()){
                    addr.sin_addr.s_addr = INADDR_ANY;
                } else {
                    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0){
                        ec = std::error_code(errno, std::generic_category());
                        return false;
                    }
                }
                if (::bind(socket_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0){
                    ec = std::error_code(errno, std::generic_category());
                    return false;
                }
                ec.clear();
                return true;
            }
            void setNonBlocking(bool nonBlocking){
                int flags = fcntl(socket_fd, F_GETFL,0);
                if (flags < 0){
                    throw std::system_error(errno, std::generic_category(), "Failed to get socket flags");
                }
                if (nonBlocking){
                    flags |= O_NONBLOCK;
                } else {
                    flags &= ~O_NONBLOCK;
                }
                if (fcntl(socket_fd, F_SETFL, flags) < 0){
                    throw std::system_error(errno, std::generic_category(), "Failed to set socket flags");
                }
            
            }
            bool setNonBlocking_ec(bool nonBlocking, std::error_code& ec){
                int flags = fcntl(socket_fd, F_GETFL,0);
                if (flags < 0){
                    ec = std::error_code(errno, std::generic_category());
                    return false;
                }
                if (nonBlocking){
                    flags |= O_NONBLOCK;
                } else {
                    flags &= ~O_NONBLOCK;
                }
                if (fcntl(socket_fd, F_SETFL, flags) < 0){
                    ec = std::error_code(errno, std::generic_category());
                    return false;
                }
                ec.clear();
                return true;
            }
            void setResueAddress(bool reuse){
                int opt = reuse ? 1 : 0;
                if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
                    throw std::system_error(errno, std::generic_category(), "Failed to set SO_REUSEADDR");
                }
            }
            ssize_t sendTo(const void* data, size_t len, const sockaddr* destAddr, socklen_t addrlen, std::error_code& ec){
                if (socket_fd < 0){
                    ec = std::error_code(EBADF, std::generic_category());
                    return -1;
                }
                #if defined(MSG_NOSIGNAL){
                    ssize_t n =::sendto(socket_fd, data, len, MSG_NOSIGNAL, destAddr,addrlen);
                #else{
                    ssize_t n =::sendto(socket_fd, data, len, 0, destAddr,addrlen);
                }
                #endif
                if (n < 0){
                    ec = std::error_code(errno, std::generic_category());
                    return -1;
                }
                ec.clear();
                return n;
            }
            ssize_t sentToOrThrow(const void* data, size_t len, const sockaddr* destAddr, socklen_t addrlen){
                std::error_code ec;
                ssize_t n = sendTo(data, len, destAddr, addrlen, ec);
                if (n < 0){
                    throw std::system_error(ec,"sentTo()");
                }
                return n;

            }
            ssize_t recvFrom(void* buffer, size_t maxLen, sockaddr_storage& outFrom, socklen_t* outFromLen, int flags, std::error_code& ec){
                if (socket_fd < 0){
                    ec = std::error_code(EBADF, std::generic_category());
                    return -1;
                }
                socklen_t addrlen = static_cast<socklen_t>(sizeof(outFrom));
                for (;;){
                    ssize_t n = ::recvfrom(socket_fd, buffer, maxLen, flags, reinterpret_cast<sockaddr*>(&outFrom), &addrlen);
                    if (n > 0){
                        outFromLen = &addrlen;
                        ec.clear();
                        return n;
                    }
                    int err = errno;
                    if (err == EINTR){
                        continue; 
                    }
                    ec = std::error_code(err, std::generic_category());
                    return -1;
                }
            }
            ssize_t recvFromOrThrow(void* buffer, size_t maxLen, sockaddr_storage& outFrom, socklen_t* outFromLen, int flags = 0){
                std::error_code ec;
                ssize_t n = recvFrom(buffer, maxLen, outFrom, outFromLen, flags, ec);
                if (n < 0){
                    throw std::system_error(ec,"recvFrom()");
                }
                return n;
            }
            static sockaddr_in makeSockAddrIn(const std::string& ip, uint16_t port){
                sockaddr_in addr{};
                addr.sin_family = AF_INET;
                addr.sin_port = htons(port);
                if (ip.empty()){
                    addr.sin_addr.s_addr = INADDR_ANY;
                } 
                else {
                    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0){
                        throw std::system_error(errno, std::generic_category(), "Invalid IP address");
                    }
                }
                return addr;
            }
            
            void close(){
                closeIsValid();
            }
            int fd() const noexcept {
                return socket_fd;
            }
            bool isOpen() const noexcept {
                return socket_fd >= 0;
            }
            
            ~UDPSocket() {
                closeIsValid();
            }
        private:
            int socket_fd;
            void closeIsValid(){
                if (socket_fd >=0){
                    ::close(socket_fd);
                    socket_fd = -1;
                }
            }
    };
}