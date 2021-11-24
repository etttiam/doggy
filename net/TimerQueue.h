#pragma once

#include "../base/header.h"

#include "./EventLoop.h"
#include "./Timer.h"
#include "./TimerId.h"

namespace doggy
{
        namespace net
        {
                class TimerQueue
                {
                        using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
                        using TimerCallback = std::function<void()>;

                public:
                        explicit TimerQueue(EventLoop *loop);
                        ~TimerQueue();

                public:
                        TimerQueue(const TimerQueue &) = delete;
                        TimerQueue &operator=(const TimerQueue &) = delete;

                public:
                        TimerId addTimer(TimerCallback cb,
                                         TimePoint when,
                                         double interval);

                        void cancel(TimerId timerId);

                private:
                        using Entry = std::pair<TimePoint, std::unique_ptr<Timer>>;
                        using TimerList = std::set<Entry, std::less<>>;
                        using ActiveTimer = std::pair<std::unique_ptr<Timer>, int64_t>;
                        using ActiveTimerSet = std::set<ActiveTimer, std::less<>>;

                        void addTimerInLoop(Timer *timer);
                        void cancelInLoop(TimerId timerId);

                        void handleRead();

                        std::vector<Entry> getExpired(TimePoint now);
                        void reset(const std::vector<Entry> &expired, TimePoint now);

                        bool insert(Timer *timer);

                private:
                        EventLoop *loop_;
                        int timerFd_;
                        Channel timerFdChannel_;

                        TimerList timers_;

                        ActiveTimerSet activeTimers_;
                        std::atomic<bool> callingExpuredTimers_;
                        ActiveTimerSet cancelingTimers_;
                };

        } // namespace net

} // namespace  doggy
