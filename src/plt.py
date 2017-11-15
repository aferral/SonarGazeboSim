import pylab as plt
import numpy as np

with open('outSensor.txt','r') as f:
    rw=f.read()
lines = rw.split('\n')
lines = lines[:-1]
vals = map(float,lines)

plt.plot(vals);
plt.show()
