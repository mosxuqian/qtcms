#ifndef RECORDER_H
#define RECORDER_H

#include "Recorder_global.h"
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QQueue>
#include <QDebug>
#include "IRecorder.h"
#include "StorageMgr.h"

//#define REC_SYS_DATA					0x11
#define AVENC_IDR		0x01
#define AVENC_PSLICE	0x02
#define AVENC_AUDIO		0x00

class Recorder : public QThread,
	public IRecorder
{
public:
	Recorder();
	~Recorder();
	//IRecorder
	virtual int Start();
	virtual int Stop();
	virtual int InputFrame(FrameInfo frameinfo);
	virtual int SetDevInfo(const QString& devname,int nChannelNum);

	virtual QString getModeName();

	virtual long __stdcall QueryInterface( const IID & iid,void **ppv );

	virtual unsigned long __stdcall AddRef();
	virtual unsigned long __stdcall Release();

	/*typedef struct _tagFrameInfo{
	char * pData;
	unsigned int uiDataSize;
	unsigned int uiTimeStamp;
	}FrameInfo;*/

	typedef struct _tagVideoInfo{
		unsigned int uiWidth;
		unsigned int uiHeight;
	}VideoInfo;

	typedef struct _tagRecNode{
		int    nChannel;
		unsigned int  dwDataType;
		unsigned int  dwBufferSize;
		unsigned int  dwTicketCount;
		int    nWidth;
		int    nHeight;
		char * Buffer;
	}RecBufferNode;
protected:
	void run();
private:
	bool CreateSavePath(QString& sSavePath);
	bool CreateDir(QString fullname);
	void cleardata();
	int m_nRef;
	QMutex m_csRef;

	//
	QString m_devname;
	int m_channelnum;
	//
	int m_nRecWidth;
	int m_nRecHeight;

	unsigned int m_nFrameCount;
	unsigned int m_nLastTicket;
	unsigned int m_nFrameCountArray[31];

	//bool m_stop;
	bool m_bFinish;
	int m_filesize;
	int m_reservedsize;
	QMutex m_dataRef;
	QQueue<RecBufferNode> m_dataqueue;

	StorageMgr m_StorageMgr;
};

#endif // RECORDER_H
