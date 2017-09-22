#ifndef RUBUS_HCSR04_HPP_INCLUDED
#define RUBUS_HCSR04_HPP_INCLUDED

#include <rubus/io.hpp>

#include <chrono>
#include <thread>

namespace rubus {


class HCSR04 {

  public:

    HCSR04(Output trigger, Input echo)
        : trigger_(std::move(trigger))
        , echo_(std::move(echo))
    {
    }

    double getDistance()
    {
        sendTrigger();
        auto delay = timeEcho();
        return delay.count() * 1.7E-7;
    }

  private:

    void sendTrigger()
    {
        using namespace std::chrono;
        using namespace std::this_thread;
        trigger_ = false;
        sleep_for(microseconds(20));
        trigger_ = true;
        sleep_for(microseconds(10));
        trigger_ = false;
    }

    std::chrono::nanoseconds timeEcho()
    {
        using namespace std::chrono;
        while (!echo_) {}
        auto start = steady_clock::now();
        while (echo_)  {}
        return duration_cast<nanoseconds>(steady_clock::now() - start);
    }

  private:

    Output trigger_;
    Input echo_;

};  // class HCSR04


}  // namespace rubus

#endif
