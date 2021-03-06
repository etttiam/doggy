#ifndef DOGGY_NET_SOCKET_H
#define DOGGY_NET_SOCKET_H

#include "doggy/net/header.h"

namespace doggy
{
        namespace net
        {
                class InetAddress;

                class Socket
                {
                public:
                        explicit Socket(const int &sockfd) : sockFd_{sockfd} {}
                        explicit Socket(int &&sockfd) : sockFd_(std::move(sockfd)) {}
                        ~Socket();

                public:
                        Socket(const Socket &) = delete;
                        Socket &operator=(const Socket &) = delete;

                public:
                        int fd() { return sockFd_; }

                        bool getTcpInfo(tcp_info *) const;

                        void bindAddress(const InetAddress &localaddr);

                        void listen();

                        int accept(InetAddress &peeraddr);
                        int accept();

                        void shutdownWrite();

                        // default on in server
                        void setTcpNoDelay(bool on);

                        // default on in server
                        void setReuseAddr(bool on);

                        // default on in server
                        void setReusePort(bool on);

                        // default on in server
                        void setKeepAlive(bool on);

                private:
                        const int sockFd_;
                };
        } // namespace net

} // namespace doggy

#endif // DOGGY_NET_SOCKET_H
