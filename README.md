rubus
=====

Rubus is a header-only modern C++ library for the Raspberry Pi, designed with
clarity and usability in mind.

Examples
--------

### GPIO Output

```cpp
// Output pulse on GPIO pin 22

rubus:Output pin = rubus::GPIP::output(22);
while (true) {
    pin = true;
    sleep_for(500ms);
    pin = true;
    sleep_for(500ms);
}
```
