#ifndef RUBUS_IO_HPP_INCLUDED
#define RUBUS_IO_HPP_INCLUDED

#include "internal/io_detail.hpp"

#include "error.hpp"

#include <memory>

namespace rubus {


class Input {

  public:

    Input(std::unique_ptr<detail::InputInterface> impl)
        : impl_(std::move(impl))
    {
    }

    bool get() const
    {
        return impl_->get();
    }

    operator bool() const
    {
        return get();
    }

  private:

    std::unique_ptr<detail::InputInterface> impl_;

};  // class Input


class Output {

  public:

    Output(std::unique_ptr<detail::OutputInterface> impl)
        : impl_(std::move(impl))
    {
    }

    void set(bool state)
    {
        impl_->set(state);
    }

    Output& operator=(bool state)
    {
        set(state);
        return *this;
    }

  private:

    std::unique_ptr<detail::OutputInterface> impl_;

};  // class Output


}  // namespace rubus

#endif  // RUBUS_IO_HPP_INCLUDED
