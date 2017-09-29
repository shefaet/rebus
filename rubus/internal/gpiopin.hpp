#ifndef RUBUS_GPIOPIN_HPP_INCLUDED
#define RUBUS_GPIOPIN_HPP_INCLUDED

#include <rubus/types.hpp>
#include <rubus/internal/ioimpl.hpp>

#include <bitset>
#include <chrono>
#include <cstdint>
#include <thread>

namespace rubus {


// Individual pins are written to and read from by manipulating bits
// in the mmap'd GPIO registers. This class creates objects to simplify this
// manipulation.

class GPIOPin : public BidirectionalImpl {

  public:

    GPIOPin(pin_no_t num, volatile uint32_t* base)
        : fsel_      (base + 0  + num/10)  // GPFSET   = base + [0..6]
        , set_       (base + 7  + num/32)  // GPSET    = base + [7..8]
        , clr_       (base + 10 + num/32)  // GPCLR    = base + [10..11]
        , lev_       (base + 13 + num/32)  // GPLEV    = base + [13..14]
        , pud_       (base + 37)           // GPPUD    = base + 37
        , pudclk_    (base + 38 + num/32)  // GPPUDCLK = base + [38..39]
        , fselShift_ ((num % 10) * 3)
        , mask_      (1 << (num & 31))
    {
    }

    virtual void set(bool state) override
    {
        auto reg = state ? set_ : clr_;
        *reg = mask_;
    }

    virtual bool get() const override
    {
        return (*lev_ & mask_) != 0;
    }

    virtual IOMode mode() const override
    {
        return mode_;
    }

    virtual void setMode(IOMode mode) override
    {
        std::bitset<32> newfsel = *fsel_;
        newfsel[fselShift_] = (mode == IOMode::Output);
        *fsel_ = newfsel.to_ulong();
        mode_ = mode;
    }

    void setPUDMode(PUDMode pudMode)
    {
        switch (pudMode) {
            case PUDMode::Off:  *pud_ = 0; break;
            case PUDMode::Down: *pud_ = 1; break;
            case PUDMode::Up:   *pud_ = 2; break;
            default:            break;
        }
        using std::this_thread::sleep_for;
        using std::chrono::microseconds;
        sleep_for(microseconds(1));
        *pudclk_ = mask_;
        sleep_for(microseconds(1));
        *pud_ = 0;
        sleep_for(microseconds(1));
        *pudclk_ = 0;
    }

  private:

    volatile uint32_t* fsel_;
    volatile uint32_t* set_;
    volatile uint32_t* clr_;
    volatile uint32_t* lev_;
    volatile uint32_t* pud_;
    volatile uint32_t* pudclk_;
    uint32_t fselShift_;
    uint32_t mask_;
    IOMode mode_;

};  // class GPIOPin


}  // namespace rubus

#endif // RUBUS_GPIO_HPP_INCLUDED
