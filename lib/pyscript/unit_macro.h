// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


#ifndef KBE_UNIT_MACRO_H
#define KBE_UNIT_MACRO_H

#include "common/common.h"
#include "server/callbackmgr.h"		

namespace KBEngine{
	
#define UNIT_METHOD_DECLARE_END()																									\
	SCRIPT_METHOD_DECLARE_END()																										\


#define UNIT_GETSET_DECLARE_BEGIN(CLASS)																							\
	SCRIPT_GETSET_DECLARE_BEGIN(CLASS)																								\


#define UNIT_GETSET_DECLARE_END()																									\
	SCRIPT_GETSET_DECLARE_END()																										\



/*
	debug info.
*/
#define CAN_DEBUG_CREATE_ENTITY
#ifdef CAN_DEBUG_CREATE_ENTITY
#define DEBUG_CREATE_UNIT_NAMESPACE																		\
		if(g_debugEntity)																					\
		{																									\
			wchar_t* PyUnicode_AsWideCharStringRet1 = PyUnicode_AsWideCharString(key, NULL);				\
			char* ccattr_DEBUG_CREATE_UNIT_NAMESPACE= strutil::wchar2char(PyUnicode_AsWideCharStringRet1);\
			PyObject* pytsval = PyObject_Str(value);														\
			wchar_t* cwpytsval = PyUnicode_AsWideCharString(pytsval, NULL);									\
			char* cccpytsval = strutil::wchar2char(cwpytsval);												\
			Py_DECREF(pytsval);																				\
			DEBUG_MSG(fmt::format("{}(refc={}, id={})::debug_createNamespace:add {}({}).\n",				\
												scriptName(),												\
												static_cast<PyObject*>(this)->ob_refcnt,					\
												this->id(),													\
																ccattr_DEBUG_CREATE_UNIT_NAMESPACE,		\
																cccpytsval));								\
			free(ccattr_DEBUG_CREATE_UNIT_NAMESPACE);														\
			PyMem_Free(PyUnicode_AsWideCharStringRet1);														\
			free(cccpytsval);																				\
			PyMem_Free(cwpytsval);																			\
		}																									\


#define DEBUG_OP_ATTRIBUTE(op, ccattr)																		\
		if(g_debugEntity)																					\
		{																									\
			wchar_t* PyUnicode_AsWideCharStringRet2 = PyUnicode_AsWideCharString(ccattr, NULL);				\
			char* ccattr_DEBUG_OP_ATTRIBUTE = strutil::wchar2char(PyUnicode_AsWideCharStringRet2);			\
			DEBUG_MSG(fmt::format("{}(refc={}, id={})::debug_op_attr:op={}, {}.\n",							\
												scriptName(),												\
												static_cast<PyObject*>(this)->ob_refcnt, this->id(),		\
															op, ccattr_DEBUG_OP_ATTRIBUTE));				\
			free(ccattr_DEBUG_OP_ATTRIBUTE);																\
			PyMem_Free(PyUnicode_AsWideCharStringRet2);														\
		}																									\


#define DEBUG_PERSISTENT_PROPERTY(op, ccattr)																\
	{																										\
		if(g_debugEntity)																					\
		{																									\
			DEBUG_MSG(fmt::format("{}(refc={}, id={})::debug_op_Persistent:op={}, {}.\n",					\
												scriptName(),												\
												static_cast<PyObject*>(this)->ob_refcnt, this->id(),		\
															op, ccattr));									\
		}																									\
	}																										\


#define DEBUG_REDUCE_EX(tentity)																			\
		if(g_debugEntity)																					\
		{																									\
			DEBUG_MSG(fmt::format("{}(refc={}, id={})::debug_reduct_ex: utype={}.\n",						\
												tentity->scriptName(),										\
												static_cast<PyObject*>(tentity)->ob_refcnt,					\
												tentity->id(),												\
												tentity->pScriptModule()->getUType()));						\
		}																									\


#else
	#define DEBUG_CREATE_UNIT_NAMESPACE			
	#define DEBUG_OP_ATTRIBUTE(op, ccattr)
	#define DEBUG_PERSISTENT_PROPERTY(op, ccattr)
	#define DEBUG_REDUCE_EX(tentity)
#endif


// 实体的标志
#define UNIT_FLAGS_UNKNOWN			0x00000000
#define UNIT_FLAGS_DESTROYING			0x00000001
#define UNIT_FLAGS_INITING			0x00000002
#define UNIT_FLAGS_TELEPORT_START		0x00000004
#define UNIT_FLAGS_TELEPORT_STOP		0x00000008

#define UNIT_HEADER(CLASS)																				\
public:																										\
	typedef KBEUnordered_map< std::string, std::vector<PyObjectPtr> > UNIT_EVENTS;						\
protected:																									\
	uint32														flags_;										\
public:																										\
																											\
	bool initing() const{ return hasFlags(UNIT_FLAGS_INITING); }											\
																											\
	void onInitializeScript();																				\
	void initializeScript()																					\
	{																										\
		removeFlags(UNIT_FLAGS_INITING);																	\
																											\
		if(PyObject_HasAttrString(this, "__init__"))														\
		{																									\
			PyObject* pyResult = PyObject_CallMethod(this, const_cast<char*>("__init__"),					\
											const_cast<char*>(""));											\
			if(pyResult != NULL)																			\
				Py_DECREF(pyResult);																		\
			else																							\
				SCRIPT_ERROR_CHECK();																		\
		}																									\
		onInitializeScript();																				\
	}																										\
																											\
	void initializeEntity(PyObject* dictData, bool persistentData = false)									\
	{																										\
		createNamespace(dictData, persistentData);															\
		initializeScript();																					\
	}																										\
	void createNamespace(PyObject* dictData, bool persistentData = false)									\
	{																										\
		if(dictData == NULL)																				\
			return;																							\
																											\
		if(!PyDict_Check(dictData)){																		\
			ERROR_MSG(fmt::format(#CLASS"::createNamespace: create"#CLASS"[{}:{}] "							\
				"args is not a dict.\n",																	\
				scriptName(), id_));																		\
			return;																							\
		}																									\
		SCRIPT_ERROR_CHECK();																				\
																											\
		Py_XDECREF(cellDataDict);																			\
	}																										\
																											\
	static PyObject* __py_reduce_ex__(PyObject* self, PyObject* protocol)									\
	{																										\
		CLASS* entity = static_cast<CLASS*>(self);															\
		DEBUG_REDUCE_EX(entity);																			\
		PyObject* args = PyTuple_New(2);																	\
		PyObject* unpickleMethod = script::Pickler::getUnpickleFunc("EntityCall");							\
		PyTuple_SET_ITEM(args, 0, unpickleMethod);															\
		PyObject* args1 = PyTuple_New(4);																	\
		PyTuple_SET_ITEM(args1, 0, PyLong_FromUnsignedLong(entity->id()));									\
		PyTuple_SET_ITEM(args1, 1, PyLong_FromUnsignedLongLong(g_componentID));								\
		PyTuple_SET_ITEM(args1, 2, PyLong_FromUnsignedLong(entity->pScriptModule()->getUType()));			\
		if(g_componentType == BASEAPP_TYPE)																	\
			PyTuple_SET_ITEM(args1, 3, PyLong_FromUnsignedLong(ENTITYCALL_TYPE_BASE));						\
		else																								\
			PyTuple_SET_ITEM(args1, 3, PyLong_FromUnsignedLong(ENTITYCALL_TYPE_CELL));						\
		PyTuple_SET_ITEM(args, 1, args1);																	\
																											\
		if(unpickleMethod == NULL){																			\
			Py_DECREF(args);																				\
			return NULL;																					\
		}																									\
		SCRIPT_ERROR_CHECK();																				\
		return args;																						\
	}																										\
	INLINE bool hasFlags(uint32 v) const																	\
	{																										\
		return (flags_ & v) > 0;																			\
	}																										\
																											\
	INLINE uint32 flags() const																				\
	{																										\
		return flags_;																						\
	}																										\
																											\
	INLINE void flags(uint32 v)																				\
	{																										\
		flags_ = v; 																						\
	}																										\
																											\
	INLINE uint32 addFlags(uint32 v)																		\
	{																										\
		flags_ |= v;																						\
		return flags_;																						\
	}																										\
																											\
	INLINE uint32 removeFlags(uint32 v)																		\
	{																										\
		flags_ &= ~v; 																						\
		return flags_;																						\
	}																										\
	int onScriptDelAttribute(PyObject* attr)																\
	{																										\
		wchar_t* PyUnicode_AsWideCharStringRet0 = PyUnicode_AsWideCharString(attr, NULL);					\
		char* ccattr = strutil::wchar2char(PyUnicode_AsWideCharStringRet0);									\
		PyMem_Free(PyUnicode_AsWideCharStringRet0);															\
		DEBUG_OP_ATTRIBUTE("del", attr)																		\
		if(pScriptModule_->findMethodDescription(ccattr, g_componentType) != NULL)							\
		{																									\
			char err[255];																					\
			kbe_snprintf(err, 255, "method[%s] defined in %s.def, del failed!", ccattr, scriptName());		\
			PyErr_SetString(PyExc_TypeError, err);															\
			PyErr_PrintEx(0);																				\
			free(ccattr);																					\
			return 0;																						\
		}																									\
																											\
		free(ccattr);																						\
		return ScriptObject::onScriptDelAttribute(attr);													\
	}																										\
																											\
	int onScriptSetAttribute(PyObject* attr, PyObject* value)												\
	{																										\
		DEBUG_OP_ATTRIBUTE("set", attr)																		\
		wchar_t* PyUnicode_AsWideCharStringRet0 = PyUnicode_AsWideCharString(attr, NULL);					\
		char* ccattr = strutil::wchar2char(PyUnicode_AsWideCharStringRet0);									\
		PyMem_Free(PyUnicode_AsWideCharStringRet0);															\
		free(ccattr);																						\
		return ScriptObject::onScriptSetAttribute(attr, value);												\
	}																										\
																											\
	PyObject * onScriptGetAttribute(PyObject* attr);														\
																											\
	DECLARE_PY_MOTHOD_ARG3(pyAddTimer, float, float, int32);												\
	DECLARE_PY_MOTHOD_ARG1(pyDelTimer, ScriptID);															\
	void writeToDB(void* data, void* extra1, void* extra2);													\
																											\
	void destroy(bool callScript = true)																	\
	{																										\
		if(hasFlags(UNIT_FLAGS_DESTROYING))																\
			return;																							\
																											\
		if(!isDestroyed_)																					\
		{																									\
			isDestroyed_ = true;																			\
			addFlags(UNIT_FLAGS_DESTROYING);																\
			EntityComponent::onEntityDestroy(this, pScriptModule_, callScript, true);						\
			onDestroy(callScript);																			\
			scriptTimers_.cancelAll();																		\
			removeFlags(UNIT_FLAGS_DESTROYING);															\
			EntityComponent::onEntityDestroy(this, pScriptModule_, callScript, false);						\
			events_.clear();																				\
			Py_DECREF(this);																				\
		}																									\
	}																										\
	INLINE bool isDestroyed() const																			\
	{																										\
		return isDestroyed_;																				\
	}																										\
	DECLARE_PY_GET_MOTHOD(pyGetIsDestroyed);																\
																											\
	void destroyEntity();																					\
	static PyObject* __py_pyDestroyEntity(PyObject* self, PyObject* args, PyObject * kwargs);				\
																											\
	DECLARE_PY_GET_MOTHOD(pyGetClassName);																	\
																											\
	void initProperty(bool isReload = false);																\
																											\
	static PyObject* __py_pyGetDatachangeEventPtr(PyObject* self, PyObject* args)							\
	{																										\
		CLASS* pobj = static_cast<CLASS*>(self);															\
		static EntityComponent::OnDataChangedEvent dataChangedEvent;										\
			dataChangedEvent = std::tr1::bind(&CLASS::onDefDataChanged, pobj,								\
			std::tr1::placeholders::_1, std::tr1::placeholders::_2, std::tr1::placeholders::_3);			\
		return PyLong_FromVoidPtr((void*)&dataChangedEvent);												\
	}																										\
																											\
	UNIT_EVENTS& events() { return events_; }																\
																											\
	bool registerEvent(const std::string& evnName, PyObject* pyCallback)									\
	{																										\
		UNIT_DESTROYED_CHECK(return false, "registerEvent", this);										\
																											\
		if (!PyCallable_Check(pyCallback))																	\
		{																									\
			PyErr_Format(PyExc_TypeError, "{}::registerEvent: '%.200s' object is not callable! eventName=%s, entityID={}",\
				scriptName(), (pyCallback ? pyCallback->ob_type->tp_name : "NULL"), evnName.c_str(), id());		\
			PyErr_PrintEx(0);																				\
			return false;																					\
		}																									\
																											\
		std::vector<PyObjectPtr>& evnVecs = events_[evnName];												\
		std::vector<PyObjectPtr>::iterator iter = evnVecs.begin();											\
		for(; iter != evnVecs.end(); ++iter)																\
		{																									\
			if((*iter).get() == pyCallback)																	\
			{																								\
				PyErr_Format(PyExc_TypeError, "{}::registerEvent: This callable('%.200s') has been registered! eventName=%s, entityID={}",\
					scriptName(), (pyCallback ? pyCallback->ob_type->tp_name : "NULL"), evnName.c_str(), id());	\
				PyErr_PrintEx(0);																			\
				return false;																				\
			}																								\
		}																									\
																											\
		events_[evnName].push_back(pyCallback);																\
        return true;                                                                                        \
	}																										\
																											\
	void fireEvent(const std::string& evnName, PyObject* pyArgs = NULL)										\
	{																										\
		UNIT_DESTROYED_CHECK(return, "fireEvent", this);													\
																											\
		std::vector<PyObjectPtr>& evnVecs = events_[evnName];												\
		std::vector<PyObjectPtr>::iterator iter = evnVecs.begin();											\
		for(; iter != evnVecs.end(); ++iter)																\
		{																									\
			PyObject* pyResult = NULL;																		\
			if (pyArgs == NULL)																				\
			{																								\
				pyResult = PyObject_CallObject((*iter).get(), NULL);										\
			}																								\
			else																							\
			{																								\
				pyResult = PyObject_CallObject((*iter).get(), pyArgs);										\
			}																								\
																											\
			if(pyResult == NULL)																			\
			{																								\
				SCRIPT_ERROR_CHECK();																		\
			}																								\
			else																							\
			{																								\
				Py_DECREF(pyResult);																		\
			}																								\
		}																									\
	}																										\
																											\
	bool deregisterEvent(const std::string& evnName, PyObject* pyCallback)									\
	{																										\
		std::vector<PyObjectPtr>& evnVecs = events_[evnName];												\
		std::vector<PyObjectPtr>::iterator iter = evnVecs.begin();											\
		for(; iter != evnVecs.end(); ++iter)																\
		{																									\
			if((*iter).get() == pyCallback)																	\
			{																								\
				evnVecs.erase(iter);																		\
				return true;																				\
			}																								\
		}																									\
																											\
		return false;																						\
	}																										\
																											\
    static PyObject* __py_pyRegisterEvent(PyObject* self, PyObject* args)									\
	{																										\
		uint16 currargsSize = (uint16)PyTuple_Size(args);													\
		CLASS* pobj = static_cast<CLASS*>(self);															\
																											\
		if(currargsSize != 2)																				\
		{																									\
			PyErr_Format(PyExc_AssertionError,																\
							"%s::registerEvent: args require %d args, gived %d!\n",							\
							pobj->scriptName(), 1, currargsSize);											\
																											\
			PyErr_PrintEx(0);																				\
			Py_RETURN_FALSE;																				\
		}																									\
																											\
		const char* eventName = NULL;																		\
		PyObject* pycallback = NULL;																		\
		if(PyArg_ParseTuple(args, "sO", &eventName, &pycallback) == -1)										\
		{																									\
			PyErr_Format(PyExc_AssertionError, "%s::registerEvent:: args error!", pobj->scriptName());		\
			PyErr_PrintEx(0);																				\
			pycallback = NULL;																				\
			Py_RETURN_FALSE;																				\
		}																									\
																											\
		if(!eventName)																						\
		{																									\
			PyErr_Format(PyExc_AssertionError, "%s::registerEvent:: eventName error!", pobj->scriptName());	\
			PyErr_PrintEx(0);																				\
			pycallback = NULL;																				\
			Py_RETURN_FALSE;																				\
		}																									\
																											\
		return PyBool_FromLong(pobj->registerEvent(eventName, pycallback));									\
	}																										\
																											\
    static PyObject* __py_pyDeregisterEvent(PyObject* self, PyObject* args)									\
	{																										\
		uint16 currargsSize = (uint16)PyTuple_Size(args);													\
		CLASS* pobj = static_cast<CLASS*>(self);															\
																											\
		if(currargsSize != 2)																				\
		{																									\
			PyErr_Format(PyExc_AssertionError,																\
							"%s::deregisterEvent: args require %d args, gived %d!\n",						\
							pobj->scriptName(), 1, currargsSize);											\
																											\
			PyErr_PrintEx(0);																				\
			Py_RETURN_FALSE;																				\
		}																									\
																											\
		const char* eventName = NULL;																		\
		PyObject* pycallback = NULL;																		\
		if(PyArg_ParseTuple(args, "sO", &eventName, &pycallback) == -1)										\
		{																									\
			PyErr_Format(PyExc_AssertionError, "%s::deregisterEvent:: args error!", pobj->scriptName());	\
			PyErr_PrintEx(0);																				\
			pycallback = NULL;																				\
			Py_RETURN_FALSE;																				\
		}																									\
																											\
		if(!eventName)																						\
		{																									\
			PyErr_Format(PyExc_AssertionError, "%s::deregisterEvent:: eventName error!", pobj->scriptName());\
			PyErr_PrintEx(0);																				\
			pycallback = NULL;																				\
			Py_RETURN_FALSE;																				\
		}																									\
																											\
		return PyBool_FromLong(pobj->deregisterEvent(eventName, pycallback));								\
	}																										\
																											\
    static PyObject* __py_pyFireEvent(PyObject* self, PyObject* args)										\
	{																										\
		uint16 currargsSize = (uint16)PyTuple_Size(args);													\
		CLASS* pobj = static_cast<CLASS*>(self);															\
																											\
		if(currargsSize == 0)																				\
		{																									\
			PyErr_Format(PyExc_AssertionError,																\
							"%s::fireEvent: args require %d args, gived %d!\n",								\
							pobj->scriptName(), 1, currargsSize);											\
																											\
			PyErr_PrintEx(0);																				\
			Py_RETURN_FALSE;																				\
		}																									\
																											\
		char* eventName = NULL;																				\
		if(currargsSize == 1)																				\
		{																									\
			if(PyArg_ParseTuple(args, "s", &eventName) == -1)												\
			{																								\
				PyErr_Format(PyExc_AssertionError, "%s::fireEvent:: args error! entityID={}", pobj->scriptName(), pobj->id());		\
				PyErr_PrintEx(0);																			\
				Py_RETURN_FALSE;																			\
			}																								\
																											\
			if(!eventName)																					\
			{																								\
				PyErr_Format(PyExc_AssertionError, "%s::fireEvent:: eventName error!", pobj->scriptName());	\
				PyErr_PrintEx(0);																			\
				Py_RETURN_FALSE;																			\
			}																								\
																											\
			pobj->fireEvent(eventName);																		\
		}																									\
		else if(currargsSize == 2)																			\
		{																									\
			PyObject* pyobj = NULL;																			\
			if (PyArg_ParseTuple(args, "sO", &eventName, &pyobj) == -1)										\
			{																								\
				PyErr_Format(PyExc_AssertionError, "%s::fireEvent:: args error! entityID={}", pobj->scriptName(), pobj->id());		\
				PyErr_PrintEx(0);																			\
				Py_RETURN_FALSE;																			\
			}																								\
																											\
			if(!eventName)																					\
			{																								\
				PyErr_Format(PyExc_AssertionError, "%s::fireEvent:: eventName error!", pobj->scriptName());	\
				PyErr_PrintEx(0);																			\
				Py_RETURN_FALSE;																			\
			}																								\
																											\
			PyObject* pyargs = PyTuple_New(1);																\
			Py_INCREF(pyobj);																				\
			PyTuple_SET_ITEM(pyargs, 0, pyobj);																\
			pobj->fireEvent(eventName, pyargs);																\
			Py_DECREF(pyargs);																				\
		}																									\
		else																								\
		{																									\
			PyObject* pyEvnName = PyTuple_GET_ITEM(args, 0);												\
																											\
			if(!PyUnicode_Check(pyEvnName))																	\
			{																								\
				PyErr_Format(PyExc_AssertionError, "%s::fireEvent:: eventName error!", pobj->scriptName());	\
				PyErr_PrintEx(0);																			\
				Py_RETURN_FALSE;																			\
			}																								\
																											\
			wchar_t* PyUnicode_AsWideCharStringRet0 = PyUnicode_AsWideCharString(pyEvnName, NULL);			\
			eventName = strutil::wchar2char(PyUnicode_AsWideCharStringRet0);								\
			PyMem_Free(PyUnicode_AsWideCharStringRet0);														\
																											\
			PyObject* pyargs = PyTuple_GetSlice(args, 1, currargsSize);										\
			pobj->fireEvent(eventName, pyargs);																\
			Py_DECREF(pyargs);																				\
			free(eventName);																				\
		}																									\
																											\
		Py_RETURN_TRUE;																						\
	}																										\
																											\
	PyObject* pyGetComponent(const std::string& componentName, bool all)									\
	{																										\
		std::vector<EntityComponent*> founds =																\
			EntityComponent::getComponents(componentName, this, pScriptModule_);							\
																											\
		if (!all)																							\
		{																									\
			if(founds.size() > 0)																			\
				return founds[0];																			\
																											\
			Py_RETURN_NONE;																					\
		}																									\
		else																								\
		{																									\
			PyObject* pyObj = PyTuple_New(founds.size());													\
																											\
			for (int i = 0; i < (int)founds.size(); ++i)													\
			{																								\
				PyTuple_SetItem(pyObj, i, founds[i]);														\
			}																								\
																											\
			return pyObj;																					\
		}																									\
																											\
        return NULL;																						\
	}																										\
																											\
    static PyObject* __py_pyGetComponent(PyObject* self, PyObject* args)									\
	{																										\
		uint16 currargsSize = (uint16)PyTuple_Size(args);													\
		CLASS* pobj = static_cast<CLASS*>(self);															\
																											\
		if(currargsSize == 0 || currargsSize > 2)															\
		{																									\
			PyErr_Format(PyExc_AssertionError,																\
							"%s::getComponent: args require 1-2 args, gived %d!\n",							\
							pobj->scriptName(), currargsSize);												\
																											\
			PyErr_PrintEx(0);																				\
			Py_RETURN_NONE;																					\
		}																									\
																											\
		char* componentName = NULL;																			\
		if(currargsSize == 1)																				\
		{																									\
			if(PyArg_ParseTuple(args, "s", &componentName) == -1)											\
			{																								\
				PyErr_Format(PyExc_AssertionError, "%s::getComponent:: args error!", pobj->scriptName());	\
				PyErr_PrintEx(0);																			\
				Py_RETURN_NONE;																				\
			}																								\
																											\
			if(!componentName)																				\
			{																								\
				PyErr_Format(PyExc_AssertionError, "%s::getComponent:: componentName error!", pobj->scriptName());\
				PyErr_PrintEx(0);																			\
				Py_RETURN_NONE;																				\
			}																								\
																											\
			return pobj->pyGetComponent(componentName, false);												\
		}																									\
		else if(currargsSize == 2)																			\
		{																									\
			PyObject* pyobj = NULL;																			\
			if (PyArg_ParseTuple(args, "sO", &componentName, &pyobj) == -1)									\
			{																								\
				PyErr_Format(PyExc_AssertionError, "%s::getComponent:: args error!", pobj->scriptName());	\
				PyErr_PrintEx(0);																			\
				Py_RETURN_NONE;																				\
			}																								\
																											\
			if(!componentName)																				\
			{																								\
				PyErr_Format(PyExc_AssertionError, "%s::getComponent:: componentName error!", pobj->scriptName());\
				PyErr_PrintEx(0);																			\
				Py_RETURN_NONE;																				\
			}																								\
																											\
			return pobj->pyGetComponent(componentName, (pyobj == Py_True));									\
		}																									\
																											\
		Py_RETURN_NONE;																						\
	}																										\


#define UNIT_CPP_IMPL(APP, CLASS)																			\
	class EntityScriptTimerHandler : public TimerHandler													\
	{																										\
	public:																									\
		EntityScriptTimerHandler(CLASS * entity) : pEntity_( entity )										\
		{																									\
		}																									\
																											\
	private:																								\
		virtual void handleTimeout(TimerHandle handle, void * pUser)										\
		{																									\
			ScriptTimers* scriptTimers = &pEntity_->scriptTimers();											\
			int id = ScriptTimersUtil::getIDForHandle( scriptTimers, handle );								\
			pEntity_->onTimer(id, intptr( pUser ));															\
		}																									\
																											\
		virtual void onRelease( TimerHandle handle, void * /*pUser*/ )										\
		{																									\
			ScriptTimers* scriptTimers = &pEntity_->scriptTimers();											\
			scriptTimers->releaseTimer(handle);																\
			delete this;																					\
		}																									\
																											\
		CLASS* pEntity_;																					\
	};																										\
																											\
	PyObject* CLASS::pyAddTimer(float interval, float repeat, int32 userArg)								\
	{																										\
		EntityScriptTimerHandler* pHandler = new EntityScriptTimerHandler(this);							\
		ScriptTimers * pTimers = &scriptTimers_;															\
		int id = ScriptTimersUtil::addTimer(&pTimers,														\
				interval, repeat,																			\
				userArg, pHandler);																			\
																											\
		if (id == 0)																						\
		{																									\
			PyErr_SetString(PyExc_ValueError, "Unable to add timer");										\
			PyErr_PrintEx(0);																				\
			delete pHandler;																				\
																											\
			return NULL;																					\
		}																									\
																											\
		return PyLong_FromLong(id);																			\
	}																										\
																											\
	PyObject* CLASS::pyDelTimer(ScriptID timerID)															\
	{																										\
		if(!ScriptTimersUtil::delTimer(&scriptTimers_, timerID))											\
		{																									\
			return PyLong_FromLong(-1);																		\
		}																									\
																											\
		return PyLong_FromLong(timerID);																	\
	}																										\
																											\
	void CLASS::destroyEntity()																				\
	{																										\
		APP::getSingleton().destroyEntity(id_, true);														\
	}																										\
																											\
	PyObject* CLASS::pyGetIsDestroyed()																		\
	{																										\
		return PyBool_FromLong(isDestroyed());																\
	}																										\
																											\
	PyObject* CLASS::pyGetClassName()																		\
	{																										\
		return PyUnicode_FromString(scriptName());															\
	}																										\
																											\
	void CLASS::addPositionAndDirectionToStream(MemoryStream& s, bool useAliasID)							\
	{																										\
		UNIT_PROPERTY_UID posuid = UNIT_BASE_PROPERTY_UTYPE_POSITION_XYZ;								\
		UNIT_PROPERTY_UID diruid = UNIT_BASE_PROPERTY_UTYPE_DIRECTION_ROLL_PITCH_YAW;					\
																											\
		Network::FixedMessages::MSGInfo* msgInfo =															\
					Network::FixedMessages::getSingleton().isFixed("Property::position");					\
																											\
		if(msgInfo != NULL)																					\
		{																									\
			posuid = msgInfo->msgid;																		\
			msgInfo = NULL;																					\
		}																									\
																											\
		msgInfo = Network::FixedMessages::getSingleton().isFixed("Property::direction");					\
		if(msgInfo != NULL)																					\
		{																									\
			diruid = msgInfo->msgid;																		\
			msgInfo = NULL;																					\
		}																									\
																											\
		PyObject* pyPos = NULL;																				\
		PyObject* pyDir = NULL;																				\
																											\
																											\
		if(g_componentType == BASEAPP_TYPE)																	\
		{																									\
			PyObject* cellDataDict = PyObject_GetAttrString(this, "cellData");								\
			if(cellDataDict == NULL)																		\
			{																								\
				PyErr_Clear();																				\
				return;																						\
			}																								\
			else																							\
			{																								\
				pyPos = PyDict_GetItemString(cellDataDict, "position");										\
				pyDir = PyDict_GetItemString(cellDataDict, "direction");									\
			}																								\
																											\
			Py_XDECREF(cellDataDict);																		\
			if(pyPos == NULL && pyDir == NULL)																\
			{																								\
				PyErr_Clear();																				\
				return;																						\
			}																								\
		}																									\
		else																								\
		{																									\
			pyPos = PyObject_GetAttrString(this, "position");												\
			pyDir = PyObject_GetAttrString(this, "direction");												\
		}																									\
																											\
																											\
		Vector3 pos, dir;																					\
		script::ScriptVector3::convertPyObjectToVector3(pos, pyPos);										\
		script::ScriptVector3::convertPyObjectToVector3(dir, pyDir);										\
																											\
		if(pScriptModule()->usePropertyDescrAlias() && useAliasID)											\
		{																									\
			ADD_POS_DIR_TO_STREAM_ALIASID(s, pos, dir)														\
		}																									\
		else																								\
		{																									\
			ADD_POS_DIR_TO_STREAM(s, pos, dir)																\
		}																									\
																											\
		if(g_componentType != BASEAPP_TYPE)																	\
		{																									\
			Py_XDECREF(pyPos);																				\
			Py_XDECREF(pyDir);																				\
		}																									\
	}																										\
																											\
	void CLASS::initProperty(bool isReload)																	\
	{																										\
		EntityDef::context().currComponentType = g_componentType;											\
		EntityDef::context().currEntityID = id();															\
																											\
		ScriptDefModule::PROPERTYDESCRIPTION_MAP* oldpropers = NULL;										\
		if(isReload)																						\
		{																									\
			ScriptDefModule* pOldScriptDefModule =															\
										EntityDef::findOldScriptModule(pScriptModule_->getName());			\
			if(!pOldScriptDefModule)																		\
			{																								\
				ERROR_MSG(fmt::format("{}::initProperty: not found old_module!\n",							\
					pScriptModule_->getName()));															\
				KBE_ASSERT(false && "Entity::initProperty: not found old_module");							\
			}																								\
																											\
			oldpropers =																					\
											&pOldScriptDefModule->getPropertyDescrs();						\
		}																									\
																											\
		ScriptDefModule::PROPERTYDESCRIPTION_MAP::const_iterator iter = pPropertyDescrs_->begin();			\
		for(; iter != pPropertyDescrs_->end(); ++iter)														\
		{																									\
			PropertyDescription* propertyDescription = iter->second;										\
			DataType* dataType = propertyDescription->getDataType();										\
																											\
			if(oldpropers)																					\
			{																								\
				ScriptDefModule::PROPERTYDESCRIPTION_MAP::iterator olditer = oldpropers->find(iter->first);	\
				if(olditer != oldpropers->end())															\
				{																							\
					if(strcmp(olditer->second->getDataType()->getName(),									\
							propertyDescription->getDataType()->getName()) == 0 &&							\
						strcmp(olditer->second->getDataType()->getName(),									\
							propertyDescription->getDataType()->getName()) == 0)							\
						continue;																			\
				}																							\
			}																								\
																											\
			if(dataType)																					\
			{																								\
				PyObject* defObj = propertyDescription->newDefaultVal();									\
																											\
				if(dataType->type() == DATA_TYPE_UNIT_COMPONENT)											\
					((EntityComponent*)defObj)->updateOwner(id(), this);									\
																											\
				PyObject_SetAttrString(static_cast<PyObject*>(this),										\
							propertyDescription->getName(), defObj);										\
				Py_DECREF(defObj);																			\
																											\
				/* DEBUG_MSG(fmt::format(#CLASS"::"#CLASS": added [{}] property ref={}.\n",					\
								propertyDescription->getName(), defObj->ob_refcnt));*/						\
			}																								\
			else																							\
			{																								\
				ERROR_MSG(fmt::format("{}::initProperty: {} dataType is NULL！ entityID={}\n",				\
					scriptName(), propertyDescription->getName(), id()));									\
			}																								\
		}																									\
																											\
		/* 由于组件初始化时会自动initProperty，所以只有重加载时需要显示的去调用组件的initProperty */				\
		if(isReload)																						\
		{																									\
			const ScriptDefModule::COMPONENTDESCRIPTION_MAP* pComponentDescrs =								\
				&pScriptModule_->getComponentDescrs();														\
																											\
			ScriptDefModule::COMPONENTDESCRIPTION_MAP::const_iterator iter1 = pComponentDescrs->begin();	\
			for (; iter1 != pComponentDescrs->end(); ++iter1)												\
			{																								\
				PyObject* pComponentProperty = PyObject_GetAttrString(this, iter1->first.c_str());			\
				if(pComponentProperty)																		\
				{																							\
					if(PyObject_TypeCheck(pComponentProperty, EntityComponent::getScriptType()))			\
					{																						\
						EntityComponent* pEntityComponent = static_cast<EntityComponent*>(pComponentProperty);\
						pEntityComponent->initProperty();													\
					}																						\
					else																					\
					{																						\
						PyErr_Format(PyExc_AssertionError, "%s.%s is not property of EntityComponent!",		\
							scriptName(), iter1->first.c_str());											\
						PyErr_PrintEx(0);																	\
					}																						\
																											\
					Py_DECREF(pComponentProperty);															\
				}																							\
				else																						\
				{																							\
					PyErr_Clear();																			\
				}																							\
			}																								\
		}																									\
	}																										\
																											\



#define UNIT_CONSTRUCTION(CLASS)																			\
	id_(id),																								\
	pScriptModule_(const_cast<ScriptDefModule*>(pScriptModule)),											\
	pPropertyDescrs_(&pScriptModule_->getPropertyDescrs()),													\
	spaceID_(0),																							\
	scriptTimers_(),																						\
	pyCallbackMgr_(),																						\
	isDestroyed_(false),																					\
	flags_(UNIT_FLAGS_INITING)																			\


#define UNIT_DECONSTRUCTION(CLASS)																		\
	DEBUG_MSG(fmt::format("{}::~{}(): {}\n", scriptName(), scriptName(), id_));								\
	pScriptModule_ = NULL;																					\
	isDestroyed_ = true;																					\
	removeFlags(UNIT_FLAGS_INITING);																		\


#define UNIT_INIT_PROPERTYS(CLASS)																		\



}
#endif // KBE_UNIT_MACRO_H
