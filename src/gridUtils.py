import numpy as np
import matplotlib.pyplot as plt
import numpy as np
from skimage.io import imsave
from skimage.morphology import dilation
#define constants

#All vars are in meters

#This are meters
wallWidth = 0.2

distLine = 2
widthPoint = 2

gridCells =  200


def dist(p0,p1):
	return 2

def createSimGrid(msg):

	debug = True
	maxRange = msg.range_max 

	#This square are the limits of the simulation
	#the laser is in the center, the laser points are in a 360 circle around it with radius maxRange
	width = 1.05 * maxRange * 2
	height = width

	p0 = 0
	previousPoint = p0

	canvas = np.zeros((gridCells,gridCells))
	halfGrid = gridCells * 0.5



	dist = list(msg.ranges)
	da = (msg.angle_max - msg.angle_min) / (len(dist) - 1)
	angles = [msg.angle_min  + (da* i) for i in range(len(dist))]


	xP = [dist[i]*np.cos(angles[i]) for i in range(len(angles))]
	yP = [dist[i]*np.sin(angles[i]) for i in range(len(angles))]

	#Elimina puntos en maximo rango
	xP = filter(lambda x : x[0] < maxRange, [(dist[i],xP[i]) for i in range(len(xP))])
	yP = filter(lambda x: x[0] < maxRange, [(dist[i], yP[i]) for i in range(len(yP))])
	xP = map(lambda x : x[1], xP)
	yP = map(lambda x: x[1], yP)

	#Cambia el maximo rango a distancia maxima en puntos restantes
	maxRange = max(np.sqrt(p[0]**2 + p[1]**2) for p in zip(xP,yP))
	dx = maxRange / halfGrid

	if debug:
		plt.plot(xP,yP)
		plt.show()

	#transform to grid points
	gx = map(lambda x : int(x / dx), xP)
	gy = map(lambda y : int(y / dx), yP)

	#translate to image cordinate (where 0,0 is top corner)
	igx = map(lambda x : x+halfGrid,gx)
	igy = map(lambda y : y+halfGrid,gy)

	#paint canvas
	canvas[igx,igy] = 1

	#Add sensor pos in 0,0

	plt.imshow(canvas)
	plt.show()



	#Recortar parte importante

	#aumentar resolucion

	#Dilatar o reparar

	canvas2 = dilation(canvas)


	imsave('canvas2.png',canvas2)
	imsave('canvas.png', canvas)

	print "hi"
	return 2

	#Exportar geoemtrica + lugar sensor + lugar fuente.



	pass

#exportar to formato simMatlab
#Se define una grilla y las paredes son una mascara
#el sensor tambien es un punto en la grilla
#al igual que la fuente

#exportar a simulacion c++
#requiere todo en cordenadas x,y 
#se definen rectangulos para la geometria con x0,y0,z0 width,height, zwidth
#La fuente se coloca en cordenadas (en este caso seria 0,0) ya que cordenadas centradas en centro de caja

#show image
if __name__ == "__main__":
	import pickle
	with open('out.pkl','rb') as f:
		msg = pickle.load(f)

	createSimGrid(msg)
	pass