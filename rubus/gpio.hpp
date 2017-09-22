#ifndef RUBUS_GPIO_HPP_INCLUDED
#define RUBUS_GPIO_HPP_INCLUDED

#include "internal/gpio_detail.hpp"

#include "error.hpp"
#include "io.hpp"
#include "types.hpp"

#include <memory>
#include <vector>

namespace rubus {

class GPIO {

  public:

    static Output output(pin_no_t n, bool state=false)
    {
        checkValidPin(n);
        return Output(std::unique_ptr<detail::GPIOOutput>(
            new detail::GPIOOutput(pin(n), state)
        ));
    }

    static Input input(pin_no_t n, PUDMode mode=PUDMode::Off)
    {
        checkValidPin(n);
        return Input(std::unique_ptr<detail::GPIOInput>(
            new detail::GPIOInput(pin(n), mode)
        ));
    }

  private:

    static detail::GPIOPin& pin(pin_no_t n)
    {
        static GPIO instance;
        return instance.pins_[n - 2];
    }

    static void checkValidPin(pin_no_t num)
    {
        if (num < 2 || 27 < num) {
            throw RubusError("Invalid pin number");
        }
    }

    GPIO()
    {
        for (size_t i = 2; i <= 27; ++i) {
            pins_.push_back(detail::GPIOPin(i));
        }
    }

  private:

    std::vector<detail::GPIOPin> pins_;

};  // class GPIO

}  // namespace rubus

#endif // RUBUS_GPIO_HPP_INCLUDED
