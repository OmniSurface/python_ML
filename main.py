import serial
import time
import os

serial_port = '/dev/cu.usbmodem2101'  # Linux
data = []  # 用于存储数据的列表
start_time = time.time()  # 记录开始时间
save_interval = 3  # 每隔5秒保存一次数据

ser = serial.Serial(serial_port, 115200, timeout=1)

try:
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(line)
            data.append(line)

        current_time = time.time()
        if current_time - start_time >= save_interval:
            if data:
                duration = current_time - start_time  # 计算记录时长
                timestamp = int(time.time())  # 获取当前时间戳
                filename = f"data/{timestamp}_{int(duration)}s_data.txt"  # 文件名
                if not os.path.exists("data"):
                    os.makedirs("data")
                with open(filename, "w") as file:
                    file.write("\n".join(data))
                print(f"数据已保存到{filename}，记录了{duration:.2f}秒。")
                
                # 重置数据和开始时间
                data = []
                start_time = current_time
            else:
                print("no data to save")
                start_time = current_time
                
except KeyboardInterrupt:
    print("程序被用户中断")

finally:
    ser.close()
    print("串口已关闭")