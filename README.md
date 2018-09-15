C-PythonBinding
=============
从kbengine中提取出来python绑定到C++模块，实现python、C++相互调用 已经导出C++类和对象到python模块中
-------------

##### kbengine中的 pyscript库
      KBEngine 引擎使用了流 行的python脚本用作上层逻辑处理，
      kbe作者对python了如指掌，实现了功能强大的pyscript层，开发者只需继承ScriptObject类，
	  并且灵活使用一些宏扩展，便可以写出各种功能用途的类和对象导出到python模块里面使用(python类也可继承C++类)。
      KBEngine中 entity、Proxy、Vector234、MemoryStream 都是继承该类。
	  python模块可以继承entity类。
      ScriptObject中核心是BASE_SCRIPT_HREADER(CLASS, SUPERCLASS)这个宏，
	  该宏主要定义PyTypeObject所需的PyMethodDef、PyMemberDef、PyGetSetDef、tp_init、tp_alloc、tp_new、tp_free。
	  而installScript函数正式组合导出类所有函数属性成员。

##### 继承C++类的python对象的生成。
      生成一个python对象既可以在python中生成，也可以在C++中生产python 对象，
	  在python中生成对象跟其他普通python对象没什么区别，只是要实现PyTypeObject的tp_alloc。
       在C++中生产一个python对象 有两个关键步骤：
	   
	   1.  PyObject * pObject = PyType_GenericAlloc((PyTypeObject*)pyClass, 0);   
	   2.   Unit* pUnit = new (pObject) Unit(id);
	   
      PyType_GenericAlloc使用python接口创建一个python对象内存空间，并且refcnt=1，
	  大小什么的在PyTypeObject里面描述了，因为python是C语言实现，所以C++对象构造函数并没有执行，
	  第二个步骤就是在某个内存中执行C++类的构造函数。
	  (这个思路 其实在暗黑战神服务器中也有类似步骤，
	  在lua中申请空间， 在C++中执行构造，这样来看其实python、lua在大的方向上其实并无二异)

##### C-PythonBinding 
      C-PythonBinding 仅仅是使用pyscript的一个测试demo 实现自定义一个C++类 Unit，导出到python中，可供python类继承。
      C++实现的Unit类，导出到python中让python模块继承
 
![python c](https://github.com/MirLegend/C-PythonBinding/blob/master/doc/1.png)
![python c](https://github.com/MirLegend/C-PythonBinding/blob/master/doc/2.png)

      python模块继承C++实现的Unit类
	  
![python c object](https://github.com/MirLegend/C-PythonBinding/blob/master/doc/3.png)