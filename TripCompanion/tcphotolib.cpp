#include "tcphotolib.h"
#include "tcphoto.h"

#include <QDir>

TcPhotoLib::TcPhotoLib()
: _thread(this)
{
}

TcPhotoLib::~TcPhotoLib()
{
}

void TcPhotoLib::Scan(const QString& sRootDirPhoto)
{
}

void TcPhotoLib::Load()
{
}

void TcPhotoLib::OnLoadStart()
{

}

void TcPhotoLib::OnLoadEnd()
{
}

void TcPhotoLib::Print(const QString& sText)
{
	emit sigPushText(sText);
}

void TcPhotoLib::SetProgress(int nProgress)
{
	emit sigProgress(nProgress);
}
