#include "QFileData.h"
#include <QFile>



#include <QDebug>


#define DEF_EXCEPTION(expCode, wndId, filePath)\
		QVariantMap msg;\
		msg.insert("wndId", wndId);\
		msg.insert("expCode", expCode);\
		msg.insert("filepath", filePath);\
		emit sigThrowException(msg);

#define CURRENT_POSITION(frameHead) m_bPlayDirection?frameHead->tPerFrameIndex.uiNextFrame:frameHead->tPerFrameIndex.uiPreFrame;

#define FILE_POSITION(filePos) m_bPlayDirection?filePos++:filePos--

#define CHECK_FILE_POSITION(filePos)\
	qint32 filePos


#define COPY_FRAME_DATA(pFileFrameHead, iter, size)\
	FrameData frameData(&pFileFrameHead->tFrameHead);\
	memcpy(&frameData.AudioConfig, &iter->curAudioConfig, size);\
	m_bPlayDirection ? iter->pBuffList->push_back(frameData) : iter->pBuffList->push_front(frameData);


QFileData::QFileData()
	: QThread(),
	m_uiStartSec(0),
	m_uiEndSec(0),
	m_i32StartPos(0),
	m_bStop(false),
	m_bPlayDirection(true),
	m_pcbTimeChg(NULL),
	m_pUser(NULL)
{

}

QFileData::~QFileData()
{

}

void QFileData::setParamer( QStringList lstFileList, uint uiStartSec, uint uiEndSec, qint32 i32StartPos )
{
	if (!lstFileList.isEmpty())
	{
		m_lstFileList = lstFileList;
		m_uiStartSec = uiStartSec;
		m_uiEndSec = uiEndSec;
		m_i32StartPos = i32StartPos;
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

void QFileData::setCbTimeChange( pcbTimeChange pro, void* pUser )
{
	if (pro && pUser)
	{
		m_pcbTimeChg = pro;
		m_pUser = pUser;
	}
}

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
	if (isRunning())
	{
		m_bStop = true;
		wait();
	}
	m_wndBuffMap.clear();
}

void QFileData::run()
{
	char *pFileBuff1 = new char[BUFFER_SIZE];
	char *pFileBuff2 = new char[BUFFER_SIZE];
	char *pFileBuff = pFileBuff1;
	if (!pFileBuff1 || !pFileBuff2)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"alloc memory error!";
		return;
	}

	memset(pFileBuff1, 0, BUFFER_SIZE);
	memset(pFileBuff2, 0, BUFFER_SIZE);

	QMap<char*, bool> bufferStatusMap;
	bufferStatusMap.insert(pFileBuff1, false);
	bufferStatusMap.insert(pFileBuff2, false);

	while(!m_bStop)
	{
		uint skiptime = MAX_SECONDS;
		QMap<uint, CurBuffInfo>::iterator iter = m_wndBuffMap.begin(); 
		//Prevent excessive buffering
		if (getMinBufferSize() > MIN_FRAME_NUM)
		{
			msleep(100);
			continue;
		}
		//check whether need to read new file
		while (iter->lastPos > iter->curPos || !iter->curPos)
		{
			if (iter->curBuffer)
			{
				char *curBuffer = iter->curBuffer == pFileBuff1 ? pFileBuff2 : pFileBuff1;
				if (!bufferStatusMap.value(curBuffer))
				{
					if (!readFile(m_lstFileList, m_i32StartPos, curBuffer, BUFFER_SIZE))
					{
						return;
					}
					bufferStatusMap[curBuffer] = true;
					bufferStatusMap[iter->curBuffer] = false;
				}
				iter->curBuffer = curBuffer;
			}
			else
			{
				if (!bufferStatusMap.value(pFileBuff1))
				{
					if (!readFile(m_lstFileList, m_i32StartPos, pFileBuff1, BUFFER_SIZE))
					{
						return;
					}
					bufferStatusMap[pFileBuff1] = true;
				}
			}
			++iter;
			if (iter == m_wndBuffMap.end())
			{
				break;
			}
		}

		iter = m_wndBuffMap.begin();
		while(!m_bStop && iter != m_wndBuffMap.end())
		{
			if (!iter->curBuffer)
			{
				iter->curBuffer = pFileBuff1;
			}
			//check channel
			tagFileHead *fileHead = (tagFileHead *)(iter->curBuffer);
			if ((checkWndIdExist(fileHead->uiChannels, iter.key())
				&& iter->pBuffList->size() >= MAX_FRAME_NUM)
				|| !(checkWndIdExist(fileHead->uiChannels, iter.key())))
			{
				++iter;
				continue;
			}
			if (!iter->curPos)
			{
				iter->curPos = fileHead->tIFrameIndex.uiFirstIFrame[iter.key()];
			}
			//find right position
			tagFileFrameHead *pFileFrameHead = (tagFileFrameHead *)(iter->curBuffer + iter->curPos);
			if (iter->bIsFirstRead)
			{
				iter->lastGMT = pFileFrameHead->tFrameHead.uiGentime;
				iter->curGMT = pFileFrameHead->tFrameHead.uiGentime;
				if (pFileFrameHead->tFrameHead.uiGentime >= m_uiStartSec)
				{
					skiptime = qMin(skiptime, pFileFrameHead->tFrameHead.uiGentime - m_uiStartSec);
				}
				else
				{
					while (!m_bStop && pFileFrameHead->tFrameHead.uiGentime < m_uiStartSec)
					{
						iter->lastPos = iter->curPos;
						iter->curPos = CURRENT_POSITION(pFileFrameHead);
						iter->lastGMT = iter->curGMT;
						iter->curGMT = pFileFrameHead->tFrameHead.uiGentime;
						pFileFrameHead = (tagFileFrameHead *)(iter->curBuffer + iter->curPos);
					}
				}
				iter->bIsFirstRead = false;
			}
			// ensure uninterrupted when read frames
			if (iter->curGMT - iter->lastGMT > 1)
			{
				iter->lastGMT = iter->curGMT;
			}
			//copy frames
// 			while (!m_bStop && iter->pBuffList->size() != MAX_FRAME_NUM)
			while (!m_bStop && (char*)pFileFrameHead < iter->curBuffer + fileHead->uiIndex)
			{
				//start to play when buffer size larger than 20
				if (iter->pBuffList->size() >= MIN_FRAME_NUM)
				{
					emit sigStartPlay(iter.key());
				}

				//has a period time between two frames
				if (iter->curGMT - iter->lastGMT > 2)
				{
					skiptime = qMin(skiptime, iter->curGMT - iter->lastGMT);
					break;
				}
				//copy data to FrameData
				switch (pFileFrameHead->tFrameHead.uiType)
				{
				case FT_Audio:
					{
						COPY_FRAME_DATA(pFileFrameHead, iter, sizeof(tagAudioConfigFrame));
					}
					break;
				case FT_IFrame:
				case FT_PFrame:
				case FT_BFrame:
					{
						COPY_FRAME_DATA(pFileFrameHead, iter, sizeof(tagVideoConfigFrame));
					}
					break;
				case FT_AudioConfig:
					{
						memcpy(&iter->curAudioConfig, &(pFileFrameHead->tFrameHead.pBuffer), sizeof(tagAudioConfigFrame));
					}
					break;
				case FT_VideoConfig:
					{
						memcpy(&iter->curVideoConfig, &(pFileFrameHead->tFrameHead.pBuffer), sizeof(tagVideoConfigFrame));
					}
					break;
				default:
					break;
				}
				iter->lastPos = iter->curPos;
				iter->curPos = CURRENT_POSITION(pFileFrameHead);
				iter->lastGMT = iter->curGMT;
				iter->curGMT = pFileFrameHead->tFrameHead.uiGentime;
				//across two files
				if (iter->lastPos > iter->curPos || !iter->curPos)
				{
					break;
				}
				pFileFrameHead = (tagFileFrameHead *)(iter->curBuffer + iter->curPos);
			}
			++iter;
		}
		if (!m_bStop && skiptime && m_pcbTimeChg)
		{
			m_pcbTimeChg(QString("SkipTime"), skiptime, m_pUser);
			sigSkipTime(skiptime);
		}

	}

	delete[] pFileBuff1;
	delete[] pFileBuff2;
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