/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRANSFER_H
#define TRANSFER_H

#include <QString>
#include <QUrl>
#include <QFile>
#include <QVariantList>
#include <qplatformdefs.h>

#ifdef MEEGO_EDITION_HARMATTAN
namespace TransferUI {
    class Client;
    class Transfer;
}
#endif

class AudioConverter;
class QNetworkAccessManager;
class QNetworkReply;

class Transfer : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(qint64 bytesTransferred READ bytesTransferred NOTIFY progressChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(QString downloadPath READ downloadPath WRITE setDownloadPath NOTIFY downloadPathChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(Priority priority READ priority WRITE setPriority NOTIFY priorityChanged)
    Q_PROPERTY(QString priorityString READ priorityString NOTIFY priorityChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString resourceId READ resourceId WRITE setResourceId NOTIFY resourceIdChanged)
    Q_PROPERTY(QString service READ service NOTIFY serviceChanged)
    Q_PROPERTY(qint64 size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusString READ statusString NOTIFY statusChanged)
    Q_PROPERTY(QString streamId READ streamId WRITE setStreamId NOTIFY streamIdChanged)
    Q_PROPERTY(QUrl streamUrl READ streamUrl WRITE setStreamUrl NOTIFY streamUrlChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(TransferType transferType READ transferType WRITE setTransferType NOTIFY transferTypeChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    
    Q_ENUMS(Priority Status TransferType)
    
public:
    enum Priority {
        HighPriority,
        NormalPriority,
        LowPriority
    };
    
    enum Status {
        Paused = 0,
        Canceled,
        Failed,
        Completed,
        Queued,
        Connecting,
        Downloading,
        Uploading,
        Converting,
        Unknown
    };
    
    enum TransferType {
        Download,
        Upload
    };
    
    explicit Transfer(QObject *parent = 0);
    ~Transfer();
    
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    
    qint64 bytesTransferred() const;
    
    QString category() const;
    void setCategory(const QString &c);
    
    QString downloadPath() const;
    void setDownloadPath(const QString &path);
    
    QString errorString() const;
        
    QString fileName() const;
    void setFileName(const QString &fn);
    
    QString id() const;
    void setId(const QString &i);
        
    Priority priority() const;
    void setPriority(Priority p);
    QString priorityString() const;
    
    int progress() const;
    
    QString resourceId() const;
    void setResourceId(const QString &ri);
    
    QString service() const;
    
    qint64 size() const;
    void setSize(qint64 s);
    
    Status status() const;
    QString statusString() const;
    
    QString streamId() const;
    void setStreamId(const QString &si);
    
    QUrl streamUrl() const;
    void setStreamUrl(const QUrl &url);
    
    QString title() const;
    void setTitle(const QString &title);
    
    TransferType transferType() const;
    void setTransferType(TransferType type);
    
    QUrl url() const;
    
public Q_SLOTS:
    void queue();
    void start();
    void pause();
    void cancel();
    
protected:
    virtual void listStreams() = 0;
    
    QString fileExtension() const;
    void setFileExtension(const QString &ext);
    
    void setErrorString(const QString &es);
    
    void setProgress(int p);
    
    void setService(const QString &s);
        
    void setStatus(Status s);
    
    void setUrl(const QUrl &u);
        
    void startDownload(const QUrl &u);
    void followRedirect(const QUrl &u);
            
    void moveDownloadedFiles();    
    
private Q_SLOTS:
    void onReplyMetaDataChanged();
    void onReplyReadyRead();
    void onReplyFinished();
    
Q_SIGNALS:
    void categoryChanged();
    void downloadPathChanged();
    void fileNameChanged();
    void idChanged();
    void priorityChanged();
    void progressChanged();
    void resourceIdChanged();
    void serviceChanged();
    void sizeChanged();
    void statusChanged();
    void streamIdChanged();
    void streamUrlChanged();
    void titleChanged();
    void transferTypeChanged();
    void urlChanged();

private:
#ifdef MEEGO_EDITION_HARMATTAN
    static TransferUI::Client *tuiClient;
    TransferUI::Transfer *m_tuiTransfer;
#endif
        
    QNetworkAccessManager *m_nam;
    QNetworkReply *m_reply;
        
    QFile m_file;
    
    bool m_ownNetworkAccessManager;
    bool m_canceled;
    
    QString m_category;
    
    QString m_downloadPath;
    
    QString m_errorString;
    
    QString m_fileExtension;
    
    QString m_fileName;
    
    QString m_id;
    
    Priority m_priority;
    
    int m_progress;
    
    QString m_resourceId;
    
    QString m_service;
    
    qint64 m_size;
    qint64 m_bytesTransferred;
    
    int m_redirects;
    
    Status m_status;
    
    QString m_streamId;
    
    QUrl m_streamUrl;
    
    QString m_title;
    
    TransferType m_transferType;
    
    QUrl m_url;
#ifdef SYMBIAN_OS
    QByteArray m_buffer;
#endif
};
    
#endif // TRANSFER_H
