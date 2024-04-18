import matplotlib.pyplot as plt
import serial

# 初始化串口连接
ser = serial.Serial('/dev/cu.usbmodem2101', 115200, timeout=None)


# 读取串口数据并实时更新图表
try:
    while True:
        if ser.in_waiting > 0:
            raw_line = ser.readline()  # 读取串口数据
            string_line = raw_line.decode('utf-8').rstrip()  # 解码数据
            try:
                # 将数据转换为浮点数并更新图表
                new_data = float(string_line)
                print(new_data)
            except ValueError:
                # 如果接收到的数据不能转换成浮点数，则忽略
                pass
finally:
    ser.close()  # 关闭串口连接
    plt.close()  # 关闭matplotlib图表窗口