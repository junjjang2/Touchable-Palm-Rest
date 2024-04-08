#  @brief  Read multi-touch data from sensor and write it to Serial
#  @details Serial output format:
#  7 x 7 matrix
#  <Tx>,<Rx0>,<Rx1>,<Rx2>,<Rx3>,<Rx4>,<Rx5>,<Rx6>,<Rx7>
#  0,<Rx0>,<Rx1>,<Rx2>,<Rx3>,<Rx4>,<Rx5>,<Rx6>,<Rx7>
#  1,<Rx0>,<Rx1>,<Rx2>,<Rx3>,<Rx4>,<Rx5>,<Rx6>,<Rx7>
#  ...
#  7,<Rx0>,<Rx1>,<Rx2>,<Rx3>,<Rx4>,<Rx5>,<Rx6>,<Rx7>
#  @return void
import serial
import numpy as np

class MultiTouchSensor:
    def __init__(self):
        self.array_size = 7
        self.sensor_array = np.zeros(shape=(self.array_size, self.array_size))
        self.threshold = 100
        self.frame_num = 0
        self.isEnd = False

    def initialize_board(self):
        self.ser.write('initialize\n')
        self.ser.write('%d %d'%(self.array_size, self.array_size) + '\n')
        self.ser.write('0,1,2,3,4,5,6,7\n')
        self.ser.write('true\n')
        self.ser.write('%d' % self.threshold + '\n')

    # Initialize sensor
    def initialize(self):
        self.ser = self.ser.open_serial(0)
        self.initialize_board()

    # Open serial port
    def open_serial(self, usb_port=0):
        return serial.Serial('/dev/ttyUSB%d'%usb_port, baudrate=115200)
    
    # Read multi-touch data from sensor and write it to Serial
    def read_signals(self):
        self.sensor_array.fill(0)
        for i in range(self.array_size):
            line = self.ser.readline().split(',')
            tx = line[0]
            rx = line[1:]
            self.sensor_array[tx] = rx
        self.write_signals()
        self.frame_num += 1
        
    # Write multi-touch data to Serial
    def write_signals(self):
        print("------Frame %d------" % self.frame_num)
        for i in range(self.array_size):
            print(str(i), end='')
            for j in range(self.array_size):
                print(',' + str(self.sensor_array[i][j]))
            print('\n')

    # Start Reading multi-touch data
    def start(self):
        self.initialize()
        while True:
            if self.isEnd:
                break
            self.read_signals()
        self.ser.close()

    def End(self):
        self.isEnd = True

if __name__ == '__main__':
    sensor = MultiTouchSensor()
    sensor.start()