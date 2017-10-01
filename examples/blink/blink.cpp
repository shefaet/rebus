#include <rubus/rubus.hpp>

#include <chrono>
#include <iostream>
#include <string>

#include <unistd.h>

using namespace rubus;
using namespace std::chrono_literals;

void getOptions(int *p, int argc, char **argv)
{
    int opt;
    while ((opt = ::getopt(argc, argv, "p:h")) != -1) {
        auto usage = "Usage: blink -p PIN_NUM\n";
        switch (opt) {
            case 'p': *p = std::stoi(optarg); break;
            case 'h':
            case '?': std::cerr << usage; std::exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv)
{
    int p = 5;
    getOptions(&p, argc, argv);
    auto pin = GPIO::output(p, false);
    while (true) {
        pin.set(true,  250ms);
        pin.set(false, 250ms);
    }
}
