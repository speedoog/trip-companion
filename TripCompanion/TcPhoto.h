#ifndef TCPHOTO_H
#define TCPHOTO_H

#include <QString>

class TcLib;

class TcPhoto
{
public:
            TcPhoto(TcLib* pLib, const QString& sPath, const QString& sFilename);
			~TcPhoto();

	void	LoadExif();
	void	Batch();

protected:
    TcLib*      _pLib;
	QString		_sPath, _sFilename;
};

#endif // TCPHOTO_H
