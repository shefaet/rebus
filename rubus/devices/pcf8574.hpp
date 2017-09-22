#ifndef RUBUS_PCF8574_HPP_INCLUDED
#define RUBUS_PCF8574_HPP_INCLUDED

#include <rubus/i2c.hpp>
#include <rubus/io.hpp>
#include <rubus/types.hpp>
#include <rubus/internal/io_detail.hpp>

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
        return Output(std::unique_ptr<PCF8574Output>(
                          new PCF8574Output(num, *this)
                     ));
    }

  private:

    static void checkValidPin(pin_no_t num)
    {
        if (7 < num) {
            throw RubusError("Invalid pin number");
        }
    }

  private:

    class PCF8574Output : public detail::OutputInterface {

      public:

        PCF8574Output(pin_no_t num, PCF8574& pcf8574)
            : mask_(1 << num), pcf8574_(pcf8574)
        {
            set(false);
        }

        virtual void set(bool state) override
        {
            uint8_t tmp = pcf8574_.state_;
            if (state) {
                tmp |= mask_;
            } else {
                tmp &= ~mask_;
            }
            pcf8574_.i2c_.write(tmp);
            pcf8574_.state_ = tmp;
        }

      private:

        uint8_t mask_;
        PCF8574& pcf8574_;

    };

  private:

    I2CDevice i2c_;
    uint8_t state_;

};  // class PCF8574


}  // namespace rubus

#endif
