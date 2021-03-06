#ifndef __IDEVICESEARCH_HEAD_FILE_HVPAPG87WDHFVC8SA__
#define __IDEVICESEARCH_HEAD_FILE_HVPAPG87WDHFVC8SA__
#include <libpcom.h>
#include <IEventRegister.h>

interface IDeviceSearch : public IPComBase
{
	virtual int Start() = 0;
	virtual int Stop() = 0;
	virtual int Flush() = 0;
	virtual int setInterval(int nInterval) = 0;
	virtual IEventRegister * QueryEventRegister() = 0;

	enum _enErrorCode{
		OK,
		E_INVALID_PARAM,
		E_SYSTEM_FAILED,
	};
};

#endif