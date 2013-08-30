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
	void		OnClickSrcLog();

	void		OnEditSrcJpg(QString sSrcJpg);
	void		OnClickSrcJpg();

	void		OnEditDstGpx(QString sDstGpx);
	void		OnClickDstGpx();

	void		OnEditDstKml(QString sDstKml);
	void		OnClickDstKml();

	void		OnEditDstJpg(QString sDstJpg);
	void		OnClickDstJpg();

	void		OnTimeOffsetChanged(int nIndex);
	void		OnSummerTimeChanged(int nState);

private:
	MainWindow*		_pMainWindow;
	Ui::MainWindow*	ui;

public:
	QString			_sSrcJpg, _sSrcLog;
	QString			_sDstJpg, _sDstGpx, _sDstKml;
	float			_fDstMegaPix;
	int				_nCompression;
	int				_nTimeOffset;
	bool			_bSummerTime;
};

#endif // Settings_H
