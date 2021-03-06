#include "QtQWebView.h"
#include <QFileDialog>
#include <QMap>
#include <QEventLoop>


QtQWebView::QtQWebView():QWebPluginFWBase(this)
{

}


QtQWebView::~QtQWebView()
{
	QList<tagViewPage>::const_iterator item;
	for (item=m_ViewPageList.constBegin();item!=m_ViewPageList.constEnd();++item)
	{
		item->m_SubWebView->close();
		delete item->m_SubWebView;
	}
	m_ViewPageList.clear();
}

void QtQWebView::LoadNewPage( QString url )
{
	if (SubWebView::bIsbuilding==true)
	{
		return;
	}
	SubWebView::bIsbuilding=true;
	QList<tagViewPage>::const_iterator ite;
	for (ite=m_ViewPageList.constBegin();ite!=m_ViewPageList.constEnd();++ite)
	{
		ite->m_SubWebView->hide();
	}
	//获取当前page的大小和位置
	QWidget *pa_size=this->parentWidget();
	PageSize=((QWebView*)pa_size)->page()->view()->size();
	QRect m_geometry=((QWebView*)pa_size)->page()->view()->geometry();
	nX=m_geometry.x();
	nY=m_geometry.y();

	QList<tagViewPage>::const_iterator item;
	for (item=m_ViewPageList.constBegin();item!=m_ViewPageList.constEnd();++item)
	{
		
		if (url==item->url)
		{
			item->m_SubWebView->resize(PageSize);
			item->m_SubWebView->move(nX,nY);
			//item->m_SubWebView->show();
			//全屏
			item->m_SubWebView->showFullScreen();
			item->m_SubWebView->OnRefressMessage();
			//隐藏主页
			QEventLoop eventloop;
			QTimer::singleShot(50,&eventloop,SLOT(quit()));
			eventloop.exec();
			QWidget *pa=this->parentWidget();
			((QWebView*)pa)->hide();
			SubWebView::bIsbuilding=false;
			return;
		}
	}
	tagViewPage m_tagViewPage;
	m_tagViewPage.m_SubWebView=new SubWebView(url,PageSize);
	if (NULL==m_tagViewPage.m_SubWebView)
	{
		SubWebView::bIsbuilding=false;
		return;
	}
	connect(m_tagViewPage.m_SubWebView,SIGNAL(LoadOrChangeUrl(const QString &)),this,SLOT(LoadNewPageFromViewSignal(const QString &)));
	connect(m_tagViewPage.m_SubWebView,SIGNAL(CloseAllPage()),this,SLOT(CloseAllPage()));
	/*m_tagViewPage.m_SubWebView->showMaximized();*/
	m_tagViewPage.m_SubWebView->resize(PageSize);
	m_tagViewPage.m_SubWebView->move(nX,nY);
	while(m_tagViewPage.m_SubWebView->IsLoad==false){
		QEventLoop eventloop;
		QTimer::singleShot(5,&eventloop,SLOT(quit()));
		eventloop.exec();
	}
	QWidget *pa=this->parentWidget();
	/*m_tagViewPage.m_SubWebView->show();*/
	//全屏
	m_tagViewPage.m_SubWebView->showFullScreen();
	//隐藏主页
	QEventLoop eventloop;
	QTimer::singleShot(10,&eventloop,SLOT(quit()));
	eventloop.exec();
	
	((QWebView*)pa)->hide();
	m_tagViewPage.url = url;
	m_ViewPageList.append(m_tagViewPage);
	SubWebView::bIsbuilding=false;
	return;
}

void QtQWebView::LoadNewPageFromViewSignal( const QString &text )
{	SubWebView::bIsbuilding=true;
	QDomDocument ConFile;
	ConFile.setContent(text);
	QDomNode pageaction=ConFile.elementsByTagName("pageaction").at(0);
	QString SrcUrl=pageaction.toElement().attribute("SrcUrl");
	QString SrcAct=pageaction.toElement().attribute("SrcAct");
	QString DstUrl=pageaction.toElement().attribute("DstUrl");
	QString DstAct=pageaction.toElement().attribute("DstAct");
	//获取当前显示窗口的大小
	QList<tagViewPage>::const_iterator itSize;
	for(itSize=m_ViewPageList.constBegin();itSize!=m_ViewPageList.constEnd();++itSize){
		if (!itSize->m_SubWebView->isHidden())
		{
			itSize->m_SubWebView->size();
			itSize->m_SubWebView->page()->view();
			 PageSize=itSize->m_SubWebView->page()->view()->size();
			 qDebug()<<PageSize<<"get PageSize";
			 QRect m_geometry= itSize->m_SubWebView->page()->view()->geometry();
			 nX=m_geometry.x();
			 nY=m_geometry.y();
		}
	}
	//跳转到主页
	if ("index"==SrcAct)
	{
		//显示主页
		QWidget *pa=this->parentWidget();
		((QWebView*)pa)->showFullScreen();
		pa->resize(PageSize);
		((QWebView*)pa)->move(nX,nY);
		QEventLoop eventloop;
		QTimer::singleShot(10,&eventloop,SLOT(quit()));
		eventloop.exec();
		QList<tagViewPage>::const_iterator it;
		for(it=m_ViewPageList.constBegin();it!=m_ViewPageList.constEnd();++it){
			it->m_SubWebView->hide();
		}
		OnRefressMessage();
		SubWebView::bIsbuilding=false;
		return;
	}

	//关闭当前页面
	QList<tagViewPage>::const_iterator item;
	for (item=m_ViewPageList.constBegin();item!=m_ViewPageList.constEnd();++item)
	{
		if (SrcUrl==item->url&&SrcAct=="close")
		{
			item->m_SubWebView->close();
			//显示主页
			QWidget *pa=this->parentWidget();
			((QWebView*)pa)->showFullScreen();
			OnRefressMessage();
			SubWebView::bIsbuilding=false;
			return;
		}
	}
	//跳转到目的页面
	if ("new"==DstAct)
	{
		bool bExit=false;

		QList<tagViewPage>::const_iterator it;
		for (it=m_ViewPageList.constBegin();it!=m_ViewPageList.constEnd();++it)
		{
			if (DstUrl==it->url)
			{
				it->m_SubWebView->showFullScreen();
				//it->m_SubWebView->page()->view()->resize(PageSize);
				//it->m_SubWebView->page()->view()->move(nX,nY);
				it->m_SubWebView->OnRefressMessage();
				QEventLoop eventloop;
				QTimer::singleShot(10,&eventloop,SLOT(quit()));
				eventloop.exec();
				bExit=true;
				break;
			}
		}
		QList<tagViewPage>::const_iterator ite;

		if (false==bExit)
		{
			tagViewPage m_tagViewPage;
			m_tagViewPage.m_SubWebView=new SubWebView(DstUrl,PageSize);
			if (NULL!=m_tagViewPage.m_SubWebView)
			{
				connect(m_tagViewPage.m_SubWebView,SIGNAL(LoadOrChangeUrl(const QString &)),this,SLOT(LoadNewPageFromViewSignal(const QString &)));
				connect(m_tagViewPage.m_SubWebView,SIGNAL(CloseAllPage()),this,SLOT(CloseAllPage()));
				m_tagViewPage.m_SubWebView->page()->view()->resize(PageSize);
				m_tagViewPage.m_SubWebView->page()->view()->move(nX,nY);
				while(m_tagViewPage.m_SubWebView->IsLoad==false){
					QEventLoop eventloop;
					QTimer::singleShot(5,&eventloop,SLOT(quit()));
					eventloop.exec();
				}
				/*m_tagViewPage.m_SubWebView->show();*/
				//全屏
				m_tagViewPage.m_SubWebView->showFullScreen();
				QEventLoop eventloop;
				QTimer::singleShot(10,&eventloop,SLOT(quit()));
				eventloop.exec();
				m_tagViewPage.url.append(DstUrl);
				m_ViewPageList.append(m_tagViewPage);
			}
		}
		for(ite=m_ViewPageList.constBegin();ite!=m_ViewPageList.constEnd();++ite){
			if (DstUrl!=ite->url)
			{
				ite->m_SubWebView->hide();
			}
		}
		SubWebView::bIsbuilding=false;
		return;
	}
	//当前页面重新加载目的页面
	if ("reload"==DstAct)
	{
		QList<tagViewPage>::iterator it;
		for(it=m_ViewPageList.begin();it!=m_ViewPageList.end();++it){
			if (it->url==SrcUrl)
			{
				QString temp=QCoreApplication::applicationDirPath();
				temp.append(DstUrl);
				it->m_SubWebView->load(temp);
				it->url.clear();
				it->url.append(DstUrl);
				it->m_SubWebView->showMaximized();
			}
		}
	}
	SubWebView::bIsbuilding=false;
	return;
}

void QtQWebView::CloseAllPage()
{
	//关闭窗口
	QWidget *pa=this->parentWidget();
	((QWebView*)pa)->close();
}

void QtQWebView::OnRefressMessage()
{
	QWidget *pa=this->parentWidget();
	((QWebView*)pa)->page()->mainFrame()->title();
	QVariantMap eventParam;
	eventParam.insert("title",((QWebView*)pa)->page()->mainFrame()->title());
	eventParam.insert("refresh","true");
	eventParam.insert("Dsturl","null");
	QString sEvent="refresh";
	QString Scripte=EventProcsScripte(sEvent,eventParam);
	((QWebView*)pa)->page()->mainFrame()->evaluateJavaScript(Scripte);
}

QString QtQWebView::EventProcsScripte( QString sEvent,QVariantMap eventParam )
{
	QString sItem="subViewMsg(data)";
	QString sScripte;
	sScripte += "{var e={";
	QVariantMap::const_iterator itParameters;
	for (itParameters = eventParam.begin();itParameters != eventParam.end(); itParameters ++)
	{
		QString sKey = itParameters.key();
		QString sValue = itParameters.value().toString();
		sScripte += sKey;
		sScripte += ":'";
		sScripte += sValue;
		sScripte += "'";
		if (itParameters + 1 != eventParam.end())
		{
			sScripte += ",";
		}
	}
	QString sEventProc;
	sEventProc+=";var Proc={'Proc':'";
	sEventProc+=sEvent;
	sEventProc+="'};";
	sScripte += "}";
	sScripte+=sEventProc;
	sScripte += sItem.replace(QRegExp("\\((.*)\\)"),"(Proc,e)");
	sScripte += ";}";
	return sScripte;
}

