
#include "TcLib.h"

#include "GpsLog.h"
#include "TcPhoto.h"
#include "TcSettings.h"

#include <QDir>

TcLib::TcLib()
: _thread(this)
, _pSettings(NULL)
, _nProgressCurrent(0)
{
}

TcLib::~TcLib()
{
}

void TcLib::Scan()
{
	// ----------------------- scan log
	{
		const QString& sRootDirLog =_pSettings->_sSrcLog;
		qDeleteAll(_lGpsLogs);
		_lGpsLogs.clear();

		QDir dirLog(sRootDirLog);
		QStringList slFiles = dirLog.entryList(QStringList("*.log"), QDir::NoDotAndDotDot|QDir::System|QDir::Hidden|QDir::Files, QDir::DirsFirst);
		slFiles.sort();
		for (int i = 0; i < slFiles.size(); ++i)
		{
			GpsLog* pGpslog =new GpsLog(this, slFiles[i]);
			_lGpsLogs.push_back(pGpslog);
		}
	}

	// ----------------------- scan photo
	{
		qDeleteAll(_lPhotos);
		_lPhotos.clear();

		const QString& sRootDirPhoto =_pSettings->_sSrcJpg;

		QDir dirLog(sRootDirPhoto);
		QStringList slFiles = dirLog.entryList(QStringList("*.jpg"), QDir::NoDotAndDotDot|QDir::System|QDir::Hidden|QDir::Files, QDir::DirsFirst);
		for (int i = 0; i < slFiles.size(); ++i)
		{
			TcPhoto* pPhoto =new TcPhoto(this, sRootDirPhoto, slFiles[i]);
			_lPhotos.push_back(pPhoto);
		}
	}
}

void TcLib::Load()
{
	moveToThread( &_thread );

	connect(&_thread, SIGNAL(started()),  this, SLOT(OnLoadStart()) );
	connect(&_thread, SIGNAL(finished()), this, SLOT(OnLoadEnd())   );

	_thread.start();
}

void TcLib::OnLoadStart()
{
	SetProgress(0);
	for (int i = 0; i < _lGpsLogs.size(); ++i)
	{
		_lGpsLogs[i]->Load();
		_lGpsLogs[i]->WriteGPX();
		_lGpsLogs[i]->WriteKML();
	}
	SetProgress(100);
	Print("Load GPS end !");

	// global log
	{
		GpsLog GpslogGlobal(this, "global.glog");
		GpslogGlobal.Load();
		GpslogGlobal.WriteGPX();
		GpslogGlobal.WriteKML();
	}

	Print("Load JPG start !");

	SetProgress(0);
	int nPhotoCount =_lPhotos.size();
	for (int i = 0; i < nPhotoCount; ++i)
	{
		_lPhotos[i]->Batch();
		int nPercent =int(100.0f*(float(i+1)/float(nPhotoCount)));
		SetProgress(nPercent);
	}
	SetProgress(100);

	Print("Load JPG end !");
}

GpsFrame* TcLib::FindGpsFrame(const GpsDateTime& gTime)
{
	for (int i = 0; i < _lGpsLogs.size(); ++i)
	{
		GpsFrame* pGpsFrame =_lGpsLogs[i]->FindGpsFrame(gTime);
		if (pGpsFrame)
		{
			return pGpsFrame;
		}
	}
	return NULL;
}

void TcLib::OnLoadEnd()
{
}

void TcLib::SetFile(const QString& sFile)
{
	emit sigSetFile(sFile);
}

void TcLib::Print(const QString& sText)
{
	emit sigPushText(sText);
}

void TcLib::SetStatus(const QString& sStatus)
{
	emit sigSetStatus(sStatus);
}

void TcLib::SetProgress(int nProgress)
{
	if (_nProgressCurrent!=nProgress)
	{
		_nProgressCurrent =nProgress;
		emit sigProgress(nProgress);
	}
}

void TcLib::NewImage(QPixmap& pixmap)
{
	emit sigNewImage(pixmap);
}
