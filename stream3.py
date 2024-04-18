import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import serial
import numpy as np

# 初始化串口连接
ser = serial.Serial('/dev/cu.usbmodem2101', 115200, timeout=0.01)  # 设置较短的超时时间

# 初始化绘图
fig, ax = plt.subplots()
x_data, y_data = [], []
line, = plt.plot([], [], '-')
buffer_size = 2000  # 每次更新图表时使用的数据点数量
display_size = buffer_size * 5  # 显示的数据点数量
data_buffer = []

# 设置图表布局
def init():
    ax.set_xlim(0, display_size)  # 初始x轴范围设为display_size
    ax.set_ylim(0, 4095)  # 根据实际情况调整Y轴范围
    return line,

# 更新图表的函数
def update(frame):
    global data_buffer, x_data, y_data
    while ser.in_waiting > 0:
        raw_line = ser.readline()
        string_line = raw_line.decode('utf-8').rstrip()
        try:
            new_data = float(string_line)
            print(new_data)
            data_buffer.append(new_data)
        except ValueError:
            pass
    
    # 每次有新数据时，添加到y_data中，同时更新x_data
    y_data.extend(data_buffer)
    x_data.extend(range(len(y_data) - len(data_buffer), len(y_data)))
    data_buffer = []  # 清空data_buffer以接收新数据

    # 保持x_data和y_data的长度匹配display_size
    if len(y_data) > display_size:
        y_data = y_data[-display_size:]
        x_data = list(range(len(y_data)))

    line.set_data(x_data, y_data)
    ax.set_xlim(max(0, x_data[-1] - display_size), x_data[-1])

    return line,

# 创建动画
ani = FuncAnimation(fig, update, init_func=init, blit=True, interval=10)

try:
    plt.show()
finally:
    ser.close()  # 确保关闭串口连接