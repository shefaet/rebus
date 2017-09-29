#ifndef RUBUS_RUBUS_HPP_INCLUDED
#define RUBUS_RUBUS_HPP_INCLUDED

#include <rubus/types.hpp>
#include <rubus/internal/ioimpl.hpp>
#include <rubus/internal/gpiopin.hpp>

#include <cerrno>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace rubus {


class RubusError : std::runtime_error {

  public:

    RubusError(const std::string& msg)
        : std::runtime_error(msg)
    {
    }

    RubusError(const std::string& msg, int errnum)
        : std::runtime_error(msg + ": " + std::string(strerror(errnum)))
    {
    }

};  // class RubusError


class Input {

  public:

    Input()                        = default;
    Input(const Input&)            = default;
    Input& operator=(const Input&) = default;
    Input(Input&&)                 = default;
    Input& operator=(Input&&)      = default;
    virtual ~Input()               = default;

    Input(std::shared_ptr<InputImpl> impl)
        : impl_(impl)
    {
    }

    virtual bool get() const
    {
        return impl_->get();
    }

    operator bool() const
    {
        return get();
    }

  private:

    std::shared_ptr<InputImpl> impl_;

};  // class Input


class Output {

  public:

    Output()                         = default;
    Output(const Output&)            = default;
    Output& operator=(const Output&) = default;
    Output(Output&&)                 = default;
    Output& operator=(Output&&)      = default;
    virtual ~Output()                = default;

    Output(std::shared_ptr<OutputImpl> impl)
        : impl_(impl)
    {
    }

    virtual void set(bool state)
    {
        return impl_->set(state);
    }

    Output& operator=(bool state)
    {
        set(state);
        return *this;
    }

  private:

    std::shared_ptr<OutputImpl> impl_;

};  // class Output


class Bidirectional : public Input, public Output {

  public:

    Bidirectional(std::shared_ptr<BidirectionalImpl> impl)
        : impl_(impl)
    {
    }

    virtual void set(bool state) override
    {
        if (mode() == IOMode::Output) {
            impl_->set(state);
        } else {
            throw RubusError("Cannot set bidirectional pin not in output mode");
        }
    }

    virtual bool get() const override
    {
        if (mode() == IOMode::Input) {
            return impl_->get();
        } else {
            throw RubusError("Cannot read bidirectional pin not in input mode");
        }
    }

    IOMode mode() const
    {
        return impl_->mode();
    }

    void setMode(IOMode mode)
    {
        impl_->setMode(mode);
    }

  private:

    std::shared_ptr<BidirectionalImpl> impl_;

};  // class Bidirectional


class GPIO {

  public:

    static Bidirectional bidirectional(pin_no_t n)
    {
        return Bidirectional(pin(n));
    }

    static Input input(pin_no_t n, PUDMode pudMode=PUDMode::Off)
    {
        auto p = pin(n);
        p->setMode(IOMode::Input);
        p->setPUDMode(pudMode);
        return Input(p);
    }

    static Output output(pin_no_t n, bool state=false)
    {
        auto p = pin(n);
        p->setMode(IOMode::Output);
        p->set(state);
        return Output(p);
    }

  private:

    static std::shared_ptr<GPIOPin> pin(pin_no_t n)
    {
        checkValidPin(n);
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
        int fd = ::open("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC);
        if (fd < 0) {
            throw RubusError("Failed to open /dev/gpiomem", errno);
        }
        const size_t pageSz = 4*1024;
        const off_t gpioOffset = 0x0020000;
        auto base = static_cast<volatile uint32_t *>(
          ::mmap(0, pageSz, PROT_READ|PROT_WRITE, MAP_SHARED, fd, gpioOffset)
        );
        if (base == MAP_FAILED) {
            throw RubusError("mmap failed", errno);
        }
        for (size_t i = 2; i <= 27; ++i) {
            pins_.push_back(std::make_shared<GPIOPin>(i, base));
        }
    }

  private:

    std::vector<std::shared_ptr<GPIOPin>> pins_;

};  // class GPIO


}  // namespace rubus

#endif
