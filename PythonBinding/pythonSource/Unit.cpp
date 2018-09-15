// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#include "pyscript/py_gc.h"
#include "Unit.h"
//#include "helper/eventhistory_stats.h"
//#include "math/math.h"

namespace KBEngine{

//-------------------------------------------------------------------------------------
SCRIPT_METHOD_DECLARE_BEGIN(Unit)
SCRIPT_METHOD_DECLARE("setViewRadius",				pySetViewRadius,				METH_VARARGS,				0)
SCRIPT_METHOD_DECLARE("printRefCnt", pyprintRefCnt, METH_VARARGS, 0)
SCRIPT_METHOD_DECLARE_END()

SCRIPT_MEMBER_DECLARE_BEGIN(Unit)
SCRIPT_MEMBER_DECLARE_END()

SCRIPT_GETSET_DECLARE_BEGIN(Unit)
SCRIPT_GET_DECLARE("id", pyGetID, 0, 0)
SCRIPT_GETSET_DECLARE_END()

BASE_SCRIPT_INIT(Unit, 0, 0, 0, 0, 0)

//-------------------------------------------------------------------------------------
int32 Unit::setViewRadius(int type)
{
	if (type >5)
	{
		return 600;
	}
	else
	{
		return 1;
	}
}

//-------------------------------------------------------------------------------------
PyObject* Unit::pySetViewRadius(int type)
{
	return PyLong_FromLong(setViewRadius(type));
}


//-------------------------------------------------------------------------------------
PyObject* Unit::pyprintRefCnt()
{
	S_Return;
}

//-------------------------------------------------------------------------------------
Unit::Unit(ENTITY_ID id,
	PyTypeObject* pyType, bool isInitialised) :
ScriptObject(pyType, isInitialised),
id_(id)
{
	script::PyGC::incTracing("Unit");
}

//-------------------------------------------------------------------------------------
Unit::~Unit()
{
	script::PyGC::decTracing("Unit");
}	

////-------------------------------------------------------------------------------------
//void Unit::onInitializeScript()
//{
//
//}

//-------------------------------------------------------------------------------------
void Unit::onDestroy(bool callScript)
{
	
}

//-------------------------------------------------------------------------------------
}
