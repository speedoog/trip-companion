#include "tcphoto.h"

#include <math.h>

#include <QPixmap>
#include <QImage>
#include <qnamespace.h>

#include "TcLib.h"
#include "TcSettings.h"
#include "GpsDateTime.h"
#include "GpsFrame.h"

#include "exiv/exiv2.hpp"

//#include "jpeg/jpgd.h"
//#include "jpeg/jpge.h"
//#include "resampler/resampler.h"
//#include "resampler/JPEGImage.h"


TcPhoto::TcPhoto(TcLib* pLib, const QString& sPath, const QString& sFilename)
: _pLib		(pLib)
, _sPath	(sPath)
, _sFilename(sFilename)
{
}

TcPhoto::~TcPhoto()
{
}

// Tag list : http://www.exiv2.org/tags.html

void ResizeMegaPix(const float fDstMegaPixel, const int nSizeSrcX, const int nSizeSrcY, int& nSizeDstX, int& nSizeDstY)
{
	float fSrcMegaPixel =float(nSizeSrcX*nSizeSrcY)/(1000.0f*1000.0f);
	float rRatio =fDstMegaPixel/fSrcMegaPixel;

	float rScale =sqrtf(rRatio);

	nSizeDstX =int(float(nSizeSrcX)*rScale)&0xFFFFFFF0;		// round to 16
	nSizeDstY =int(float(nSizeSrcY)*rScale)&0xFFFFFFF0;
}

void TcPhoto::Batch()
{
	_pLib->SetFile(_sFilename);

	QString sFullFilename =_sPath+"/"+_sFilename;

	_pLib->SetStatus("Read exif");
//	_pLib->SetProgress(0);
	_pLib->Print("");
	_pLib->Print("Batch File "+_sFilename);
	QByteArray sUTF8 = sFullFilename.toUtf8();
	Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open( sUTF8.data() );
	image->readMetadata();

	Exiv2::ExifData& exifData = image->exifData();
	Exiv2::Exifdatum& tagDate = exifData["Exif.Photo.DateTimeOriginal"];		// http://www.exiv2.org/tags.html

	QString	sExifDate(tagDate.toString().c_str());

	GpsDateTime	gTime;
	gTime.FromExifDate(sExifDate);

	_pLib->SetStatus("FindGpsFrame");
//	_pLib->SetProgress(10);
	GpsFrame* pGpsFrame =_pLib->FindGpsFrame(gTime);
	if (pGpsFrame)
	{
		const int BUFFER_SIZE =128;
		char tmp[BUFFER_SIZE];

		// ------- Altitude -------
		{
			snprintf(tmp, BUFFER_SIZE, "%d/1", (int)pGpsFrame->_dAltitude);
			exifData["Exif.GPSInfo.GPSAltitude"]	= std::string(tmp);
			exifData["Exif.GPSInfo.GPSAltitudeRef"] = (pGpsFrame->_dAltitude < 0) ? "1" : "0";
		}

		// ------- Latitude -------
		{
			double latitude = fabs(pGpsFrame->_dLatitudeValue);
			double num1Lat  = floor(latitude);
			double num2Lat  = floor((latitude - num1Lat) * 60.0);
			double num3Lat  = (latitude - (num1Lat+(num2Lat/60.0))) * 3600.0;
			snprintf(tmp, BUFFER_SIZE, "%d/1 %d/1 %d/16384", int(num1Lat), int(num2Lat), int(num3Lat*16384.0));
			exifData["Exif.GPSInfo.GPSLatitude"]	= std::string(tmp);

			tmp[0] =pGpsFrame->_nLatitudeSign;	tmp[1] =0;
			exifData["Exif.GPSInfo.GPSLatitudeRef"] = std::string(tmp);
		}

		// ------- Longitude -------
		{
			double longitude = fabs(pGpsFrame->_dLongitudeValue);
			double num1Lon	 = floor(longitude);
			double num2Lon	 = floor((longitude - num1Lon) * 60.0);
			double num3Lon	 = (longitude - (num1Lon+(num2Lon/60.0))) * 3600.0;
			snprintf(tmp, BUFFER_SIZE, "%d/1 %d/1 %d/16384", int(num1Lon), int(num2Lon), int(num3Lon*16384.0));
			exifData["Exif.GPSInfo.GPSLongitude"]	= std::string(tmp);
			tmp[0] =pGpsFrame->_nLongitudeSign; tmp[1]=0;
			exifData["Exif.GPSInfo.GPSLongitudeRef"] =std::string(tmp);
		}
		_pLib->Print("   found " + QString::number(pGpsFrame->_dLatitudeValue) + " " +  QString::number(pGpsFrame->_dLongitudeValue) );
	}

	// ----------- Fix Time Date
	{
		GpsDateTime	gTimeFix =gTime;
		int nTimeOffset =_pLib->_pSettings->_nTimeOffset;
		if (_pLib->_pSettings->_bSummerTime)
		{
			++nTimeOffset;
		}
		gTimeFix.FixOffsetTime(nTimeOffset);
		gTimeFix.ToExifDate(sExifDate);				// patch exif date

		tagDate =std::string(sExifDate.toUtf8().data());
	}

	// ------------ Exif Copyright
	{
		exifData["Exif.Image.ImageDescription"] = "Nos Aventures aux Amériques";
		exifData["Exif.Photo.UserComment"]		= "http://www.nosaventuresauxameriques.com";
		exifData["Exif.Image.XPComment"]		= "http://www.nosaventuresauxameriques.com";
		exifData["Exif.Image.Copyright"]		= "Copyright Claire Masson & Bertrand Faure";
	}

	// ----------- ReadFromFile
	_pLib->SetStatus("Read");
//	_pLib->SetProgress(20);
	_pLib->Print("   Read Jpeg "+_sFilename);

	QImage qimageSrc(sFullFilename);

	// ----------- Resize
	_pLib->SetStatus("Resize");
//	_pLib->SetProgress(50);
	_pLib->Print("   Resize");
	const int nSizeSrcX =qimageSrc.width();
	const int nSizeSrcY =qimageSrc.height();
	int nSizeDstX, nSizeDstY;
	ResizeMegaPix(_pLib->_pSettings->_fDstMegaPix, nSizeSrcX, nSizeSrcY, nSizeDstX, nSizeDstY);
	QImage qimageDst =qimageSrc.scaled(nSizeDstX, nSizeDstY, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	// ----------- Thmubnail
	_pLib->SetStatus("Thmubnail");
//	_pLib->SetProgress(70);
	_pLib->Print("   Thumb");
	QImage qimageThumb =qimageDst.scaledToHeight(300);
	QPixmap qPixThumb =QPixmap::fromImage(qimageThumb);
	_pLib->NewImage(qPixThumb);

	// ----------- Write out file
	_pLib->SetStatus("Save");
//	_pLib->SetProgress(80);
	_pLib->Print("   SaveToFile");

	QString sOutFile =_pLib->GetSettings()->_sDstJpg+"/s"+_sFilename;

	qimageDst.save(sOutFile, "JPG", _pLib->_pSettings->_nCompression);

	// ----------- Write exif
	_pLib->SetStatus("Write exif");
//	_pLib->SetProgress(90);
	QByteArray sOutUTF8 = sOutFile.toUtf8();
	Exiv2::Image::AutoPtr imageDst = Exiv2::ImageFactory::open( sOutUTF8.data() );
	imageDst->setExifData(exifData);
	imageDst->writeMetadata();

//	_pLib->SetProgress(100);
}

/*
void TcPhoto::LoadExif()
{
	_pLib->SetFile(_sFilename);

	//----------------------------------- test read
	{
//		Exiv2::Image::AutoPtr imagetest = Exiv2::ImageFactory::open( "c:/Users/bfa/Downloads/-- Trip --/JpgOut/sSAM_0823.JPG" );
//		imagetest->readMetadata();
//		Exiv2::ExifData& exifDatatest = imagetest->exifData();
//		Exiv2::Exifdatum& tagtest = exifDatatest["Exif.GPSInfo.GPSLatitude"];
//		QString	sTest(tagtest.toString().c_str());
	}

	//----------------------------------- test

	QString sFullFilename =_sPath+_sFilename;

	QByteArray sUTF8 = sFullFilename.toUtf8();
	Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open( sUTF8.data() );
	image->readMetadata();

	Exiv2::ExifData& exifData = image->exifData();
	Exiv2::Exifdatum& tagDate = exifData["Exif.Photo.DateTimeOriginal"];		// http://www.exiv2.org/tags.html

	QString	sDate(tagDate.toString().c_str());

	GpsDateTime	gTime;
	gTime.FromExifDate(sDate);
	_pLib->Print( QString("%1 > %2").arg(_sFilename).arg(sDate) );

	GpsFrame* pGpsFrame =_pLib->FindGpsFrame(gTime);
	if (pGpsFrame)
	{
		const int BUFFER_SIZE =128;
		char tmp[BUFFER_SIZE];

		// ------- Altitude -------
		{
			snprintf(tmp, BUFFER_SIZE, "%d/1", (int)pGpsFrame->_dAltitude);
			exifData["Exif.GPSInfo.GPSAltitude"]	= std::string(tmp);
			exifData["Exif.GPSInfo.GPSAltitudeRef"] = (pGpsFrame->_dAltitude < 0) ? "1" : "0";
		}

		// ------- Latitude -------
		{
			double latitude = fabs(pGpsFrame->_dLatitudeValue);
			double num1Lat  = floor(latitude);
			double num2Lat  = floor((latitude - num1Lat) * 60.0);
			double num3Lat  = (latitude - (num1Lat+(num2Lat/60.0))) * 3600.0;
			snprintf(tmp, BUFFER_SIZE, "%d/1 %d/1 %d/16384", int(num1Lat), int(num2Lat), int(num3Lat*16384.0));
			exifData["Exif.GPSInfo.GPSLatitude"]	= std::string(tmp);

			tmp[0] =pGpsFrame->_nLatitudeSign;	tmp[1] =0;
			exifData["Exif.GPSInfo.GPSLatitudeRef"] = std::string(tmp);
		}

		// ------- Longitude -------
		{
			double longitude = fabs(pGpsFrame->_dLongitudeValue);
			double num1Lon	 = floor(longitude);
			double num2Lon	 = floor((longitude - num1Lon) * 60.0);
			double num3Lon	 = (longitude - (num1Lon+(num2Lon/60.0))) * 3600.0;
			snprintf(tmp, BUFFER_SIZE, "%d/1 %d/1 %d/16384", int(num1Lon), int(num2Lon), int(num3Lon*16384.0));
			exifData["Exif.GPSInfo.GPSLongitude"]	= std::string(tmp);
			tmp[0] =pGpsFrame->_nLongitudeSign; tmp[1]=0;
			exifData["Exif.GPSInfo.GPSLongitudeRef"] =std::string(tmp);
		}
	}

	_pLib->SetStatus("Read");
	_pLib->Print("   ReadFromFile");

	JPEG_IMAGE jpg;
	jpg.ReadFromFile(sUTF8.data());

	_pLib->SetStatus("Resize");
	_pLib->Print("   Resize");
	jpg.ResizeMpix(_pLib->_pSettings->_fDstMegaPix);

	// ------ Thmubnail
	_pLib->SetStatus("Thmubnail");
	_pLib->Print("   Thumb");
	QImage	qimage((uchar*)jpg.GetData(), jpg.Width(), jpg.Height(), QImage::Format_RGB888);
	QPixmap pixmap;
	pixmap.convertFromImage(qimage);

	int orientation=0;
	Exiv2::ExifKey keyStd("Exif.Image.Orientation");
	Exiv2::ExifData::iterator it = exifData.findKey(keyStd);
	if (it != exifData.end() && it->count())
	{
		orientation = it->toLong();
	}
	switch (orientation)
	{
	case 3:				// 180
	{
		QTransform rot =QTransform().rotate(180);
		pixmap =pixmap.transformed(rot);
		break;
	}
	case 6:				// 90
	{
		QTransform rot =QTransform().rotate(90);
		pixmap =pixmap.transformed(rot);
		break;
	}
	case 8:				// -90
	{
		QTransform rot =QTransform().rotate(-90);
		pixmap =pixmap.transformed(rot);
		break;
	}
	default:
		break;
	}

	pixmap =pixmap.scaledToHeight(300);

	_pLib->NewImage(pixmap);


	// ------------ write out
	_pLib->SetStatus("Save");
	_pLib->Print("   SaveToFile");

	QString sOutFile =_pLib->GetSettings()->_sDstJpg+"s"+_sFilename;
	QByteArray sOutUTF8 = sOutFile.toUtf8();
	const char* pOutFilename =sOutUTF8.data();

	jpg.SaveToFile(pOutFilename, _pLib->_pSettings->_nCompression);

	_pLib->SetStatus("CopyExif");
	// copy exif
	Exiv2::Image::AutoPtr imageDst = Exiv2::ImageFactory::open( sOutUTF8.data() );
	imageDst->setExifData(exifData);
	imageDst->writeMetadata();
}
*/

