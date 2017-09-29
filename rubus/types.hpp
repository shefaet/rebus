#ifndef RUBUS_TYPES_HPP_INCLUDED
#define RUBUS_TYPES_HPP_INCLUDED

#include <cstdint>

namespace rubus {


using pin_no_t = uint8_t;

enum class PUDMode { Up, Down, Off };

enum class IOMode { Input, Output };


}  // namespace rubus

#endif
