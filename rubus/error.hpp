#ifndef RUBUS_RUBUSERROR_HPP_INCLUDED
#define RUBUS_RUBUSERROR_HPP_INCLUDED

#include <cstring>
#include <stdexcept>
#include <string>

namespace rubus {

// =============================================================================
//                             rubus::RubusError
// =============================================================================
struct RubusError : std::runtime_error {

    RubusError(const std::string& msg) : std::runtime_error(msg) { }

    RubusError(const std::string& msg, int errnum)
        : std::runtime_error(msg + ": " + std::string(strerror(errnum))) { }

};

}  // namespace rubus

#endif  // RUBUS_RUBUSERROR_HPP_INCLUDED
