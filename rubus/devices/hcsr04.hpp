#ifndef RUBUS_HCSR04_HPP_INCLUDED
#define RUBUS_HCSR04_HPP_INCLUDED

#include <rubus/rubus.hpp>

#include <chrono>
#include <thread>

namespace rubus {


class HCSR04 {

  public:

    struct Config {
        Output trigger;
        Input  echo;
    };

    HCSR04(Config cfg)
        : trigger_(cfg.trigger)
        , echo_(cfg.echo)
    {
    }

    double getDistance()
    {
        trigger();
        return echo_.duration<std::chrono::microseconds>() * 1.7E-4;
    }

  private:

    void trigger()
    {
        trigger_.set(false, std::chrono::microseconds(20));
        trigger_.set(true,  std::chrono::microseconds(10));
        trigger_ = false;
    }

  private:

    Output trigger_;
    Input  echo_;

};  // class HCSR04


}  // namespace rubus

#endif
