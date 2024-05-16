import numpy as np
import queue

data = queue.Queue()
array1 = [[[i+k for i in range(7)] for j in range(7)] for k in range(100)]
for i in range(100):
    data.put(np.array(array1[i]).astype(np.uint8))


np.save('recorded_signals.npy', np.array(list(data.queue)))

