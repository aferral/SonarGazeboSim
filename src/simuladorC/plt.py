import pylab as plt
import numpy as np

with open('outSensor.txt','r') as f:
    rw=f.read()
lines = rw.split('\n')
lines = lines[:-1]
vals = map(float,lines)

t=np.array([1000*15e-6*i for i in range(len(vals))])
plt.plot(t,vals);
plt.show()
