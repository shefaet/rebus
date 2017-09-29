#ifndef RUBUS_DHT11_HPP_INCLUDED
#define RUBUS_DHT11_HPP_INCLUDED

#include <rubus/io.hpp>

#include <chrono>
#include <thread>

namespace rubus {


class DHT11 {

  public:

    DHT11(Output trigger, Input echo)
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

};  // class DHT11


}  // namespace rubus

#endif
