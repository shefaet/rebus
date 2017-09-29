#ifndef RUBUS_BIDIRECTIONALIMPL__HPP_INCLUDED
#define RUBUS_BIDIRECTIONALIMPL__HPP_INCLUDED

#include <rubus/types.hpp>

namespace rubus {


class OutputImpl {

  public:

    OutputImpl()                             = default;
    OutputImpl& operator=(const OutputImpl&) = default;
    OutputImpl(const OutputImpl&)            = default;
    OutputImpl(OutputImpl&&)                 = default;
    OutputImpl& operator=(OutputImpl&&)      = default;
    virtual ~OutputImpl()                    = default;

    virtual void set(bool state) =0;

};  // class OutputImpl


class InputImpl {

  public:

    InputImpl()                            = default;
    InputImpl& operator=(const InputImpl&) = default;
    InputImpl(const InputImpl&)            = default;
    InputImpl(InputImpl&&)                 = default;
    InputImpl& operator=(InputImpl&&)      = default;
    virtual ~InputImpl()                   = default;

    virtual bool get() const =0;

};  // class InputImpl


class BidirectionalImpl : public InputImpl, public OutputImpl {

  public:

    BidirectionalImpl()                                    = default;
    BidirectionalImpl& operator=(const BidirectionalImpl&) = default;
    BidirectionalImpl(const BidirectionalImpl&)            = default;
    BidirectionalImpl(BidirectionalImpl&&)                 = default;
    BidirectionalImpl& operator=(BidirectionalImpl&&)      = default;
    virtual ~BidirectionalImpl()                           = default;

    virtual void set(bool state)      = 0;
    virtual bool get() const          = 0;
    virtual IOMode mode() const       = 0;
    virtual void setMode(IOMode mode) = 0;

};  // class BidirectionalImpl


}  // namespace rubus

#endif
