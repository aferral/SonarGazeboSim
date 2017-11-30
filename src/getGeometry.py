#!/usr/bin/env python

from sonar_sim.srv import *
from gazebo_msgs.msg import ModelStates
from sensor_msgs.msg import LaserScan
import rospy
from newVer import createSimGrid
import numpy as np
import os
import rospkg
#todo cambiar topico por servicio get state



class nodeSensorAdapter:
	def __init__(self):
		rospack = rospkg.RosPack()
		a=rospack.get_path('sonar_sim')
		os.chdir(os.path.join(a,'src'))
		print os.getcwd()
		rospy.init_node('getGeo_server')
		s = rospy.Service('getGeo', getGeo, self.handle_add_two_ints)
		#self.sub = rospy.Subscriber('gazebo/model_states', ModelStates, self.handlePose)
		self.laser = rospy.Subscriber('/rrbot/laser/scan', LaserScan, self.handleLaser)
		self.modelName = 'rrbot'
		self.lastMessage = None
		self.lastSensorR = None
		print "Server getGeo started"
		rospy.spin()

	def handle_add_two_ints(self,req):
		#Here get request
		createSimGrid(self.lastSensorR ) 

		#Get geometry representation
		

		return 2
	def handleLaser(self,msg):
		self.lastSensorR = msg
		

	def handlePose(self,msg):
		#do processing here
		debug = False
		try:
			whereIs = msg.name.index(self.modelName)
			if debug:
				print "Pose of sensor "
				print msg.pose[whereIs]
			self.lastMessage = msg.pose[whereIs]
		except ValueError:
			print "Sensor object not found "



if __name__ == "__main__":
	newN = nodeSensorAdapter()

