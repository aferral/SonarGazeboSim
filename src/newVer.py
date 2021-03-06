import numpy as np
import matplotlib.pyplot as plt
import numpy as np
from skimage.io import imsave
from skimage.morphology import dilation
import cv2
import json
import base64
import os
import subprocess
import os
#define constants

#All vars are in meters
from gridUtils import rectangularize

wallWidth = 0.4

distLine = 2
widthPoint = 2

gridCells =  200

pixelPerMeter = 0.2 #how many [meteres/pixel]


pixelWallWidth = int(wallWidth / pixelPerMeter)
simFolder = 'simuladorC'


def outTxt(d,image):
	with open(os.path.join(simFolder,"out.txt"),'w') as f:
		f.write(str(d['gridSize'])+",")
		f.write(str(d['org'][0])+",")
		f.write(str(d['org'][1])+",")
		f.write(str(d['sensor'][0])+",")
		f.write(str(d['sensor'][1])+",")
		temp = np.where(image)
		f.write(str(len(temp[0])+1)+";")

		cordXs = temp[0]
		cordYs = temp[1]

		cordsString = ""
		for i in range(len(cordXs)):
			cordsString += (str(cordXs[i])+","+str(cordYs[i])+";")
		f.write(cordsString[0:-1])
	
	pass

class recta:
	def __init__(self,p0,p1):
		self.p0 = p0
		self.p1 = p1
	def transformTopixel(self,dx,dy):
		self.p0 = (int(self.p0[0] / dx ), int(self.p0[1] / dy ) )
		self.p1 = (int(self.p1[0] / dx ), int(self.p1[1] / dy ) )
	def translateOr(self,newOr):
		self.p0 = (self.p0[0] - newOr[0], self.p0[1] - newOr[1] )
		self.p1 = (self.p1[0] - newOr[0], self.p1[1] - newOr[1] )
		pass

	def drawOnImage(self,image): #ALL POINTS SHOULD HAVE IMAGE CORDINATES (all positive 0,0 at down left )
		cv2.line(image,self.p0,self.p1,(255,0,0),pixelWallWidth)
		

def getDistance(p0,p1):
	return np.sqrt((p0[0]-p1[0])**2 + (p0[1]-p1[1])**2)

def createSimGrid(msg):

	debug = False
	maxRange = msg.range_max 

	#This square are the limits of the simulation
	#the laser is in the center, the laser points are in a 360 circle around it with radius maxRange
	width = 1.05 * maxRange * 2
	height = width


	canvas = np.zeros((gridCells,gridCells))
	halfGrid = int(gridCells * 0.5)


	dist = list(msg.ranges)
	da = (msg.angle_max - msg.angle_min) / (len(dist) - 1)
	angles = [msg.angle_min  + (da* i) for i in range(len(dist))]


	xP = [dist[i]*np.cos(angles[i]) for i in range(len(angles))]
	yP = [dist[i]*np.sin(angles[i]) for i in range(len(angles))]

	#Filter points in maxRange dist (those are the laser finding nothing)
	xP = filter(lambda x : x[0] < maxRange, [(dist[i],xP[i]) for i in range(len(xP))])
	yP = filter(lambda x: x[0] < maxRange, [(dist[i], yP[i]) for i in range(len(yP))])
	xP = map(lambda x : x[1], xP)
	yP = map(lambda x: x[1], yP)

	

	#Now maxRange is the max manhattan distancae between two points * 0.5
	print "Old max range ",maxRange
	allCombinationsX = [(i,j) for i in xP for j in xP] 
	maxRx = max([abs(elem[0] - elem[1] ) for elem in allCombinationsX] ) * 0.5
	allCombinationsY = [(i,j) for i in xP for j in yP]
	maxRy = max([abs(elem[0] - elem[1] ) for elem in allCombinationsY] ) * 0.5

	maxRange = max(1.1*maxRy,1.1*maxRx)

	print "NEW max range ",maxRange

	if debug:
		plt.plot(xP,yP)
		plt.show()

	rectList = []

	#Check consecutive points
	for i in range(0,len(xP)-1):
		p1 = (xP[i],yP[i])
		p2 = (xP[i+1],yP[i+1])
		
		distbetw = getDistance(p1,p2)
		if  distbetw < distLine: #if they are close enought make them a line
			rectList.append(recta(p1,p2)) 
		else:
			print "Warning isolated points at ",i," ",i+1," distance ",distbetw
		#else they are a point ??? (should i make them a little box ??)
	
	#simplify lines if they have similar slope

	#

	dx = maxRange / halfGrid
	dy = maxRange / halfGrid

	#draw the remaining lines
	for elem in rectList:
		elem.transformTopixel(dx,dy)
	minX = min([min(rect.p0[0],rect.p1[0]) for rect in rectList])
	minY = min([min(rect.p0[1],rect.p1[1]) for rect in rectList])
	print "minX : ",minX," minY : ",minY

	nuevoOr = (minX - 5, minY - 5)
	
	for elem in rectList:
		elem.translateOr(nuevoOr) #The new origin is the down left corner
		elem.drawOnImage(canvas)

	print "New origin at ",(-minX + 5)," ",(-minY + 5)," IMAGE CORDS"
	outOr = ((-minX + 5),(-minY + 5))

	cv2.imwrite("outCanvas"+".png", canvas)


	#Now export all this info to json for simulation (MATLAB)
	rectangularize(canvas,outOr)
	encoded = base64.b64encode(open('out.png','rb').read())

	d = {}
	d['image'] = encoded
	d['org'] = outOr
	d['sensor'] = (outOr[0] + 3 , outOr[1])
	d['gridSize'] = gridCells

	with open('out.json','w') as f:
		json.dump(d,f)

	#Export to txt (requiered to use C simulation)
	outTxt(d,canvas)

	#Call the C program with show
	st = './'+os.path.join(simFolder,'out')
	print st
	sleep = subprocess.Popen([st])
	sleep.wait() #Wait it for end
	print sleep.stdout  # sleep outputs nothing but...
	rt= sleep.returncode  # you get the exit value
	
	if (rt == 0): #Plot sensor readings
		with open(os.path.join(simFolder,'outSensor.txt'),'r') as f:
			fullText = f.read()
		ns = fullText.split()
		vals = map(float,ns)
		plt.plot(vals)
		plt.savefig('senalSensor.png')
	
