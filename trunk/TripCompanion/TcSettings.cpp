#include "TcSettings.h"
#include "ui_Settings.h"

#include <QSettings>
#include <QFileDialog>

#ifdef WIN32
	QString	sRootCfg		="C:/Users/bfa/Downloads/-- GPS --/";
#else
	QString	sRootCfg		="/sdcard/download/GPS/";
#endif

TcSettings::TcSettings(MainWindow* pMainWindow)
: QObject()
, ui(pMainWindow->ui)
, _pMainWindow(pMainWindow)
{

	// default
#ifdef WIN32
	_sSrcLog	="C:/Users/bfa/Downloads/-- Trip --/Log";
	_sSrcJpg	="C:/Users/bfa/Downloads/-- Trip --/JpgIn";
	_sDstGpx	="C:/Users/bfa/Downloads/-- Trip --/Gpx";
	_sDstKml	="C:/Users/bfa/Downloads/-- Trip --/Kml";
	_sDstJpg	="C:/Users/bfa/Downloads/-- Trip --/JpgOut";
#else
	_sSrcLog	="/sdcard/download/Trip/Log";
	_sSrcJpg	="/sdcard/download/Trip/JpgIn";
	_sDstGpx	="/sdcard/download/Trip/Gpx";
	_sDstKml	="/sdcard/download/Trip/Kml";
	_sDstJpg	="/sdcard/download/Trip/JpgOut";
#endif

	_fDstMegaPix	=1.0f;
	_nCompression	=75;
	_nTimeOffset	=1;
	_bSummerTime	=false;

	ui->comboTimeOffset->addItem("-12",										-12 );
	ui->comboTimeOffset->addItem("-11",										-11 );
	ui->comboTimeOffset->addItem("-10",										-10 );
	ui->comboTimeOffset->addItem("-9 (Alaska)",								-9  );
	ui->comboTimeOffset->addItem("-8 (USA/Canada Côte Ouest)",				-8  );
	ui->comboTimeOffset->addItem("-7 (USA/Canada Mid Ouest)",				-7  );
	ui->comboTimeOffset->addItem("-6 (Mexique, Canada Mid Est)",			-6  );
	ui->comboTimeOffset->addItem("-5 (Equateur, Pérou, USA/Canada Est)",	-5  );
	ui->comboTimeOffset->addItem("-4 (Bolivie, Chili)",						-4  );
	ui->comboTimeOffset->addItem("-3 (Argentine)",							-3  );
	ui->comboTimeOffset->addItem("-2",										-2  );
	ui->comboTimeOffset->addItem("-1",										-1  );
	ui->comboTimeOffset->addItem(" 0 (UK)",									 0  );
	ui->comboTimeOffset->addItem("+1 (France)",								 1  );
	ui->comboTimeOffset->addItem("+2",										 2  );
	ui->comboTimeOffset->addItem("+3",										 3  );
	ui->comboTimeOffset->addItem("+4",										 4  );
	ui->comboTimeOffset->addItem("+5",										 5  );
	ui->comboTimeOffset->addItem("+6",										 6  );
	ui->comboTimeOffset->addItem("+7",										 7  );
	ui->comboTimeOffset->addItem("+8",										 8  );
	ui->comboTimeOffset->addItem("+9",										 9  );
	ui->comboTimeOffset->addItem("+10",										10	);
	ui->comboTimeOffset->addItem("+11",										11  );
	ui->comboTimeOffset->addItem("+12",										12  );

	Load();

	// connect after load (avoid feedback)
	connect(ui->editLOG,		SIGNAL(textChanged(QString)),		this, SLOT(OnEditSrcLog(QString))			);
	connect(ui->btnLOG,			SIGNAL(clicked()),					this, SLOT(OnClickSrcLog())					);

	connect(ui->editJPG,		SIGNAL(textChanged(QString)),		this, SLOT(OnEditSrcJpg(QString))			);
	connect(ui->btnJPG,			SIGNAL(clicked()),					this, SLOT(OnClickSrcJpg())					);

	connect(ui->editGPX,		SIGNAL(textChanged(QString)),		this, SLOT(OnEditDstGpx(QString))			);
	connect(ui->btnGPX,			SIGNAL(clicked()),					this, SLOT(OnClickDstGpx())					);

	connect(ui->editKML,		SIGNAL(textChanged(QString)),		this, SLOT(OnEditDstKml(QString))			);
	connect(ui->btnKML,			SIGNAL(clicked()),					this, SLOT(OnClickDstKml())					);

	connect(ui->editJPG2,		SIGNAL(textChanged(QString)),		this, SLOT(OnEditDstJpg(QString))			);
	connect(ui->btnJPG2,		SIGNAL(clicked()),					this, SLOT(OnClickDstJpg())					);

	connect(ui->editComp,		SIGNAL(textChanged(QString)),		this, SLOT(OnEditCompression(QString))		);
	connect(ui->editMegaPixels, SIGNAL(textChanged(QString)),		this, SLOT(OnEditMegaPixels(QString))		);

	connect(ui->comboTimeOffset,SIGNAL(currentIndexChanged(int)),	this, SLOT(OnTimeOffsetChanged(int))		);
	connect(ui->checkSummerTime,SIGNAL(stateChanged(int)),			this, SLOT(OnSummerTimeChanged(int))		);

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
		_nTimeOffset	= set.value("TimeOffset").toInt();
		_bSummerTime	= set.value("SummerTime").toBool();
	}

	ui->editLOG->setText(_sSrcLog);
	ui->editJPG->setText(_sSrcJpg);

	ui->editGPX->setText(_sDstGpx);
	ui->editKML->setText(_sDstKml);
	ui->editJPG2->setText(_sDstJpg);

	ui->editComp->setText(QString::number(_nCompression));
	ui->editMegaPixels->setText(QString::number(_fDstMegaPix));

	int index =ui->comboTimeOffset->findData(_nTimeOffset);
	ui->comboTimeOffset->setCurrentIndex(index);

	ui->checkSummerTime->setCheckState(_bSummerTime?Qt::Checked:Qt::Unchecked);
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
	set.setValue("TimeOffset",	_nTimeOffset	);
	set.setValue("SummerTime",	_bSummerTime	);
}

void TcSettings::OnEditSrcLog(QString sSrcLog)
{
	_sSrcLog =sSrcLog;
}

void TcSettings::OnClickSrcLog()
{
	_sSrcLog = QFileDialog::getExistingDirectory(_pMainWindow, tr("LOG Input Dir"), _sSrcLog,
												 QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
	ui->editLOG->setText(_sSrcLog);
}

void TcSettings::OnEditSrcJpg(QString sSrcJpg)
{
	_sSrcJpg =sSrcJpg;
}

void TcSettings::OnClickSrcJpg()
{
	_sSrcJpg = QFileDialog::getExistingDirectory(_pMainWindow, tr("JPG Input Dir"), _sSrcJpg,
												 QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
	ui->editJPG->setText(_sSrcJpg);
}

void TcSettings::OnEditDstGpx(QString sDstGpx)
{
	_sDstGpx =sDstGpx;
}

void TcSettings::OnClickDstGpx()
{
	_sDstGpx = QFileDialog::getExistingDirectory(_pMainWindow, tr("GPX Out Dir"), _sDstGpx,
												 QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
	ui->editGPX->setText(_sDstGpx);
}

void TcSettings::OnEditDstKml(QString sDstKml)
{
	_sDstKml =sDstKml;
}

void TcSettings::OnClickDstKml()
{
	_sDstKml = QFileDialog::getExistingDirectory(_pMainWindow, tr("KML Out Dir"), _sDstKml,
												 QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
	ui->editKML->setText(_sDstKml);
}

void TcSettings::OnEditDstJpg(QString sDstJpg)
{
	_sDstJpg =sDstJpg;
}

void TcSettings::OnClickDstJpg()
{
	_sDstJpg = QFileDialog::getExistingDirectory(_pMainWindow, tr("JPG Out Dir"), _sDstJpg,
												 QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
	ui->editJPG2->setText(_sDstJpg);
}

void TcSettings::OnEditCompression(QString sCompression)
{
	_nCompression =sCompression.toInt();
}

void TcSettings::OnEditMegaPixels(QString sMegaPixels)
{
	_fDstMegaPix  =sMegaPixels.toFloat();
}

void TcSettings::OnTimeOffsetChanged(int nIndex)
{
	_nTimeOffset = ui->comboTimeOffset->itemData(nIndex).toInt();
}

void TcSettings::OnSummerTimeChanged(int nState)
{
	_bSummerTime =(nState==Qt::Checked);
}
