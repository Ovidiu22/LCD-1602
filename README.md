# LCD-1602

C library for interfacing a **16×2 character LCD (HD44780-compatible)** with AVR microcontrollers.

This project provides a **modular and reusable driver** for displaying text on LCD-1602 modules using low-level GPIO control.

---

## Overview

The LCD-1602 library simplifies working with character LCD displays by abstracting
the low-level command timing and data transfer required by the HD44780 controller.

The code is written with **clarity and portability** in mind and can be easily integrated
into embedded projects using ATmega microcontrollers.

---

## Features

- 4-bit LCD interface
- Low-level command and data handling
- Character and string display support
- Clear display and cursor positioning
- Modular design for easy reuse

---

## Hardware / Environment

- **Microcontroller:** ATmega328P  
- **Development environment:** Atmel Studio  
- **Display:** LCD 16×2 (HD44780-compatible)  

---

## Usage

1. Connect the LCD module to the microcontroller GPIO pins.
2. Configure the data and control pins in the source code.
3. Initialize the LCD using the provided initialization function.
4. Write characters or strings to the display.

> See source files for function definitions and usage examples.

---

## Status

✔️ **Stable** — tested on ATmega328P with LCD-1602 module.

---

## License

This project is open-source and intended for educational and embedded development use.

---

## Schematics
![grafik](https://user-images.githubusercontent.com/75970114/150550530-54911e2c-e799-4665-8328-cfe6ae02a8bb.png)
