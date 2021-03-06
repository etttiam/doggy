#ifndef DOGGY_NET_TCPSERVER_H
#define DOGGY_NET_TCPSERVER_H

#include "doggy/net/header.h"

#include "doggy/net/TcpConnection.h"

namespace doggy
{
        namespace net
        {
                class Acceptor;
                class EventLoop;
                class EventLoopPool;

                class TcpServer
                {
                        using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
                        using ThreadInitCallback = std::function<void(EventLoop *)>;
                        using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
                        using MessageCallback = std::function<void(const TcpConnectionPtr &, Buff &buff)>;

                public:
                        enum Option
                        {
                                kNoReusePort,
                                kReusePort,
                        };

                        TcpServer(EventLoop *loop, const InetAddress &localAddr, Option option = kReusePort);
                        ~TcpServer();

                public:
                        // nocopyable
                        TcpServer(const TcpServer &) = delete;
                        TcpServer &operator=(TcpServer &) = delete;

                public:
                        void start();
                        EventLoop *getLoop() const { return loop_; };
                        /// must be called before @c start
                        /// @param numThreads
                        ///-0 means all I/O in loop's thread,no thread will created.
                        ///-1 means all I/O in another thread.
                        ///-N means a thread pool with N threads.
                        /// this is default setting
                        /// the default value is the return value of std::thread::hardware_concurrency()
                        void setNumThreads(int numThreads);

                        std::shared_ptr<EventLoopPool> threadPool() { return threadPool_; };
                        void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; };
                        void setConnectionCallback(ConnectionCallback &&cb) { connectionCallback_ = std::move(cb); };
                        void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; };
                        void setMessageCallback(MessageCallback &&cb) { messageCallback_ = std::move(cb); };
                        void setThreadInitCallback(const ThreadInitCallback &cb) { threadInitCallback_ = cb; };
                        void setThreadInitCallback(ThreadInitCallback &&cb) { threadInitCallback_ = std::move(cb); };

                private:
                        void newConnection(int sockfd, const InetAddress &peerAddr);
                        void removeConnection(const TcpConnectionPtr &conn);
                        void removeConnectionInLoop(const TcpConnectionPtr &conn);

                private:
                        EventLoop *loop_;
                        std::unique_ptr<Acceptor> acceptor_;
                        int numThreads_;
                        std::shared_ptr<EventLoopPool> threadPool_;
                        ConnectionCallback connectionCallback_;
                        MessageCallback messageCallback_;
                        ThreadInitCallback threadInitCallback_;

                        std::atomic<bool> started_;
                };

        } // namespace net

} // namespace doggy

#endif // DOGGY_NET_TCPSERVER_H
