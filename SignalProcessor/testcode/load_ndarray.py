import numpy as np


recored_signals = np.load('recorded_signals.npy').astype(np.uint8)

for signals in recored_signals:
    print(signals)
    print(signals.shape)
    print('--------------------------------------')