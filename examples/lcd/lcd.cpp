#include <rubus/devices/lcd.hpp>
#include <rubus/devices/pcf8574.hpp>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

using namespace rubus;
using namespace std::this_thread;
using namespace std::chrono_literals;

int main()
{
    PCF8574 pcf8574(1, 0x3f);

    LCD lcd {{
        .rows = 16,
        .cols = 2,
        .rs   = pcf8574.output(0),
        .rw   = pcf8574.output(1),
        .e    = pcf8574.output(2),
        .led  = pcf8574.output(3),
        .db4  = pcf8574.output(4),
        .db5  = pcf8574.output(5),
        .db6  = pcf8574.output(6),
        .db7  = pcf8574.output(7)
    }};

    std::string line;
    while (std::getline(std::cin, line)) {
        lcd.clear();
        lcd.write(line);
    }

    lcd.clear();
    lcd.write("Goodbye!");
    sleep_for(2s);
}
