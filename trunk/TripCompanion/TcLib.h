#ifndef GPSLOGLIB_H
#define GPSLOGLIB_H

#include <QList>
#include <QThread>
#include <QPixmap>

class TcSettings;
class GpsLog;
class TcPhoto;
class GpsFrame;
class GpsDateTime;

class TcLib : public QObject
{
	Q_OBJECT

public:
					TcLib();
					~TcLib();

	void			Scan();
	void			Load();

	void			Print(const QString& sText);
	void			SetProgress(int nProgress);
	void			NewImage(QPixmap& pixmap);
	void			SetFile(const QString& sFile);
	void			SetStatus(const QString& sStatus);

	void			SetSettings(TcSettings* pSettings)		{  _pSettings =pSettings; }
	TcSettings*		GetSettings()							{ return _pSettings;	  }

	GpsFrame*		FindGpsFrame(const GpsDateTime& gTime);

public slots:
	void	OnLoadStart();
	void	OnLoadEnd();

signals:
	void	sigProgress(int nProgress);
	void	sigPushText(QString sText);
	void	sigSetFile(QString sFile);
	void	sigNewImage(QPixmap pixmap);
	void	sigSetStatus(QString sText);

public:
	TcSettings*		_pSettings;
	QList<GpsLog*>	_lGpsLogs;
	QList<TcPhoto*>	_lPhotos;
	QThread			_thread;
	int				_nProgressCurrent;
};

#endif // GPSLOGLIB_H
