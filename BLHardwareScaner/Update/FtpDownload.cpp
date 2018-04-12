
#include "FtpDownload.h"

#include <QtCore/QUrl>

#include <QtCore/QFileInfo>

LFtpDownload::LFtpDownload(IN const QString& url)
{
    m_downloadUrl = url;

    m_filePath = ".\\";

    m_downloadState.IsDone = true;
    m_downloadState.DoneSize = 0;
    m_downloadState.TotalSize = 1;
    m_downloadState.IsError = false;
    m_downloadState.ErrorMsg = "";
}

LFtpDownload::~LFtpDownload()
{

}

void LFtpDownload::SetLogin(IN const QString& user, IN const QString& pwd)
{
    m_loginUser = user;
    m_loginPwd = pwd;
}

void LFtpDownload::SetFilePath(IN const QString& path)
{
    m_filePath = path;
}

void LFtpDownload::StartDownloadAsync()
{
    m_downloadState.IsDone = false;
    m_downloadState.DoneSize = 0;
    m_downloadState.TotalSize = 1;
    m_downloadState.IsError = false;
    m_downloadState.ErrorMsg = "";

    QUrl url(m_downloadUrl);
    url.setUserName(m_loginUser);
    url.setPassword(m_loginPwd);

    QString fileName = QFileInfo(url.path()).fileName();
    m_downloadFile.setFileName(m_filePath + fileName);
    m_downloadFile.open(QIODevice::WriteOnly);

    m_pDownloadReply = m_ftp.get(QNetworkRequest(url));

    connect(
        m_pDownloadReply, SIGNAL(downloadProgress(qint64, qint64)), 
        this, SLOT(DownloadProgress(qint64, qint64)));
    connect(
        m_pDownloadReply, SIGNAL(finished()), 
        this, SLOT(DownloadDone()));
    connect(
        m_pDownloadReply, SIGNAL(error(QNetworkReply::NetworkError)), 
        this, SLOT(DownloadError(QNetworkReply::NetworkError)));
}

const DownloadState& LFtpDownload::GetDownloadState()
{
    return m_downloadState;
}

void LFtpDownload::DownloadProgress(IN qint64 done, IN qint64 total)
{
    m_downloadState.TotalSize = total;
    m_downloadState.DoneSize = done;
    m_downloadFile.write(m_pDownloadReply->readAll());
}

void LFtpDownload::DownloadDone()
{
    m_downloadFile.flush();
    m_downloadFile.close();

    m_downloadState.IsDone = true;
}

void LFtpDownload::DownloadError(QNetworkReply::NetworkError error)
{
    m_downloadFile.flush();
    m_downloadFile.close();
    m_downloadState.IsDone = true;
    m_downloadState.IsError = true;
}



