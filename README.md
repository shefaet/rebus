rubus
=====

Rubus is a header-only C++ library for the Raspberry Pi 3, designed with clarity
and usability in mind.

Examples
--------

### GPIO Output

```cpp
// Output pulse on pin 22

rubus::gpio::Output pin(22);
while (true) {
    pin = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    pin = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
```

### GPIO Input — Polling

```cpp
// Turn on pin 22 when pin 21 input is high

rubus::gpio::Output output(22);
rubus::gpio::Input input(21);
while (true) {
    if (input && !output) {
        output.set(true);
    } else if (!input && output) {
        input.set(true);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
```
