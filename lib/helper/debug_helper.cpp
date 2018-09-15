// Copyright 2008-2018 Yolo Technologies, Inc. All Rights Reserved. https://www.comblockengine.com


#include "debug_helper.h"
#include "profile.h"
#include "common/common.h"
#include "common/timer.h"
#include "thread/threadguard.h"

#ifdef unix
#include <unistd.h>
#include <syslog.h>
#endif

#include <sys/timeb.h>

#ifndef NO_USE_LOG4CXX
#if KBE_PLATFORM == PLATFORM_WIN32
#pragma comment (lib, "Mswsock.lib")
#pragma comment( lib, "odbc32.lib" )
#endif
#endif

namespace KBEngine{
	
KBE_SINGLETON_INIT(DebugHelper);

DebugHelper dbghelper;
ProfileVal g_syncLogProfile("syncLog");

#define KBE_LOG4CXX_ERROR(s)	\
	{	\
			printf("ERROR=  %s\n",s.c_str());	\
    }

#define KBE_LOG4CXX_WARN(s)	\
	{	\
			printf("WARN=  %s\n", s.c_str());	\
    }
    
#define KBE_LOG4CXX_INFO(s)	\
	{	\
			printf("INFO=  %s\n", s.c_str());	\
    }
    
#define KBE_LOG4CXX_DEBUG(s)	\
	{	\
			printf("DEBUG=  %s\n",s.c_str());	\
    }

#define KBE_LOG4CXX_FATAL(s)	\
	{	\
			printf("FATAL=  %s\n", s.c_str());	\
    }
    
#define KBE_LOG4CXX_LOG(s)	\
	{	\
			printf("LOG=  %s\n", s.c_str());	\
    }
    

#define DBG_PT_SIZE 1024 * 4

bool g_shouldWriteToSyslog = false;

#ifdef KBE_USE_ASSERTS
void myassert(const char * exp, const char * func, const char * file, unsigned int line)
{
	DebugHelper::getSingleton().backtrace_msg();
	std::string s = (fmt::format("assertion failed: {}, file {}, line {}, at: {}\n", exp, file, line, func));

	dbghelper.print_msg(s);
    abort();
}
#endif

#if KBE_PLATFORM == PLATFORM_WIN32
	#define ALERT_LOG_TO(NAME, CHANGED)							\
	{															\
		wchar_t exe_path[MAX_PATH];								\
		memset(exe_path, 0, MAX_PATH * sizeof(wchar_t));		\
		GetCurrentDirectory(MAX_PATH, exe_path);				\
																\
		char* ccattr = strutil::wchar2char(exe_path);			\
		if(CHANGED)												\
			printf("Logging(changed) to: %s/logs/"NAME"%s.*.log\n\n", ccattr, COMPONENT_NAME_EX(BASEAPP_TYPE));\
		else													\
			printf("Logging to: %s/logs/"NAME"%s.*.log\n\n", ccattr, COMPONENT_NAME_EX(BASEAPP_TYPE));\
		free(ccattr);											\
	}															\

#else
#define ALERT_LOG_TO(NAME, CHANGED) {}
#endif

//-------------------------------------------------------------------------------------
void utf8printf(FILE *out, const char *str, ...)
{
    va_list ap;
    va_start(ap, str);
    vutf8printf(stdout, str, &ap);
    va_end(ap);
}

//-------------------------------------------------------------------------------------
void vutf8printf(FILE *out, const char *str, va_list* ap)
{
    vfprintf(out, str, *ap);
}

//-------------------------------------------------------------------------------------
class DebugHelperSyncHandler  : public TimerHandler
{
public:
	DebugHelperSyncHandler():
	pActiveTimerHandle_(NULL)
	{
	}

	virtual ~DebugHelperSyncHandler()
	{
		// cancel();
	}

	enum TimeOutType
	{
		TIMEOUT_ACTIVE_TICK,
		TIMEOUT_MAX
	};

	virtual void handleTimeout(TimerHandle handle, void * arg)
	{
		g_syncLogProfile.start();
		DebugHelper::getSingleton().sync();
		g_syncLogProfile.stop();
	}

	//-------------------------------------------------------------------------------------
	void cancel()
	{
		if(pActiveTimerHandle_ == NULL)
			return;

		pActiveTimerHandle_->cancel();
		delete pActiveTimerHandle_;
		pActiveTimerHandle_ = NULL;
	}

	//-------------------------------------------------------------------------------------
	void startActiveTick()
	{
		
	}

private:
	TimerHandle* pActiveTimerHandle_;
};

DebugHelperSyncHandler* g_pDebugHelperSyncHandler = NULL;

//-------------------------------------------------------------------------------------
DebugHelper::DebugHelper() :
_logfile(NULL),
_currFile(),
_currFuncName(),
_currLine(0),
logMutex(),
noSyncLog_(false),
canLogFile_(true),

#if KBE_PLATFORM == PLATFORM_WIN32
mainThreadID_(GetCurrentThreadId())
#else
mainThreadID_(pthread_self())
#endif
{
	g_pDebugHelperSyncHandler = new DebugHelperSyncHandler();
}

//-------------------------------------------------------------------------------------
DebugHelper::~DebugHelper()
{
	finalise(true);
}	

//-------------------------------------------------------------------------------------
void DebugHelper::shouldWriteToSyslog(bool v)
{
	g_shouldWriteToSyslog = v;
}

//-------------------------------------------------------------------------------------
std::string DebugHelper::getLogName()
{
#ifndef NO_USE_LOG4CXX
#endif

	return "";
}

//-------------------------------------------------------------------------------------
void DebugHelper::changeLogger(const std::string& name)
{
#ifndef NO_USE_LOG4CXX
#endif
}

//-------------------------------------------------------------------------------------
void DebugHelper::lockthread()
{
	logMutex.lockMutex();
}

//-------------------------------------------------------------------------------------
void DebugHelper::unlockthread()
{
	logMutex.unlockMutex();
}

//-------------------------------------------------------------------------------------
void DebugHelper::initialize(COMPONENT_TYPE componentType)
{
}

//-------------------------------------------------------------------------------------
void DebugHelper::finalise(bool destroy)
{
	if(!destroy)
	{

		sleep(1000);
	}

	DebugHelper::getSingleton().clearBufferedLog(true);
}

//-------------------------------------------------------------------------------------
void DebugHelper::clearBufferedLog(bool destroy)
{
}

//-------------------------------------------------------------------------------------
void DebugHelper::sync()
{
	
}

//-------------------------------------------------------------------------------------
void DebugHelper::printBufferedLogs()
{

}

//-------------------------------------------------------------------------------------
void DebugHelper::print_msg(const std::string& s)
{

}

//-------------------------------------------------------------------------------------
void DebugHelper::error_msg(const std::string& s)
{
	KBEngine::thread::ThreadGuard tg(&this->logMutex); 

	KBE_LOG4CXX_ERROR(s);

	set_errorcolor();
	set_normalcolor();
}

//-------------------------------------------------------------------------------------
void DebugHelper::info_msg(const std::string& s)
{
	KBEngine::thread::ThreadGuard tg(&this->logMutex); 

	if(canLogFile_)
		KBE_LOG4CXX_INFO( s);

}

//-------------------------------------------------------------------------------------
int KBELOG_TYPE_MAPPING(int type)
{
	return KBELOG_SCRIPT_NORMAL;

}

//-------------------------------------------------------------------------------------
void DebugHelper::script_info_msg(const std::string& s)
{
	KBEngine::thread::ThreadGuard tg(&this->logMutex); 

	if(canLogFile_)
		KBE_LOG4CXX_LOG(s);
}

//-------------------------------------------------------------------------------------
void DebugHelper::script_error_msg(const std::string& s)
{
	KBEngine::thread::ThreadGuard tg(&this->logMutex); 

	if(canLogFile_)
		KBE_LOG4CXX_LOG( s);

	set_errorcolor();
	set_normalcolor();
}

//-------------------------------------------------------------------------------------
void DebugHelper::setScriptMsgType(int msgtype)
{
	scriptMsgType_ = msgtype;
}

//-------------------------------------------------------------------------------------
void DebugHelper::resetScriptMsgType()
{
}

//-------------------------------------------------------------------------------------
void DebugHelper::debug_msg(const std::string& s)
{
	KBEngine::thread::ThreadGuard tg(&this->logMutex); 
	if(canLogFile_)
		KBE_LOG4CXX_DEBUG(s);
}

//-------------------------------------------------------------------------------------
void DebugHelper::warning_msg(const std::string& s)
{
	KBEngine::thread::ThreadGuard tg(&this->logMutex); 


	if(canLogFile_)
		KBE_LOG4CXX_WARN(s);

	set_warningcolor();
	//printf("%s%02d: [WARNING]: %s", COMPONENT_NAME_EX_2(g_componentType), g_componentGroupOrder, s.c_str());
	set_normalcolor();
}

//-------------------------------------------------------------------------------------
void DebugHelper::critical_msg(const std::string& s)
{
	KBEngine::thread::ThreadGuard tg(&this->logMutex); 

	char buf[DBG_PT_SIZE];
	kbe_snprintf(buf, DBG_PT_SIZE, "%s(%d) -> %s\n\t%s\n", _currFile.c_str(), _currLine, _currFuncName.c_str(), s.c_str());

	KBE_LOG4CXX_FATAL( std::string(buf));

	set_errorcolor();
	//printf("%s%02d: [FATAL]: %s", COMPONENT_NAME_EX_2(g_componentType), g_componentGroupOrder, s.c_str());
	set_normalcolor();
	backtrace_msg();
}

//-------------------------------------------------------------------------------------
void DebugHelper::set_errorcolor()
{
#if KBE_PLATFORM == PLATFORM_WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
#endif
}

//-------------------------------------------------------------------------------------
void DebugHelper::set_normalcolor()
{
#if KBE_PLATFORM == PLATFORM_WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED|FOREGROUND_GREEN|
		FOREGROUND_BLUE);
#endif
}

//-------------------------------------------------------------------------------------
void DebugHelper::set_warningcolor()
{
#if KBE_PLATFORM == PLATFORM_WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED|FOREGROUND_GREEN);
#endif
}

//-------------------------------------------------------------------------------------
#ifdef unix
#define MAX_DEPTH 50
#include <execinfo.h>
#include <cxxabi.h>

void DebugHelper::backtrace_msg()
{
	void ** traceBuffer = new void*[MAX_DEPTH];
	uint32 depth = backtrace( traceBuffer, MAX_DEPTH );
	char ** traceStringBuffer = backtrace_symbols( traceBuffer, depth );
	for (uint32 i = 0; i < depth; i++)
	{
		// Format: <executable path>(<mangled-function-name>+<function
		// instruction offset>) [<eip>]
		std::string functionName;

		std::string traceString( traceStringBuffer[i] );
		std::string::size_type begin = traceString.find( '(' );
		bool gotFunctionName = (begin >= 0);

		if (gotFunctionName)
		{
			// Skip the round bracket start.
			++begin;
			std::string::size_type bracketEnd = traceString.find( ')', begin );
			std::string::size_type end = traceString.rfind( '+', bracketEnd );
			std::string mangled( traceString.substr( begin, end - begin ) );

			int status = 0;
			size_t demangledBufferLength = 0;
			char * demangledBuffer = abi::__cxa_demangle( mangled.c_str(), 0, 
				&demangledBufferLength, &status );

			if (demangledBuffer)
			{
				functionName.assign( demangledBuffer, demangledBufferLength );

				// __cxa_demangle allocates the memory for the demangled
				// output using malloc(), we need to free it.
				free( demangledBuffer );
			}
			else
			{
				// Didn't demangle, but we did get a function name, use that.
				functionName = mangled;
			}
		}

		std::string ss = fmt::format("Stack: #{} {}\n", 
			i,
			((gotFunctionName) ? functionName.c_str() : traceString.c_str()));

#ifdef NO_USE_LOG4CXX
#else
			KBE_LOG4CXX_INFO(g_logger, ss);
#endif

			onMessage(KBELOG_PRINT, ss.c_str(), ss.size());

	}

	free(traceStringBuffer);
	delete[] traceBuffer;
}

#else
void DebugHelper::backtrace_msg()
{
}
#endif

//-------------------------------------------------------------------------------------
void DebugHelper::closeLogger()
{
	// close logger for fork + execv
#ifndef NO_USE_LOG4CXX

#endif
}


}


