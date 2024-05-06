import serial
import time
import os
from datetime import datetime

serial_port = '/dev/cu.usbmodem1101'  # Linux --- run "ls /dev/cu.*" in terminal to find the port
baud_rate = 115200
data = []  # 用于存储数据的列表

def main():
    with serial.Serial(serial_port, baud_rate, timeout=1) as ser:  # Adjust timeout as needed
        print(f"Connected to {serial_port}. Waiting for data...")
        out_file = None
        last_data_time = time.time()

        try:
            while True:
                line = ser.readline()
                if line:
                    if out_file is None:
                        # Data received, open a new file
                        current_time = datetime.now().strftime("%Y%m%d-%H%M%S")
                        out_file = open(f'data/output_{current_time}.txt', 'w')
                        print("Opened a new file.")
                    
                    # Write the received data to the file
                    decoded_line = line.decode('utf-8').strip()
                    out_file.write(decoded_line + '\n')
                    out_file.flush()
                    last_data_time = time.time()  # Update the last data receive time
                else:
                    # No data received
                    current_time = time.time()
                    if out_file and (current_time - last_data_time) > 1:  # Timeout, no data for 20 seconds
                        out_file.close()
                        out_file = None
                        print("Closed the file due to inactivity.")

        except KeyboardInterrupt:
            print("Stopped by user.")
            if out_file:
                out_file.close()

if __name__ == '__main__':
    main()