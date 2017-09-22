#include <rubus/gpio.hpp>
#include <rubus/devices/hcsr04.hpp>
#include <rubus/devices/pcf8574.hpp>
#include <rubus/devices/lcd.hpp>

#include <unistd.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace std::this_thread;

int main(int argc, char **argv)
{
    int opt, t = 21, e = 20, l = 5, d = 1000;

    while ((opt = getopt(argc, argv, "t:e:l:d:")) != -1) {
        std::string usage = "Usage: ranger -t TRIGGER_PIN -e ECHO_PIN\n"
                            "              -l LIGHT_PIN -d DELAY_MS\n";
        switch (opt) {
            case 't': t = atoi(optarg); break;
            case 'e': e = atoi(optarg); break;
            case 'l': l = atoi(optarg); break;
            case 'd': d = atoi(optarg); break;
            case '?': std::cerr << usage; return -1;
        }
    }

    rubus::Output lightPin = rubus::GPIO::output(l);
    rubus::HCSR04 ranger(rubus::GPIO::output(t), rubus::GPIO::input(e));
    rubus::PCF8574 i2cExpander(1, 0x3f);
    rubus::LCD lcd {{ .rows = 16,
                      .cols = 2,
                      .rs   = i2cExpander.output(0),
                      .rw   = i2cExpander.output(1),
                      .e    = i2cExpander.output(2),
                      .led  = i2cExpander.output(3),
                      .db4  = i2cExpander.output(4),
                      .db5  = i2cExpander.output(5),
                      .db6  = i2cExpander.output(6),
                      .db7  = i2cExpander.output(7)  }};

    std::ostringstream oss;
    while (true) {
        auto dist = ranger.getDistance();
        oss << std::fixed << std::setprecision(1) << dist * 100.0 << " cm";
        lcd.write(oss.str());
        lightPin = dist < 0.15;
        sleep_for(milliseconds(d));
        lcd.clear();
        oss.str("");
    }
}
