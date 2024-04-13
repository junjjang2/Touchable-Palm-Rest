import blobDetector
import MTKReader
import cv2
import tkinter as tk
import threading
import numpy as np
import os
import queue

class Program:
    def __init__(self, rows, cols) -> None:
        self.blobDetector = blobDetector.BlobDetector(rows, cols)
        self.mtkReader = MTKReader.MultiTouchKitReader()
        self.isend = False

    def process(self, signal):
        blob_centers = self.blobDetector.process(signal)
        return blob_centers
    
    # Start Reading multi-touch data
    def start_with_serial_connection(self):
        print("Start with real-time signals")
        self.mtkReader.initialize()
        self.window = cv2.namedWindow('Blob Image', cv2.WINDOW_NORMAL)

        while True:
            if cv2.waitKey() == ord('q'): # q를 누르면 종료
                break
            if self.isend:
                break
            signals = self.mtkReader.read_signals()
            blob_centers = self.process(signals)
            print(blob_centers)
            cv2.imshow('Blob Image', self.blobDetector.image)

        # Close serial port
        self.mtkReader.close_serial()
        cv2.destroyAllWindows()

    def start_with_recorded(self):
        print("Start with recorded signals")
        self.window = cv2.namedWindow('Blob Image', cv2.WINDOW_NORMAL)

        # Load recorded signals
        if not os.path.isfile('recorded_signals.npy'):
            recored_signals = np.load('recorded_signals.npy')

        # Process recorded signals
        for signals in recored_signals:
            if self.isend:
                break
            blob_centers = self.process(signals)
            cv2.imshow('Blob Image', self.blobDetector.image)
        print('Done')

    def stop(self):
        self.isend = True

class SignalThread:
    def __init__(self, program, message_queue) -> None:
        self.program = program
        self.message_queue = message_queue

    def run(self):
        while True:
            if self.program.isend:
                break
            if not self.message_queue.empty():
                message = self.message_queue.get()
                if message == 'stop':
                    self.program.stop()
                    break
                elif message == 'start_with_serial':
                    self.program.start_with_serial_connection()        
                elif message == 'start_with_recorded':
                    self.program.start_with_recorded()
                elif message.startswith('record_data'):
                    self.program.mtkReader.isRecording = message.split(':')[1]
                    if self.program.mtkReader.isRecording:
                        self.program.mtkReader.record_signals = []


class GUI:
    def __init__(self):
        self.window = tk.Tk()
        self.window.title('Main Menu')
        self.window.geometry('400x400')

        # checkbox isRecording 
        self.isRecording = tk.BooleanVar()
        self.checkbox = tk.Checkbutton(self.window, text='Record Data', variable=self.isRecording)
        self.checkbox.pack()

        # Start with Serial button
        self.start_serial_button = tk.Button(self.window, text='Start With Serial')
        self.start_serial_button.pack()

        # Start with Recorded button
        self.start_recorded_button = tk.Button(self.window, text='Start With Recorded')
        self.start_recorded_button.pack()

        # Stop button
        self.stop_button = tk.Button(self.window, text='Stop All')
        self.stop_button.pack()

    def start(self):
        self.window.mainloop()

    def stop(self):
        pass
    

if __name__ == '__main__':
    gui = GUI()
    program = Program(7, 7)
    message_queue = queue.Queue()
    signal_thread = SignalThread(program, message_queue)
    threading.Thread(target=signal_thread.run).start()
    gui.checkbox.bind('<Button-1>', lambda e: message_queue.put("record_data:"+str(gui.isRecording)))
    gui.start_serial_button.bind('<Button-1>', lambda e: message_queue.put('start_with_serial'))
    gui.start_recorded_button.bind('<Button-1>', lambda e: message_queue.put('start_with_recorded'))
    gui.stop_button.bind('<Button-1>', lambda e: message_queue.put('stop'))
    gui.start()
    print('Done')