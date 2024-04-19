# This repo is used for the ML part of the OmniSurface project

## Files
- `ESP32S3/serial_read`: the ESP32S3 code, which simply read the analog value from one pin and print it to the serial.
- `main.py`: uses Serial communication to read data from serial ports, record the data, and save it every 3 seconds.
- `data` folder: documents raw data readings from ESP32S3 under different interaction circumstances.
    - P.S. reversed sensor: the wires are on the back.
- `visualize.ipynb`: initial visualization of the raw data.
- `stream3.py`: the main data streaming visualization script.

## Circuit Setup
![circuit](./img/circuit_setup.png)
