#include "GpsLog.h"

#include <QFile>
#include <QXmlStreamWriter>
#include <QFileInfo>

#include "GpsFrame.h"
#include "TcLib.h"
#include "TcSettings.h"

GpsLog::GpsLog(TcLib* pLib, const QString& sFilename)
: _pLib(pLib)
, _sFilename(sFilename)
{

}

GpsLog::~GpsLog()
{
}

void GpsLog::Load()
{
	_nTimeStart	=0;
	_nTimeEnd	=0;

	QFile file(_pLib->_pSettings->_sSrcLog + _sFilename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
        _pLib->Print("Failed to open " + _sFilename);
		return;
	}

    _pLib->SetFile(_sFilename);
	_pLib->SetStatus("Read LOG");
    _pLib->Print("Read " + _sFilename);

	const int nTotalSize =file.size();

	GpsFrame* pGpsFrameLast =NULL;
	GpsFrame::GpsFrameType	nTypeLast =GpsFrame::GF_OTHER;
	int			nPackedTimeLast	=0;
	while (!file.atEnd())
	{
		QString		sLine =QString(file.readLine());
		QStringList slSplit =sLine.split(",");

		GpsFrame::GpsFrameType nType =GpsFrame::GetFrameType(slSplit);
		int			nPackedTime	=0;

		GpsFrame* pGpsFrame =NULL;
		switch(nType)
		{
			case GpsFrame::GF_GGA:
			{
				nPackedTimeLast =GpsFrame::GetPackedTime(slSplit);
				pGpsFrame =new GpsFrame;
				pGpsFrame->FeedWithFrameGPGGA(slSplit);
				_lFrames.push_back(pGpsFrame);
			}
			break;

			case GpsFrame::GF_RMC:
			{
				nPackedTime =GpsFrame::GetPackedTime(slSplit);
				if (pGpsFrameLast && nTypeLast==GpsFrame::GF_GGA && nPackedTime==nPackedTimeLast)
				{
					// update GGA frame with date
					pGpsFrame =pGpsFrameLast;
					pGpsFrame->FeedWithFrameGPRMC(slSplit);
				}
			}
			break;
		default:;
		}

		nTypeLast	  =nType;
		pGpsFrameLast =pGpsFrame;

		int nPos =file.pos();
		_pLib->SetProgress((100*nPos)/nTotalSize);
	}

	if (_lFrames.size()>2)
	{
		_nTimeStart =_lFrames[0]->_Time.GetPacked();
		_nTimeEnd	=_lFrames.last()->_Time.GetPacked();
	}

	_pLib->Print(QString("   > %1 frames").arg(_lFrames.size()));
}

GpsFrame* GpsLog::FindGpsFrame(const GpsTime& gTime)
{
	const qint64 n64TimeFind =gTime.GetPacked();
	if (n64TimeFind>=_nTimeStart && n64TimeFind<=_nTimeEnd)
	{
		for (int i = 0; i < _lFrames.size(); ++i)
		{
			GpsFrame* pGpsFrame =_lFrames[i];
			qint64 n64Time =pGpsFrame->_Time.GetPacked();
			if (n64Time>=n64TimeFind)
			{
				return pGpsFrame;
			}
		}
	}

	return NULL;
}

/*
	 *  ------------------ GPX FILE ------------------

	<?xml version="1.0" encoding="UTF-8"?>
	<gpx
		version="1.0"
		creator="GPSBabel - http://www.gpsbabel.org"
		xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
		xmlns="http://www.topografix.com/GPX/1/0"
		xsi:schemaLocation="http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd">
		<time>2013-08-11T22:02:30Z</time>
		<bounds minlat="48.841241667" minlon="2.416945000" maxlat="48.853498333" maxlon="2.439183333"/>
		<trk>
			<trkseg>
				<trkpt lat="48.845960000" lon="2.424243333">
					<ele>57.600000</ele>
					<time>2013-08-11T19:53:55Z</time>
					<course>77.099998</course>
					<speed>3.009500</speed>
					<fix>3d</fix>
					<sat>5</sat>
					<hdop>3.000000</hdop>
				</trkpt>
			</trkseg>
		</trk>
	</gpx>

	 *  ------------------ GPX FILE ------------------
*/

void GpsLog::WriteGPX()
{
	QFileInfo fileinfo(_sFilename);

    QString	sGpxShort =fileinfo.baseName() +".gpx";
    QString	sGpxFilename =_pLib->_pSettings->_sDstGpx + sGpxShort;

	QFile file(sGpxFilename);
	if (!file.open(QIODevice::WriteOnly) || _lFrames.size()<2)
	{
        _pLib->Print("Failed to open " + sGpxShort);
		return;		// fail
	}

    _pLib->SetFile(sGpxShort);
    _pLib->SetStatus("Write GPX");
    _pLib->Print("Write " + sGpxShort);

	QXmlStreamWriter xmlWriter(&file);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();
	{
		xmlWriter.writeStartElement("gpx");
		{
			xmlWriter.writeAttribute("version",				"1.0");
			xmlWriter.writeAttribute("creator",				"Trip Companion");
			xmlWriter.writeAttribute("xmlns:xsi",			"http://www.w3.org/2001/XMLSchema-instance");
			xmlWriter.writeAttribute("xmlns",				"http://www.topografix.com/GPX/1/0");
			xmlWriter.writeAttribute("xsi:schemaLocation",	"http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd");
			GpsFrame* p0 =_lFrames[0];
			QString	sDateTime;
			p0->ToGpxDate(sDateTime);
			xmlWriter.writeTextElement("time", sDateTime);
			xmlWriter.writeStartElement("trk");
			{
				xmlWriter.writeStartElement("trkseg");
				{
                    const int nCount =_lFrames.size();
                    for (int i = 0; i < nCount; ++i)
					{
						GpsFrame* pGpsFrame =_lFrames[i];

						xmlWriter.writeStartElement("trkpt");
						{
							xmlWriter.writeAttribute("lat", QString::number(pGpsFrame->_dLatitudeValue) );
							xmlWriter.writeAttribute("lon",	QString::number(pGpsFrame->_dLongitudeValue));

							xmlWriter.writeTextElement("ele", QString::number(pGpsFrame->_dAltitude) );

							pGpsFrame->ToGpxDate(sDateTime);
							xmlWriter.writeTextElement("time", sDateTime);
						}
						xmlWriter.writeEndElement();
                        _pLib->SetProgress((i*100)/nCount);
					}
				}
				xmlWriter.writeEndElement();
			}
			xmlWriter.writeEndElement();
		}
		xmlWriter.writeEndElement();
	}
	xmlWriter.writeEndDocument();

	file.close();

    _pLib->Print("End Write " + sGpxShort);
}

	/*
	 *  ------------------ KML FILE ------------------

	<?xml version="1.0" encoding="UTF-8"?>
	<kml xmlns="http://earth.google.com/kml/2.2">
		<Document xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
			<name>test__picnic.gpx</name>
			<Style id="lineStyle">
				<LineStyle>
					<color>99ffac59</color>
					<width>6</width>
				</LineStyle>
			</Style>
			<Placemark>
				<name>picnic</name>
				<styleUrl>#lineStyle</styleUrl>
				<LineString>
					<coordinates>
						2.439183333,48.853498333,
						2.439003333,48.853016667,48
						2.424243333,48.845960000,58
					</coordinates>
				</LineString>
			</Placemark>
		</Document>
	</kml>

	 *  ------------------ KML FILE ------------------
	 */

void GpsLog::WriteKML()
{
	QFileInfo fileinfo(_sFilename);

	QString sBaseName =fileinfo.baseName();
    QString	sKmlShort =sBaseName +".kml";
    QString	sKmlFilename =_pLib->_pSettings->_sDstKml + sKmlShort;

    QFile file(sKmlFilename);
	if (!file.open(QIODevice::WriteOnly) || _lFrames.size()<2)
	{
        _pLib->Print("Failed to open " + sKmlFilename);
		return;		// fail
	}

    _pLib->SetFile(sKmlShort);
    _pLib->SetStatus("Write KML");
    _pLib->Print("Write " + sKmlShort);

	QXmlStreamWriter xmlWriter(&file);
	xmlWriter.setAutoFormatting(true);
	xmlWriter.writeStartDocument();
	{
		xmlWriter.writeStartElement("kml");
		{
			xmlWriter.writeAttribute("xmlns",	"http://earth.google.com/kml/2.2");

			xmlWriter.writeStartElement("Document");
			{
				// ------ styles
				xmlWriter.writeAttribute("xmlns:xsi",	"http://www.w3.org/2001/XMLSchema-instance");
				xmlWriter.writeTextElement("name", sBaseName);

				xmlWriter.writeStartElement("Style");
				{
					xmlWriter.writeAttribute("id",	"lineStyle");

					xmlWriter.writeStartElement("LineStyle");
					{
						xmlWriter.writeTextElement("color",	"99ffac59");
						xmlWriter.writeTextElement("width",	"6");
					}
					xmlWriter.writeEndElement();

				}
				xmlWriter.writeEndElement();

				//--- Placemark
				xmlWriter.writeStartElement("Placemark");
				{
					xmlWriter.writeTextElement("name",	   sBaseName);
					xmlWriter.writeTextElement("styleUrl", "#lineStyle");

					xmlWriter.writeStartElement("LineString");
					{
                        const int nCount =_lFrames.size();

						QString sText;
                        const int nReserve =23*nCount;
						sText.reserve(nReserve);
                        for (int i = 0; i < nCount; ++i)
						{
							GpsFrame* pGpsFrame =_lFrames[i];
							QString sTemp;
							sTemp.sprintf(" %.6lf,%.6lf,%d", pGpsFrame->_dLongitudeValue, pGpsFrame->_dLatitudeValue, int(pGpsFrame->_dAltitude) );
							sText +=sTemp;

                            _pLib->SetProgress((i*100)/nCount);
						}
						xmlWriter.writeTextElement("coordinates", sText);
					}
					xmlWriter.writeEndElement();
				}
				xmlWriter.writeEndElement();
			}
			xmlWriter.writeEndElement();
		}
		xmlWriter.writeEndElement();
	}
	xmlWriter.writeEndDocument();

	file.close();
}











