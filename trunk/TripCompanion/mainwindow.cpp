#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

#include "TcSettings.h"
#include "gpslog.h"

GpsLog* pGpslog;

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, _bScanDone(false)

{
    ui->setupUi(this);
	ui->btnExec->setText("Scan for log");
	ui->progressBar->setValue(0);

	_pSettings =new TcSettings(this);

#ifndef WIN32
	setWindowState(Qt::WindowMaximized);
#endif

	connect(ui->btnExec,	SIGNAL(clicked()), this, SLOT(OnButtonExec())	);
	connect(ui->tabMain,	SIGNAL(currentChanged(int)), this, SLOT(OnTabChanged(int))	);

	_nTabCurrent =ui->tabMain->currentIndex();

	connect(&_LogLib, SIGNAL(sigProgress(int)),			this, SLOT(OnProgressUpdate(int))	);
	connect(&_LogLib, SIGNAL(sigPushText(QString)),		this, SLOT(OnPrint(QString))		);
	connect(&_LogLib, SIGNAL(sigSetFile(QString)),		this, SLOT(OnSetFile(QString))		);
	connect(&_LogLib, SIGNAL(sigSetStatus(QString)),	this, SLOT(OnSetStatus(QString))	);
	connect(&_LogLib, SIGNAL(sigNewImage(QPixmap)),		this, SLOT(OnNewImage(QPixmap))		);

	_LogLib.SetSettings(_pSettings);

//	ui->labelPreview->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//	ui->labelPreview->setScaledContents(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnButtonExec(void)
{
	if (_bScanDone==false)
	{
		ui->btnExec->setEnabled(false);

		_LogLib.Scan();
		OnPrint(QString("Found %1 Logs").arg(_LogLib._lGpsLogs.size()));
		OnPrint(QString("Found %1 Photo").arg(_LogLib._lPhotos.size()));

		_bScanDone =true;
		ui->btnExec->setText("Generate");
		ui->btnExec->setEnabled(true);
	}
	else
	{
		_LogLib.Load();
	}
}

void MainWindow::OnTabChanged(int nIndex)
{
	if (_nTabCurrent==1 && nIndex !=1)	// leaving config
	{
		_pSettings->Save();
	}

	_nTabCurrent =nIndex;
}

void MainWindow::OnProgressUpdate(int nProgress)
{
	ui->progressBar->setValue(nProgress);
}

void MainWindow::OnSetFile(QString sFile)
{
	ui->labelFile->setText(sFile);
}

void MainWindow::OnPrint(QString sText)
{
	ui->listWidget->addItem(sText);
	ui->listWidget->scrollToBottom();
}

void MainWindow::OnSetStatus(QString sStatus)
{
	ui->labelStatus->setText(sStatus);
}

void MainWindow::OnNewImage(QPixmap pixmap)
{
	ui->labelPreview->setPixmap(pixmap);
}

