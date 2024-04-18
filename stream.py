import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# Set up the serial connection (adjust the COM port as needed)
serial_port = '/dev/cu.usbmodem2101'  # Linux
ser = serial.Serial(serial_port, 115200, timeout=1)

# Set up the plotting window
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
xs = []
ys = []

# This function is called periodically from FuncAnimation
def animate(i, xs, ys):
    # Read temperature (Celsius) from serial port
    line = ser.readline().decode('utf-8').strip()
    print(line)
    if line.isdigit():
        value = int(line)
        # Add x and y to lists
        xs.append(i)
        ys.append(value)

        # Limit x and y lists to 20 items
        xs = xs[-20:]
        ys = ys[-20:]

        # Draw x and y lists
        ax.clear()
        ax.plot(xs, ys)

        # Format plot
        plt.xticks(rotation=45, ha='right')
        plt.subplots_adjust(bottom=0.30)
        plt.title('Sensor Data over Time')
        plt.ylabel('Sensor Value')
        plt.ylim(1600, 3000)

# Set up plot to call animate() function periodically
ani = animation.FuncAnimation(fig, animate, fargs=(xs, ys), interval=0.05)
plt.show()
