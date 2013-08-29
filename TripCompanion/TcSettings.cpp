#include "TcSettings.h"
#include "ui_Settings.h"

#include <QSettings>

#ifdef WIN32
	QString	sRootCfg		="C:/Users/bfa/Downloads/-- GPS --/";
#else
	QString	sRootCfg		="/sdcard/download/GPS/";
#endif

TcSettings::TcSettings(MainWindow* pMainWindow)
: QObject()
, ui(pMainWindow->ui)
{
//	ui->setupUi(this);

	// default
#ifdef WIN32
	_sSrcLog	="C:/Users/bfa/Downloads/-- Trip --/Log/";
	_sSrcJpg	="C:/Users/bfa/Downloads/-- Trip --/JpgIn/";
	_sDstGpx	="C:/Users/bfa/Downloads/-- Trip --/Gpx/";
	_sDstKml	="C:/Users/bfa/Downloads/-- Trip --/Kml/";
	_sDstJpg	="C:/Users/bfa/Downloads/-- Trip --/JpgOut/";
#else
	_sSrcLog	="/sdcard/download/Trip/Log/";
	_sSrcJpg	="/sdcard/download/Trip/JpgIn/";
	_sDstGpx	="/sdcard/download/Trip/Gpx/";
	_sDstKml	="/sdcard/download/Trip/Kml/";
	_sDstJpg	="/sdcard/download/Trip/JpgOut/";
#endif

	_fDstMegaPix	=1.0f;
	_nCompression	=75;

	Load();

	// connect after load (avoid feedback)
	connect(ui->editLOG,		SIGNAL(textChanged(QString)), this, SLOT(OnEditSrcLog(QString))			);
	connect(ui->editJPG,		SIGNAL(textChanged(QString)), this, SLOT(OnEditSrcJpg(QString))			);

	connect(ui->editGPX,		SIGNAL(textChanged(QString)), this, SLOT(OnEditDstGpx(QString))			);
	connect(ui->editKML,		SIGNAL(textChanged(QString)), this, SLOT(OnEditDstKml(QString))			);
	connect(ui->editJPG2,		SIGNAL(textChanged(QString)), this, SLOT(OnEditDstJpg(QString))			);

	connect(ui->editComp,		SIGNAL(textChanged(QString)), this, SLOT(OnEditCompression(QString))	);
	connect(ui->editMegaPixels, SIGNAL(textChanged(QString)), this, SLOT(OnEditMegaPixels(QString))		);

}

TcSettings::~TcSettings()
{
}

void TcSettings::Load()
{
	QSettings set("automagic", "TripCompanion", this);

	if (set.contains("Compression"))
	{
		_sSrcLog		= set.value("SrcLog").toString();
		_sSrcJpg		= set.value("SrcJpg").toString();

		_sDstGpx		= set.value("DstGpx").toString();
		_sDstKml		= set.value("DstKml").toString();
		_sDstJpg		= set.value("DstJpg").toString();

		_nCompression	= set.value("Compression").toInt();
		_fDstMegaPix	= set.value("Megapixels").toFloat();
	}

	ui->editLOG->setText(_sSrcLog);
	ui->editJPG->setText(_sSrcJpg);

	ui->editGPX->setText(_sDstGpx);
	ui->editKML->setText(_sDstKml);
	ui->editJPG2->setText(_sDstJpg);

	ui->editComp->setText(QString::number(_nCompression));
	ui->editMegaPixels->setText(QString::number(_fDstMegaPix));
}

void TcSettings::Save()
{
	QSettings set("automagic", "TripCompanion", this);

	set.setValue("SrcLog",		_sSrcLog		);
	set.setValue("SrcJpg",		_sSrcJpg		);

	set.setValue("DstGpx",		_sDstGpx		);
	set.setValue("DstKml",		_sDstKml		);
	set.setValue("DstJpg",		_sDstJpg		);

	set.setValue("Compression",	_nCompression	);
	set.setValue("Megapixels",	_fDstMegaPix	);
}

void TcSettings::OnEditSrcLog(QString sSrcLog)
{
	_sSrcLog =sSrcLog;
}

void TcSettings::OnEditSrcJpg(QString sSrcJpg)
{
	_sSrcJpg =sSrcJpg;
}
void TcSettings::OnEditDstGpx(QString sDstGpx)
{
	_sDstGpx =sDstGpx;
}
void TcSettings::OnEditDstKml(QString sDstKml)
{
	_sDstKml =sDstKml;
}
void TcSettings::OnEditDstJpg(QString sDstJpg)
{
	_sDstJpg =sDstJpg;
}

void TcSettings::OnEditCompression(QString sCompression)
{
	_nCompression =sCompression.toInt();
}

void TcSettings::OnEditMegaPixels(QString sMegaPixels)
{
	_fDstMegaPix  =sMegaPixels.toFloat();
}
