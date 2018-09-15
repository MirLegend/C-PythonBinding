// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


#include "math.h"
namespace KBEngine{ namespace script{ namespace math {

//-------------------------------------------------------------------------------------
bool installModule(const char* moduleName)
{
	// 初始化一个数学相关的模块
	PyObject *mathModule = PyImport_AddModule(moduleName);
	PyObject_SetAttrString(mathModule, "__doc__", PyUnicode_FromString("This module is created by KBEngine!"));
	return true;
}

//-------------------------------------------------------------------------------------
bool uninstallModule()
{
	return true;
}

}
}
}
