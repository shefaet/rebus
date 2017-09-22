#ifndef RUBUS_GPIO_DETAIL_HPP_INCLUDED
#define RUBUS_GPIO_DETAIL_HPP_INCLUDED

#include "io_detail.hpp"

#include "../error.hpp"
#include "../types.hpp"

#include <bitset>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <sstream>
#include <string>
#include <thread>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>


namespace rubus {
namespace detail {


// GPIO pins on the BCM2837 can be controlled in userland programs through the
// `/dev/gpiomem` interface, which exposes all of the BCM2837 peripheral
// registers, with the GPIO registers starting at 0x0020000. We mmap this "file"
// for faster access, and make the mmap globally available through this
// singleton. Individual pins are written to and read from by manipulating bits
// in the mmap'd GPIO registers. This struct creates objects to simplify this
// manipulation.


class GPIOMap {

  public:

    static volatile uint32_t* base()
    {
        static GPIOMap instance;
        return instance.base_;
    }

    GPIOMap()
    {
        int fd = ::open("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC);
        if (fd < 0) {
            throw RubusError("Failed to open /dev/gpiomem", errno);
        }
        const size_t pageSz = 4*1024;
        const off_t gpioOffset = 0x0020000;
        base_ = static_cast<uint32_t *>(
          ::mmap(0, pageSz, PROT_READ|PROT_WRITE, MAP_SHARED, fd, gpioOffset)
        );
        if (base_ == MAP_FAILED) {
            throw RubusError("mmap failed", errno);
        }
    }

  private:

    volatile uint32_t *base_;

};  // class GPIOMap


struct GPIOPin {

    GPIOPin(pin_no_t num)
        : base      (GPIOMap::base())
        , fsel      (base + 0  + num/10)  // GPFSET   = base + [0..6]
        , set       (base + 7  + num/32)  // GPSET    = base + [7..8]
        , clr       (base + 10 + num/32)  // GPCLR    = base + [10..11]
        , lev       (base + 13 + num/32)  // GPLEV    = base + [13..14]
        , pud       (base + 37)           // GPPUD    = base + 37
        , pudclk    (base + 38 + num/32)  // GPPUDCLK = base + [38..39]
        , fselShift ((num % 10) * 3)
        , mask      (1 << (num & 31))
    {
    }

    volatile uint32_t* base;
    volatile uint32_t* fsel;
    volatile uint32_t* set;
    volatile uint32_t* clr;
    volatile uint32_t* lev;
    volatile uint32_t* pud;
    volatile uint32_t* pudclk;
    uint32_t fselShift;
    uint32_t mask;
    bool isOutput;

};  // struct GPIOPin


class GPIOOutput : public OutputInterface {

  public:

    explicit GPIOOutput(GPIOPin& pin, bool state)
        : pin_(pin)
    {
        std::bitset<32> newfsel = *pin_.fsel;
        newfsel[pin_.fselShift] = true;
        *pin_.fsel = newfsel.to_ulong();
        pin_.isOutput = true;
        set(state);
    }

    virtual void set(bool state) override
    {
        if (pin_.isOutput) {
            auto reg = state ? pin_.set : pin_.clr;
            *reg = pin_.mask;
        } else {
            throw RubusError("GPIO pin is not currently an output");
        }
    }

  private:

    GPIOPin& pin_;

}; // class Output


class GPIOInput : public InputInterface {

  public:

    explicit GPIOInput(GPIOPin& pin, PUDMode pudMode) : pin_(pin)
    {
        // Select input function
        std::bitset<32> newfsel = *pin_.fsel;
        newfsel[pin_.fselShift] = false;
        *pin_.fsel = newfsel.to_ulong();
        pin_.isOutput = false;
        // Select pull up/down mode
        switch (pudMode) {
            case PUDMode::Off:  *pin_.pud = 0; break;
            case PUDMode::Down: *pin_.pud = 1; break;
            case PUDMode::Up:   *pin_.pud = 2; break;
            default:            break;
        }
        using std::this_thread::sleep_for;
        using std::chrono::microseconds;
        sleep_for(microseconds(1));
        *pin_.pudclk = pin_.mask;
        sleep_for(microseconds(1));
        *pin_.pud = 0;
        sleep_for(microseconds(1));
        *pin_.pudclk = 0;
    }

    virtual bool get() const override
    {
        if (!pin_.isOutput) {
            return (*pin_.lev & pin_.mask) != 0;
        } else {
            throw RubusError("GPIO pin is not currently an input");
        }
    }

  private:

    GPIOPin& pin_;

}; // class Input


}  // namespace detail
}  // namespace rubus

#endif // RUBUS_GPIO_HPP_INCLUDED
