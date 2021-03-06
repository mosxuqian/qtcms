#include "QFileData.h"
#include <QFile>

//#include "vld.h"

#include <QDebug>


#define DEF_EXCEPTION(expCode, wndId, filePath)\
		QVariantMap msg;\
		msg.insert("wndId", wndId);\
		msg.insert("expCode", expCode);\
		msg.insert("filepath", filePath);\
		emit sigThrowException(msg);

#define CURRENT_POSITION(frameHead) m_bPlayDirection?frameHead->tPerFrameIndex.uiNextFrame:frameHead->tPerFrameIndex.uiPreFrame;

#define FILE_POSITION(filePos) m_bPlayDirection?filePos++:filePos--


#define COPY_FRAME_DATA(pFileFrameHead, iter, size)\
	FrameData frameData(&pFileFrameHead->tFrameHead);\
	memcpy(&frameData.AudioConfig, &iter->curAudioConfig, size);\
	m_bPlayDirection ? iter->pBuffList->push_back(frameData) : iter->pBuffList->push_front(frameData);


QFileData::QFileData()
	: QThread(),
	m_uiStartSec(0),
	m_uiEndSec(0),
	m_i32StartPos(0),
	m_i32Types(0x0f),
	m_bStop(false),
	m_bPlayDirection(true),
	m_bPause(false),
// 	m_pcbTimeChg(NULL),
	m_pFileBuff1(NULL),
	m_pFileBuff2(NULL),
	m_pUser(NULL)
{
	m_pFileBuff1 = new char[BUFFER_SIZE];
	m_pFileBuff2 = new char[BUFFER_SIZE];
}

QFileData::~QFileData()
{
	if (m_pFileBuff1)
	{
		delete[] m_pFileBuff1;
		m_pFileBuff1 = NULL;
	}
	if (m_pFileBuff2)
	{
		delete[] m_pFileBuff2;
		m_pFileBuff2 = NULL;
	}
}

void QFileData::setParamer( QStringList lstFileList, uint uiStartSec, uint uiEndSec, qint32 i32StartPos, qint32 i32Types )
{
	if (!lstFileList.isEmpty())
	{
		m_lstFileList = lstFileList;
		m_uiStartSec = uiStartSec;
		m_uiEndSec = uiEndSec;
		m_i32StartPos = i32StartPos;
		m_i32Types = i32Types;
	}
}

void QFileData::setWndIdList(QList<qint32> &wndList)
{
	m_wndList = wndList;
}

void QFileData::setBuffer(qint32 wndId, QList<FrameData> *pquFrameBuffer )
{
	if (pquFrameBuffer && wndId >= 0)
	{
		CurBuffInfo curBuffInfo;
		curBuffInfo.pBuffList = pquFrameBuffer;
		m_wndBuffMap.insert(wndId, curBuffInfo);
	}
}

// void QFileData::setCbTimeChange( pcbTimeChange pro, void* pUser )
// {
// 	if (pro && pUser)
// 	{
// 		m_pcbTimeChg = pro;
// 		m_pUser = pUser;
// 	}
// }

void QFileData::startReadFile()
{
	if (!isRunning())
	{
		m_bStop = false;
		start();
	}
}

void QFileData::stopThread()
{
	qDebug()<<__FUNCTION__<<__LINE__<<"stop thread start";
	if (isRunning())
	{
		m_bStop = true;
		wait();
	}
	m_wndBuffMap.clear();
	m_wndList.clear();
	m_lstFileList.clear();

	qDebug()<<__FUNCTION__<<__LINE__<<"stop thread end";
}

// void QFileData::clearBuffer()
// {
// 	if (m_pFileBuff1)
// 	{
// 		delete[] m_pFileBuff1;
// 		m_pFileBuff1 = NULL;
// 	}
// 	if (m_pFileBuff2)
// 	{
// 		delete[] m_pFileBuff2;
// 		m_pFileBuff2 = NULL;
// 	}
// }

void QFileData::run()
{
	qDebug()<<__FUNCTION__<<__LINE__<<"start run";

// 	m_pFileBuff1 = new char[BUFFER_SIZE];
// 	m_pFileBuff2 = new char[BUFFER_SIZE];

// 	char *m_pFileBuff1 = new char[BUFFER_SIZE];
// 	char *m_pFileBuff2 = new char[BUFFER_SIZE];
	char *pFileBuff = m_pFileBuff1;
	if (!m_pFileBuff1 || !m_pFileBuff2)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"alloc memory error!";
		return;
	}

	memset(m_pFileBuff1, 0, BUFFER_SIZE);
	memset(m_pFileBuff2, 0, BUFFER_SIZE);

// 	QMap<char*, bool> bufferStatusMap;
// 	bufferStatusMap.insert(m_pFileBuff1, false);
// 	bufferStatusMap.insert(m_pFileBuff2, false);

	while(!m_bStop)
	{
// 		QMap<uint, CurBuffInfo>::iterator iter = m_wndBuffMap.begin(); 
		//Prevent excessive buffering
// 		if (getMinBufferSize() > MIN_FRAME_NUM)
// 		if (getMaxBufferSize() > MAX_FRAME_NUM)
// 		{
// 			msleep(100);
// 			continue;
// 		}


// 		//check whether need to read new file
// 		while (iter->lastPos > iter->curPos || !iter->curPos)
// 		{
// 			if (iter->curBuffer)
// 			{
// 				char *curBuffer = iter->curBuffer == pFileBuff1 ? pFileBuff2 : pFileBuff1;
// 				if (!bufferStatusMap.value(curBuffer))
// 				{
// 					if (!readFile(m_lstFileList, m_i32StartPos, curBuffer, BUFFER_SIZE))
// 					{
// 						delete[] pFileBuff1;
// 						delete[] pFileBuff2;
// 						return;
// 					}
// 					bufferStatusMap[curBuffer] = true;
// 					bufferStatusMap[iter->curBuffer] = false;
// 				}
// 				iter->curBuffer = curBuffer;
// 			}
// 			else
// 			{
// 				if (!bufferStatusMap.value(pFileBuff1))
// 				{
// 					if (!readFile(m_lstFileList, m_i32StartPos, pFileBuff1, BUFFER_SIZE))
// 					{
// 						delete[] pFileBuff1;
// 						delete[] pFileBuff2;
// 						return;
// 					}
// 					bufferStatusMap[pFileBuff1] = true;
// 				}
// 			}
// 			++iter;
// 			if (iter == m_wndBuffMap.end())
// 			{
// 				break;
// 			}
// 		}


		if (!readFile(m_lstFileList, m_i32StartPos, pFileBuff, BUFFER_SIZE))
		{
// 			delete[] m_pFileBuff1;
			emit sigStopPlay();
			return;
		}

		tagFileHead *pFileHead = (tagFileHead *)m_pFileBuff1;
		tagFileFrameHead *pFrameHead = (tagFileFrameHead*)(m_pFileBuff1 + sizeof(tagFileHead));
		QMap<uint, CurBuffInfo>::iterator iter;

		while (!m_bStop && (char*)pFrameHead < m_pFileBuff1 + pFileHead->uiIndex)
		{
			if (m_bPause){
				msleep(10);
				continue;
			}

			iter = m_wndBuffMap.find(pFrameHead->tFrameHead.uiChannel);
			//current channel isn't in group or types are not needed
			if (iter == m_wndBuffMap.end() || pFrameHead->tFrameHead.uiGentime < m_uiStartSec || !(pFrameHead->tFrameHead.uiRecType & m_i32Types) )
			{
				pFrameHead = (tagFileFrameHead *)((char*)pFrameHead + sizeof(tagFileFrameHead) - sizeof(pFrameHead->tFrameHead.pBuffer) + pFrameHead->tFrameHead.uiLength);
				continue;
			}
			//start to play when buffer size larger than 20
			if (!iter->bIsPlaying && iter->pBuffList->size() >= MIN_FRAME_NUM)
			{
				emit sigStartPlay(iter.key());
				iter->bIsPlaying = true;
			}
			//current buffer size is out of 1000
			if (iter->pBuffList->size() >= MAX_FRAME_NUM)
			{
				do 
				{
					msleep(100);
				} while (!m_bStop && iter->pBuffList->size() > 200);
				getMaxBufferSize();
// 				msleep(100);
// 				continue;
			}
			//copy data to FrameData
			switch (pFrameHead->tFrameHead.uiType)
			{
			case FT_Audio:
				{
					if (!m_i32Speed){
						COPY_FRAME_DATA(pFrameHead, iter, sizeof(tagAudioConfigFrame));
					}
				}
				break;
			case FT_IFrame:
			case FT_PFrame:
			case FT_BFrame:
				{
// 					if (-8 == m_i32Speed && FT_IFrame != pFrameHead->tFrameHead.uiType){
// 						break;
// 					}
					COPY_FRAME_DATA(pFrameHead, iter, sizeof(tagVideoConfigFrame));
				}
				break;
			case FT_AudioConfig:
				{
					if (!m_i32Speed){
						memcpy(&iter->curAudioConfig, &(pFrameHead->tFrameHead.pBuffer), sizeof(tagAudioConfigFrame));
					}
				}
				break;
			case FT_VideoConfig:
				{
					memcpy(&iter->curVideoConfig, &(pFrameHead->tFrameHead.pBuffer), sizeof(tagVideoConfigFrame));
				}
				break;
			default:
				break;
			}
			pFrameHead = (tagFileFrameHead *)((char*)pFrameHead + sizeof(tagFileFrameHead) - sizeof(pFrameHead->tFrameHead.pBuffer) + pFrameHead->tFrameHead.uiLength);
		}

		iter = m_wndBuffMap.begin();
		while (iter != m_wndBuffMap.end())
		{
			qDebug()<<__FUNCTION__<<"wnd: "<<iter.key()<<" buffsize: "<<iter->pBuffList->size();
			++iter;
		}

// 		iter = m_wndBuffMap.begin();
// 		while(!m_bStop && iter != m_wndBuffMap.end())
// 		{
// 			if (!iter->curBuffer)
// 			{
// 				iter->curBuffer = m_pFileBuff1;
// 			}
// 			//check channel
// 			tagFileHead *fileHead = (tagFileHead *)(iter->curBuffer);
// 			if (/*(checkWndIdExist(fileHead->uiChannels, iter.key())
// 				&& iter->pBuffList->size() >= MAX_FRAME_NUM)
// 				|| */!(checkWndIdExist(fileHead->uiChannels, iter.key())))
// 			{
// 				++iter;
// 				continue;
// 			}
// 			if (!iter->curPos)
// 			{
// 				iter->curPos = fileHead->tIFrameIndex.uiFirstIFrame[iter.key()];
// 			}
// 			//find first frame for current channel in file
// 			tagFileFrameHead *pFileFrameHead = NULL;
// 			if (!iter->curPos)
// 			{
// 				//no I frame in this file
// 				pFileFrameHead = (tagFileFrameHead *)(iter->curBuffer + sizeof(tagFileHead));
// 				while (pFileFrameHead->tFrameHead.uiChannel != iter.key())
// 				{
// 					pFileFrameHead = (tagFileFrameHead *)((char*)pFileFrameHead + sizeof(tagFileFrameHead) - sizeof(pFileFrameHead->tFrameHead.pBuffer) + pFileFrameHead->tFrameHead.uiLength);
// 				}
// 			} 
// 			else
// 			{
// 				//find first I frame
// 				pFileFrameHead = (tagFileFrameHead *)(iter->curBuffer + iter->curPos);
// 				while (pFileFrameHead->tPerFrameIndex.uiPreFrame > 0)
// 				{
// 					pFileFrameHead = (tagFileFrameHead *)(iter->curBuffer + pFileFrameHead->tPerFrameIndex.uiPreFrame);
// 					iter->curPos = pFileFrameHead->tPerFrameIndex.uiPreFrame;
// 				}
// 			}
// 
// 			//find right position
// 			if (iter->bIsFirstRead)
// 			{
// 				while (!m_bStop && pFileFrameHead->tFrameHead.uiGentime < m_uiStartSec)
// 				{
// 					iter->lastPos = iter->curPos;
// 					iter->curPos = CURRENT_POSITION(pFileFrameHead);
// 					pFileFrameHead = (tagFileFrameHead *)(iter->curBuffer + iter->curPos);
// 				}
// 				iter->bIsFirstRead = false;
// 			}
// 			//copy frames
// // 			while (!m_bStop && iter->pBuffList->size() != MAX_FRAME_NUM)
// // 			while (!m_bStop && (char*)pFileFrameHead < iter->curBuffer + fileHead->uiIndex)
// 			while (!m_bStop)
// 			{
// 				//start to play when buffer size larger than 20
// 				if (!iter->bIsPlaying && iter->pBuffList->size() >= MIN_FRAME_NUM)
// 				{
// 					emit sigStartPlay(iter.key());
// 					iter->bIsPlaying = true;
// 				}
// 				//copy data to FrameData
// 				switch (pFileFrameHead->tFrameHead.uiType)
// 				{
// 				case FT_Audio:
// 					{
// 						COPY_FRAME_DATA(pFileFrameHead, iter, sizeof(tagAudioConfigFrame));
// 					}
// 					break;
// 				case FT_IFrame:
// 				case FT_PFrame:
// 				case FT_BFrame:
// 					{
// 						COPY_FRAME_DATA(pFileFrameHead, iter, sizeof(tagVideoConfigFrame));
// 					}
// 					break;
// 				case FT_AudioConfig:
// 					{
// 						memcpy(&iter->curAudioConfig, &(pFileFrameHead->tFrameHead.pBuffer), sizeof(tagAudioConfigFrame));
// 					}
// 					break;
// 				case FT_VideoConfig:
// 					{
// 						memcpy(&iter->curVideoConfig, &(pFileFrameHead->tFrameHead.pBuffer), sizeof(tagVideoConfigFrame));
// 					}
// 					break;
// 				default:
// 					break;
// 				}
// 				iter->lastPos = iter->curPos;
// 				iter->curPos = CURRENT_POSITION(pFileFrameHead);
// 				//across two files
// 				if (iter->lastPos > iter->curPos || !iter->curPos)
// 				{
// 					break;
// 				}
// 				pFileFrameHead = (tagFileFrameHead *)(iter->curBuffer + iter->curPos);
// 			}
// 
// 			qDebug()<<__FUNCTION__<<__LINE__<<"wndId: "<<iter.key()<<" buff size: "<<iter->pBuffList->size();
// 
// 			++iter;
// 
// 		}
	}

	qDebug()<<__FUNCTION__<<__LINE__<<"stop run";

// 	delete[] m_pFileBuff1;
// 	delete[] m_pFileBuff2;
}

bool QFileData::readFile( QStringList &filePathList, qint32 &startPos, char* buffer, qint32 buffSize )
{
	qint32 pos = FILE_POSITION(startPos);
	if (pos < 0 || pos > filePathList.size() - 1)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"file pos not in file list!";
		return false;
	}
	QString filePath = filePathList[pos];
	memset(buffer, 0, buffSize);
	//check file exist
	if (!QFile::exists(filePath))
	{
		qDebug()<<__FUNCTION__<<__LINE__<<filePath<<" don't exist!";
		return false;
	}
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"can't open "<<filePath;
		return false;
	}
	//read file to buffer
	qint64 fileSize = file.size();
	if (file.read(buffer, fileSize) != fileSize)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"read file to buffer error!";
		file.close();
		return false;
	}
	qDebug()<<__FUNCTION__<<__LINE__<<"read file "<<filePath;
	file.close();
	return true;
}

bool QFileData::checkWndIdExist( uint *pChlArr, uint wndId )
{
	if (wndId >= WNDMAX_SIZE/2)
	{
		++pChlArr;
		wndId %= WNDMAX_SIZE/2;
	}
	return ((*pChlArr)>>wndId) & 1;
}

qint32 QFileData::getMinBufferSize()
{
	qint32 minSize = 0;
	QMap<uint, CurBuffInfo>::iterator it = m_wndBuffMap.begin();
	minSize = it->pBuffList->size();
	++it;
	while (it != m_wndBuffMap.end())
	{
		minSize = qMin(minSize, it->pBuffList->size());
		++it;
	}
	return minSize;
}

qint32 QFileData::getMaxBufferSize()
{
	qint32 maxSize = 0;
	QMap<uint, CurBuffInfo>::iterator it = m_wndBuffMap.begin();
	maxSize = it->pBuffList->size();
	qDebug()<<__FUNCTION__<<"wndId: "<<it.key()<<" buffer size: "<<it->pBuffList->size();
	++it;
	while (it != m_wndBuffMap.end())
	{
		qDebug()<<__FUNCTION__<<"wndId: "<<it.key()<<" buffer size: "<<it->pBuffList->size();
		maxSize = qMax(maxSize, it->pBuffList->size());
		++it;
	}
	return maxSize;
}

void QFileData::setSpeed( qint32 speed )
{
	m_i32Speed = speed;
}

void QFileData::paused( bool pause )
{
	m_bPause = pause;
}
