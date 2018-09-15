import KBEngine

class Monster(KBEngine.Unit):
	def __init__(self):
		KBEngine.Unit.__init__(self)
		print("Monster::init id:%i"%(self.id))
		print("Monster::setViewRadius :%i"%(self.setViewRadius(500)))
		self.printRefCnt()
		
		
	def onClientEnabled(self):
		"""
		KBEngine method.
		该entity被正式激活为可使用， 此时entity已经建立了client对应实体， 可以在此创建它的
		cell部分。
		"""
		#INFO_MSG("account[%i] entities enable. entityCall:%s" % (self.id, self.client))
		print ("hello my python world!")
		
	#def setViewRadius(self, type):
	#	return 100

#def hello():
#	print ("hello my python world!")
#	print ("KBEngine com is %s!" % (KBEngine.component))

#hello()

def onAppInit(isBootstrap, str):
	"""
	KBEngine method.
	baseapp已经准备好了
	@param isBootstrap: 是否为第一个启动的baseapp
	@type isBootstrap: BOOL
	"""
	print('onBaseAppReady: isBootstrap=%s, str=%s' % \
	 (isBootstrap, str))
	#monster2 = Monster(2)
