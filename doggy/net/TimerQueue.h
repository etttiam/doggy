#ifndef DOGGY_NET_TIMERQUEUE_H
#define DOGGY_NET_TIMERQUEUE_H

#include "doggy/net/header.h"

#include "doggy/net/Channel.h"

namespace doggy
{
        namespace net
        {
                class EventLoop;
                class Timer;
                class TimerId;

                class TimerQueue
                {
                        using Timestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;
                        using TimerCallback = std::function<void()>;

                public:
                        explicit TimerQueue(EventLoop *loop);
                        ~TimerQueue();

                public:
                        TimerQueue(const TimerQueue &) = delete;
                        TimerQueue &operator=(const TimerQueue &) = delete;

                public:
                        TimerId addTimer(const TimerCallback &cb, Timestamp when, std::chrono::microseconds interval);
                        TimerId addTimer(TimerCallback &&cb, Timestamp when, std::chrono::microseconds interval);

                        void cancel(TimerId timerId);

                private:
                        using Entry = std::pair<Timestamp, std::shared_ptr<Timer>>;
                        using TimerList = std::set<Entry, std::less<>>;
                        using ActiveTimer = std::pair<Timer *, int64_t>;
                        using ActiveTimerSet = std::set<ActiveTimer, std::less<>>;

                        void addTimerInLoop(std::shared_ptr<Timer> &timer);
                        void cancelInLoop(TimerId timerId);

                        void handleRead();

                        void getExpired(std::vector<Entry> &expired, Timestamp now);
                        void reset(std::vector<Entry> &expired, Timestamp now);

                        bool insert(std::shared_ptr<Timer> &timer);

                private:
                        EventLoop *loop_;
                        int timerFd_;
                        Channel timerFdChannel_;

                        TimerList timers_;

                        ActiveTimerSet activeTimers_;
                        std::atomic<bool> callingExpiredTimers_;
                        ActiveTimerSet cancelingTimers_;
                };

        } // namespace net

} // namespace  doggy

#endif // DOGGY_NET_TIMERQUEUE_H
