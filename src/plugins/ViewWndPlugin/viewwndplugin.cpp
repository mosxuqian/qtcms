#include "viewwndplugin.h"
#include "qfviewedit.h"
#include <QtPlugin>
#include <guid.h>

ViewWndPlugin::ViewWndPlugin() :
m_nRef(0)
{

}

ViewWndPlugin::~ViewWndPlugin()
{

}

QList<QWebPluginFactory::Plugin> ViewWndPlugin::plugins() const
{
	QWebPluginFactory::MimeType mimeType;
	mimeType.name = QString("application/cms-preview-window-test");
	mimeType.description=QString("cms preview window");

	QList<QWebPluginFactory::MimeType> mimeTypes;
	mimeTypes.append(mimeType);

	QWebPluginFactory::Plugin plugin;
	plugin.name = QString("cms preview window");
	plugin.description = QString("cms preview window");
	plugin.mimeTypes=mimeTypes;

	QList<QWebPluginFactory::Plugin> plugins;
	plugins.append(plugin);
	return plugins;
}

QObject * ViewWndPlugin::create( const QString& mimeType, const QUrl&, const QStringList& argumentNames, const QStringList& argumentValues ) const
{
	qfviewedit * edit= new qfviewedit();
	Q_UNUSED(argumentNames);
	Q_UNUSED(argumentValues);
	return edit;
}

long __stdcall ViewWndPlugin::QueryInterface( const IID & iid,void **ppv )
{
	if (IID_IWebPluginBase == iid)
	{
		*ppv = static_cast<IWebPluginBase *>(this);
	}
	else if (IID_IPcomBase == iid)
	{
		*ppv = static_cast<IPcomBase *>(this);
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	static_cast<IPcomBase *>(this)->AddRef();

	return S_OK;
}

unsigned long __stdcall ViewWndPlugin::AddRef()
{
	m_csRef.lock();
	m_nRef ++;
	m_csRef.unlock();
	return m_nRef;
}

unsigned long __stdcall ViewWndPlugin::Release()
{
	int nRet = 0;
	m_csRef.lock();
	m_nRef -- ;
	nRet = m_nRef;
	m_csRef.unlock();
	if (0 == nRet)
	{
		delete this;
	}
	return nRet;
}

Q_EXPORT_PLUGIN2("ViewWndPlugin.dll",ViewWndPlugin)