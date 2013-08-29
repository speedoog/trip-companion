#ifndef GPSLOG_H
#define GPSLOG_H

#include <QList>
#include <QThread>

class GpsFrame;
class GpsTime;
class TcLib;

class GpsLog
{
public:
                GpsLog(TcLib* pLib, const QString& sFilename);
				~GpsLog();

	void		Load();
	GpsFrame*	FindGpsFrame(const GpsTime& gTime);

	void		WriteGPX();
	void		WriteKML();

protected:
    TcLib*               _pLib;
	QString				_sFilename;
	QList<GpsFrame*>	_lFrames;

	qint64				_nTimeStart, _nTimeEnd;
};

#endif // GPSLOG_H
