#ifndef Settings_H
#define Settings_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QObject>

class TcSettings : public	QObject
{
	Q_OBJECT
	
public:
	explicit	TcSettings(MainWindow* pMainWindow);
				~TcSettings();

	void		Load();
	void		Save();

public slots:
	void        OnEditCompression(QString sCompression);
	void		OnEditMegaPixels(QString sMegaPixels);
	void		OnEditSrcLog(QString sSrcLog);
	void		OnEditSrcJpg(QString sSrcJpg);
	void		OnEditDstGpx(QString sDstGpx);
	void		OnEditDstKml(QString sDstKml);
	void		OnEditDstJpg(QString sDstJpg);

private:
	MainWindow*		_pMainWindow;
	Ui::MainWindow*	ui;

public:
	QString			_sSrcJpg, _sSrcLog;
	QString			_sDstJpg, _sDstGpx, _sDstKml;
	float			_fDstMegaPix;
	int				_nCompression;

};

#endif // Settings_H
