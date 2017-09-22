#ifndef RUBUS_I2CLCD_HPP_INCLUDED
#define RUBUS_I2CLCD_HPP_INCLUDED

#include <rubus/io.hpp>
#include <rubus/error.hpp>

#include <bitset>
#include <cstdint>
#include <chrono>
#include <string>
#include <thread>

namespace rubus {

class LCD {

  public:

    struct Config {

        uint8_t rows;
        uint8_t cols;
        Output rs;
        Output rw;
        Output e;
        Output led;
        Output db4;
        Output db5;
        Output db6;
        Output db7;

    };  // struct Config

    explicit LCD(Config&& cfg) : cfg_(std::move(cfg)), control_(0x08)
    {
        setWrite();
        set4BitMode();
        setDisplay(true);
        setCursor(false);
        setBlink(false);
        clear();
        setEntryMode();
        setBacklight(true);
    }

    ~LCD()
    {
        clear();
        setBacklight(false);
    }

    void clear()
    {
        sendCommand(0x01);
        setCursorPosition(0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    void setCursorPosition(uint8_t x, uint8_t y)
    {
        if (cfg_.cols <= x || cfg_.rows <= y) {
            throw RubusError("Invalid cursor position");
        }
        if (x == 0 && y == 0) {
            static const uint8_t returnHome = 0x02;
            sendCommand(returnHome);
        } else {
            static const uint8_t setDDRAM = 0x80;
            static const uint8_t rowStarts[] = {0x00, 0x40};
            sendCommand(x + (setDDRAM | rowStarts[y]));
        }
        cx_ = x;
        cy_ = y;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    void setBacklight(bool state)
    {
        cfg_.led = state;
    }

    void setBlink(bool state)
    {
        setControl(0, state);
    }

    void setCursor(bool state)
    {
        setControl(1, state);
    }

    void setDisplay(bool state)
    {
        setControl(2, state);
    }

    void write(char c)
    {
        if (++cx_ == 16) {
            cx_ = 0;
            if (++cy_ == 2) {
                cy_ = 0;
            }
            setCursorPosition(cx_, cy_);
        }
        sendData(c);
    }

    void write(const std::string& s)
    {
        for (auto c : s) { write(c); }
    }

  private:

    void latch()
    {
        cfg_.e = true;
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        cfg_.e = false;
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }

    void send(std::bitset<8> data, bool isData)
    {
        cfg_.rs = isData;
        cfg_.db4 = data[4];
        cfg_.db5 = data[5];
        cfg_.db6 = data[6];
        cfg_.db7 = data[7];
        latch();
        cfg_.db4 = data[0];
        cfg_.db5 = data[1];
        cfg_.db6 = data[2];
        cfg_.db7 = data[3];
        latch();
    }

    void sendCommand(uint8_t command)
    {
        send(command, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    void sendData(uint8_t data)
    {
        send(data, true);
    }

    void set4BitMode()
    {
        // Set 4-bit by setting 8-bit three times, then setting 4-bit last
        static const uint8_t funcmode = 0x20;
        static const uint8_t func8bit = 0x10;
        static const uint8_t func4bit = 0x00;
        static const uint8_t func8bitNibble = (funcmode | func8bit) >> 4;
        static const uint8_t func4bitNibble = (funcmode | func4bit) >> 4;
        sendCommand(func8bitNibble << 4 | func8bitNibble);
        sendCommand(func8bitNibble << 4 | func4bitNibble);
    }

    void setControl(uint8_t n, bool state)
    {
        control_[n] = state;
        sendCommand(control_.to_ulong());
    }

    void setEntryMode()
    {
        static const uint8_t entryMode = 0x04;
        static const uint8_t entryIncrement = 0x02;
        sendCommand(entryMode | entryIncrement);
    }

    void setWrite()
    {
        cfg_.rw = false;
    }

  private:

    Config cfg_;
    std::bitset<8> control_;
    uint8_t cx_;
    uint8_t cy_;

};

}  // namespace rubus

#endif  // RUBUS_I2CLCD_HPP_INCLUDED
