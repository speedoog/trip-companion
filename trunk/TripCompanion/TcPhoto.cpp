#include "tcphoto.h"

#include <math.h>

#include <QPixmap>
#include <QImage>
#include <QDate>
#include <QDir>
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

static char * pMonthName[12] =
{
   "janvier", "fevrier", "mars", "avril", "mai", "juin",
   "juillet", "aout", "septembre", "octobre", "novembre", "decembre"
};

static char * pDayName[7] =
{
	"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche"
};

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
	_pLib->Print("");
	_pLib->Print("Batch File "+_sFilename);

	QByteArray sUTF8 = sFullFilename.toUtf8();
	Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open( sUTF8.data() );
	image->readMetadata();

	Exiv2::ExifData& exifData = image->exifData();
	Exiv2::Exifdatum& tagDate = exifData["Exif.Photo.DateTimeOriginal"];		// http://www.exiv2.org/tags.html

	QString	sExifDate(tagDate.toString().c_str());

	GpsDateTime	gtOriginal;
	gtOriginal.FromExifDate(sExifDate);

	_pLib->SetStatus("FindGpsFrame");
	GpsFrame* pGpsFrame =_pLib->FindGpsFrame(gtOriginal);
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

	GpsDateTime	gtFix =gtOriginal;
	// ----------- Fix Time Date
	{
		int nTimeOffset =_pLib->_pSettings->_nTimeOffset;
		if (_pLib->_pSettings->_bSummerTime)
		{
			++nTimeOffset;
		}
		gtFix.FixOffsetTime(nTimeOffset);
		gtFix.ToExifDate(sExifDate);				// patch exif date
		tagDate =std::string(sExifDate.toUtf8().data());
	}

	// ------------ Exif Copyright
	{
//		exifData["Exif.Image.ImageDescription"] = "Nos Aventures aux Amériques";				// surtout pas !
		exifData["Exif.Photo.UserComment"]		= "http://www.nosaventuresauxameriques.com";
		exifData["Exif.Image.XPComment"]		= "http://www.nosaventuresauxameriques.com";
		exifData["Exif.Image.Copyright"]		= "Copyright Claire Masson & Bertrand Faure";
	}

	TcSettings* pSettings =_pLib->GetSettings();

	QImage qimageSrc, qimageDst;
	// ----------- ReadFromFile
	{
		_pLib->SetStatus("Read");
		_pLib->Print("   Read Jpeg "+_sFilename);
		qimageSrc.load(sFullFilename);
	}

	// ----------- Resize
	{
		_pLib->SetStatus("Resize");
		_pLib->Print("   Resize");
		const int nSizeSrcX =qimageSrc.width();
		const int nSizeSrcY =qimageSrc.height();
		int nSizeDstX, nSizeDstY;
		ResizeMegaPix(_pLib->_pSettings->_fDstMegaPix, nSizeSrcX, nSizeSrcY, nSizeDstX, nSizeDstY);
		qimageDst =qimageSrc.scaled(nSizeDstX, nSizeDstY, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}

	// ----------- Thmubnail
	{
		_pLib->SetStatus("Thmubnail");
		_pLib->Print("   Thumb");

		// get label dimensions
		QLabel* pLabelPreview =pSettings->GetLabelPreview();
		int w = pLabelPreview->width();
		int h = pLabelPreview->height();

		// set a scaled pixmap to a w x h window keeping its aspect ratio
		QImage qimageThumb =qimageDst.scaled(w,h,Qt::KeepAspectRatio);
		QPixmap qPixThumb =QPixmap::fromImage(qimageThumb);
		_pLib->NewImage(qPixThumb);
	}

	QString sOutFile;
	// ----------- Write out file
	{
		_pLib->SetStatus("Save");
		_pLib->Print("   SaveToFile");
		QDate qdate(gtFix._nYear, gtFix._nMonth, gtFix._nDay);
		QString sDirectoryOut;
		sDirectoryOut.sprintf("%d %03d - ", gtFix._nYear, qdate.dayOfYear());
		int nDayOfWeek =qdate.dayOfWeek();
		sDirectoryOut +=pDayName[nDayOfWeek-1];
		sDirectoryOut +=" ";
		sDirectoryOut +=QString::number(gtFix._nDay);
		sDirectoryOut +=" ";
		sDirectoryOut +=pMonthName[qdate.month()-1];

		QString sFileOutDir =pSettings->_sDstJpg+"/"+sDirectoryOut;
		QDir	dirOut(sFileOutDir);
		if (dirOut.exists()==false)
		{
			dirOut.mkpath(".");
		}

		sOutFile =sFileOutDir+"/s"+_sFilename;
		qimageDst.save(sOutFile, "JPG", _pLib->_pSettings->_nCompression);
	}

	// ----------- Write exif
	_pLib->SetStatus("Write exif");
	QByteArray sOutUTF8 = sOutFile.toUtf8();
	Exiv2::Image::AutoPtr imageDst = Exiv2::ImageFactory::open( sOutUTF8.data() );
	imageDst->setExifData(exifData);
	imageDst->writeMetadata();
}
