#include <rubus/rubus.hpp>
#include <rubus/devices/hcsr04.hpp>
#include <rubus/devices/pcf8574.hpp>
#include <rubus/devices/lcd.hpp>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>

#include <unistd.h>

using namespace rubus;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace std::this_thread;

void getOptions(int* t, int* e, int* l, int argc, char** argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "t:e:l:h")) != -1) {
        std::string usage = "Usage: ranger -t TRIGGER -e ECHO -l LIGHT\n";
        switch (opt) {
            case 't': *t = std::stoi(optarg); break;
            case 'e': *e = std::stoi(optarg); break;
            case 'l': *l = std::stoi(optarg); break;
            case 'h':
            case '?': std::cerr << usage; std::exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv)
{
    int t = 21, e = 20, l = 5;
    getOptions(&t, &e, &l, argc, argv);

    Output lightPin = GPIO::output(l);
    HCSR04 ranger {{ .trigger = GPIO::output(t), .echo = GPIO::input(e) }};
    PCF8574 i2c {{ .bus = 1, .address = 0x3f }};
    LCD lcd {{ .rows = 16,
               .cols = 2,
               .rs   = i2c.output(0),
               .rw   = i2c.output(1),
               .e    = i2c.output(2),
               .led  = i2c.output(3),
               .db4  = i2c.output(4),
               .db5  = i2c.output(5),
               .db6  = i2c.output(6),
               .db7  = i2c.output(7)  }};

    std::ostringstream oss;
    while (true) {
        auto dist = ranger.getDistance();
        oss << std::fixed << std::setprecision(1) << dist * 100.0 << " cm";
        lcd.write(oss.str());
        lightPin = dist < 0.15;
        sleep_for(1s);
        lcd.clear();
        oss.str("");
    }
}
