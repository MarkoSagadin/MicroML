import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import serial


# Setup image plot
fig, ax = plt.subplots(1,1)
ax.set_title('Camera output')

#data = np.random.rand(3,3)
data = np.zeros((60, 80))
data[0][0] = 255
im = plt.imshow(data, animated=True, cmap='plasma')
cbar = fig.colorbar(im)


# Open serial port
ser = serial.Serial(port='COM4', baudrate=921600)
rows = 60
def updatefig(*args):
    try:
        ser_bytes = ser.readline().strip().decode('utf-8')
        # print(ser_bytes)
        if ser_bytes == 'We are done':
            #print("START")
            #data = np.zeros((3, 3))
            row = 0
            while(True):
                ser_bytes = ser.readline().strip().decode('utf-8')
                nums = [int(n) for n in ser_bytes.split()]
                data[row] = nums
                row += 1
                if(row == rows):
                    im.set_array(data)
                    #im.set_clim(np.amin(data), np.amax(data))
                    return im,

        return im,

    except Exception as e:
        print("Error" +str(e))
        return im,

ani = animation.FuncAnimation(fig, updatefig, interval=1, blit=True)
plt.show()

