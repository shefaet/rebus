#ifndef RUBUS_PCF8574_HPP_INCLUDED
#define RUBUS_PCF8574_HPP_INCLUDED

#include <rubus/rubus.hpp>
#include <rubus/internal/i2c.hpp>
#include <rubus/internal/ioimpl.hpp>

#include <bitset>
#include <memory>
#include <vector>

namespace rubus {


class PCF8574 {

  public:

    PCF8574(uint8_t bus, uint8_t addr) : i2c_(bus, addr)
    {
        i2c_.write(0);
        state_ = 0;
    }

    Output output(pin_no_t num)
    {
        checkValidPin(num);
        return Output(std::make_shared<PCF8574Output>(num, *this));
    }

  private:

    static void checkValidPin(pin_no_t num)
    {
        if (7 < num) {
            throw RubusError("Invalid pin number");
        }
    }

    void set(pin_no_t n, bool state)
    {
        state_[n] = state;
        i2c_.write(state_.to_ulong());
    }

  private:

    class PCF8574Output : public OutputImpl {

      public:

        PCF8574Output(pin_no_t num, PCF8574& pcf8574)
            : num_(num), device_(pcf8574)
        {
            set(false);
        }

        virtual void set(bool state) override
        {
            device_.set(num_, state);
        }

      private:

        pin_no_t num_;
        PCF8574& device_;

    };

  private:

    I2CDevice i2c_;
    std::bitset<8> state_;

};  // class PCF8574


}  // namespace rubus

#endif
