#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TcLib.h"
//#include "Settings.h"

namespace Ui {
class MainWindow;
}

class GpsLog;
class TcSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
	void        OnButtonExec();
//	void        OnButtonConfig();
	void		OnProgressUpdate(int nProgress);
	void		OnSetFile(QString sFile);
	void		OnSetStatus(QString sStatus);
	void		OnPrint(QString sText);
	void		OnNewImage(QPixmap pixmap);
	void		OnTabChanged(int);

public:
	Ui::MainWindow*	ui;

private:

	TcLib			_LogLib;
	TcSettings*		_pSettings;

	int				_nTabCurrent;

	bool			_bScanDone;
};

#endif // MAINWINDOW_H
