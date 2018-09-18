import KBEngine

class Monster(KBEngine.Unit):
	def __init__(self):
		KBEngine.Unit.__init__(self)
		print("Monster::init id:%i"%(self.id))
		print("Monster::setViewRadius :%i"%(self.setViewRadius(500)))
		self.printRefCnt()
		
	#def setViewRadius(self, type):
	#	return 100

def onAppInit(isBootstrap, str):
	"""
	KBEngine method.
	baseapp已经准备好了
	@param isBootstrap: 是否为第一个启动的baseapp
	@type isBootstrap: BOOL
	"""
	print('onBaseAppReady: isBootstrap=%s, str=%s' % \
	 (isBootstrap, str))
