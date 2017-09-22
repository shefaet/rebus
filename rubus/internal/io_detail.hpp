#ifndef RUBUS_IO_DETAIL_HPP_INCLUDED
#define RUBUS_IO_DETAIL_HPP_INCLUDED

namespace rubus {
namespace detail {


class InputInterface {

  public:

    InputInterface()                                 = default;
    InputInterface& operator=(const InputInterface&) = default;
    InputInterface(const InputInterface&)            = default;
    InputInterface(InputInterface&&)                 = default;
    InputInterface& operator=(InputInterface&&)      = default;
    virtual ~InputInterface()                        = default;

    virtual bool get() const =0;

};


class OutputInterface {

  public:

    OutputInterface()                                  = default;
    OutputInterface& operator=(const OutputInterface&) = default;
    OutputInterface(const OutputInterface&)            = default;
    OutputInterface(OutputInterface&&)                 = default;
    OutputInterface& operator=(OutputInterface&&)      = default;
    virtual ~OutputInterface()                         = default;

    virtual void set(bool state) = 0;

};


}  // namespace detail
}  // namespace rubus

#endif  // RUBUS_IO_HPP_INCLUDED
