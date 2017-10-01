#ifndef RUBUS_I2C_HPP_INCLUDED
#define RUBUS_I2C_HPP_INCLUDED

#include <rubus/types.hpp>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <fcntl.h>

#include <string>

namespace rubus {


class I2CDevice {

  public:

    struct Config {
        uint8_t bus;
        uint8_t address;
    };

    I2CDevice(Config cfg)
    {
        std::string busfile = "/dev/i2c-" + std::to_string(cfg.bus);
        if ((fd_ = ::open(busfile.c_str(), O_RDWR)) < 0) {
            throw RubusError("Failed to open I2C bus: " + busfile, errno);
        }
        if (::ioctl(fd_, I2C_SLAVE, cfg.address) < 0) {
            throw RubusError("Failed to open I2C device", errno);
        }
    }

    uint8_t read()
    {
        ::i2c_smbus_data data;
        ::i2c_smbus_ioctl_data payload {
            .read_write = I2C_SMBUS_READ,
            .command    = 0,
            .size       = I2C_SMBUS_BYTE,
            .data       = &data,
        };
        if (::ioctl(fd_, I2C_SMBUS, &payload) != 0) {
            throw RubusError("Failed to read from I2C device", errno);
        }
        return data.byte;
    }

    void write(uint8_t data)
    {
        ::i2c_smbus_ioctl_data payload {
            .read_write = I2C_SMBUS_WRITE,
            .command    = data,
            .size       = I2C_SMBUS_BYTE,
            .data       = 0
        };
        if(::ioctl(fd_, I2C_SMBUS, &payload) != 0) {
            throw RubusError("Failed to write to I2C device", errno);
        }
    }

  private:

    int fd_;

};


}  // namespace rubus

#endif  // RUBUS_I2C_HPP_INCLUDED
