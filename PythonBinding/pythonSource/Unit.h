// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com

#ifndef KBE_ENTITY_H
#define KBE_ENTITY_H
	
//#include "helper/profile.h"
#include "common/timer.h"
#include "common/common.h"
#include "common/smartpointer.h"
#include "helper/debug_helper.h"
//#include "pyscript/math.h"
#include "pyscript/scriptobject.h"
	
namespace KBEngine{
	class Unit;
//typedef SmartPointer<Unit> UnitPtr;
//typedef std::vector<UnitPtr> SPACE_ENTITIES;

class Unit : public script::ScriptObject
{
	/** 子类化 将一些py操作填充进派生类 */
	BASE_SCRIPT_HREADER(Unit, ScriptObject)

public:
	Unit(ENTITY_ID id,
		PyTypeObject* pyType = getScriptType(), bool isInitialised = true);
	~Unit();
	
	/** 
		销毁这个Uint 
	*/
	void onDestroy(bool callScript);

	int32 setViewRadius(int type);
	DECLARE_PY_MOTHOD_ARG1(pySetViewRadius, int);

	DECLARE_PY_MOTHOD_ARG0(pyprintRefCnt);

	INLINE ENTITY_ID id() const
	{
		return id_;	
	}
		INLINE void id(ENTITY_ID v)
	{
		id_ = v;
	}
	static PyObject* __pyget_pyGetID(Unit *self, void *closure)
	{
		return PyLong_FromLong(self->id());
	}

public:
	ENTITY_ID													id_;
};

}

#endif // KBE_ENTITY_H
