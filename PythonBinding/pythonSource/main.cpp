#include "stdio.h"
#include "common/common.h"
#include "pyscript/py_gc.h"
#include "pyscript/script.h"
#include "pyscript/pyprofile.h"
#include "pyscript/pyprofile_handler.h"
#include "Include/import.h"
#include "Unit.h"

using namespace KBEngine;
KBEngine::script::Script								script_;
KBEngine::script::Script& getScript() { return script_; }
PyObject*												entryScript_;
int main(int argc, char* argv[])
{
	printf("hello world! \n");
	wchar_t* tbuf = KBEngine::strutil::char2wchar("./data/common");
	std::wstring pyPaths = L"./data/base;";
	bool ret = getScript().install(tbuf, pyPaths, "KBEngine", BASEAPP_TYPE);
	free(tbuf);
	Unit::installScript(getScript().getModule());

	std::string entryScriptFileName = "helloWorld";
	PyObject *pyEntryScriptFileName = PyUnicode_FromString(entryScriptFileName.c_str());
	entryScript_ = PyImport_Import(pyEntryScriptFileName);

	if (PyErr_Occurred())
	{
		INFO_MSG(fmt::format("EntityApp::installPyModules: importing scripts/{}.py...\n",entryScriptFileName));

		PyErr_PrintEx(0);
	}

	S_RELEASE(pyEntryScriptFileName);

	if (entryScript_ == NULL)
	{
		printf("ImportModule:%s.py fail!\n", entryScriptFileName.c_str());
	}
	else
	{
		PyObject* pyClass =
			PyObject_GetAttrString(entryScript_, const_cast<char *>("Monster"));
		
		if (pyClass == NULL)
		{
			ERROR_MSG(fmt::format("EntityDef::initialize: Could not find ComponentClass[{}]\n",
				"Monster"));
		}
		else
		{
			ERROR_MSG(fmt::format("EntityDef::initialize: ComponentClass[{}], Successfully!!!\n",
				"Monster"));
			PyObject * pObjectMonster = PyType_GenericAlloc((PyTypeObject*)pyClass, 0);
			if (pObjectMonster == NULL)
			{
				PyErr_Print();
				ERROR_MSG("ScriptDefModule::createObject: GenericAlloc is failed.\n");
			}
			else
			{
				Unit* pUnit = new (pObjectMonster) Unit(9);
				if (PyObject_HasAttrString(pUnit, "__init__"))
				{	
					PyObject* pyResult = PyObject_CallMethod(pUnit, const_cast<char*>("__init__"),
						const_cast<char*>(""));	
					if (pyResult != NULL)
						Py_DECREF(pyResult);
					else
						SCRIPT_ERROR_CHECK();
				}
				printf("Monster refcnt:%d\n", pObjectMonster->ob_refcnt);
			}
		}

		{
			PyObject* pBool = PyBool_FromLong(1);
			PyObject* pyOrder = PyUnicode_FromString("asdgasd");
			PyObject* pyResult = PyObject_CallMethod(entryScript_,
			const_cast<char*>("onAppInit"),
			const_cast<char*>("OO"),
			pBool, pyOrder);
			if (pyResult != NULL)
			Py_DECREF(pyResult);
			else
			SCRIPT_ERROR_CHECK();
			Py_DECREF(pBool);
		}
		
	}
	S_RELEASE(entryScript_);
	Unit::uninstallScript();
	getScript().uninstall();
	char c;
	scanf("%c", &c);
}
