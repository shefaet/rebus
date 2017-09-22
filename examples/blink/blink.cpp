#include <rubus/rubus.hpp>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include <unistd.h>

using namespace std::chrono;
using namespace std::this_thread;

void getOptions(int *p, int *d, int argc, char **argv)
{
    int opt;
    while ((opt = ::getopt(argc, argv, "p:d:")) != -1) {
        std::string usage = "Usage: blink -p PIN_NUM -d DELAY_MS\n";
        switch (opt) {
            case 'p': *p = std::atoi(optarg); break;
            case 'd': *d = std::atoi(optarg); break;
            case '?': std::cerr << usage; std::exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv)
{
    int p = 2, d = 500;
    getOptions(&p, &d, argc, argv);

    rubus::Output pin = rubus::GPIO::output(p, false);

    while (true) {
        pin = true;  sleep_for(milliseconds(d));
        pin = false; sleep_for(milliseconds(d));
    }
}
