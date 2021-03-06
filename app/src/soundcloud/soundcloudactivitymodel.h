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

#ifndef SOUNDCLOUDACTIVITYMODEL_H
#define SOUNDCLOUDACTIVITYMODEL_H

#include "soundcloudactivity.h"
#include <QAbstractListModel>

class SoundCloudActivityModel : public QAbstractListModel
{
    Q_OBJECT
    
    Q_PROPERTY(bool canFetchMore READ canFetchMore NOTIFY statusChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QSoundCloud::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    
public:
    enum Roles {
        ActivityTypeRole = Qt::UserRole + 1,
        ActivityTypeStringRole,
        ArtistRole,
        ArtistIdRole,
        DateRole,
        DescriptionRole,
        IdRole,
        OriginIdRole,
        ThumbnailUrlRole,
        TitleRole
    };
    
    explicit SoundCloudActivityModel(QObject *parent = 0);
    
    QString errorString() const;
    
    QSoundCloud::ResourcesRequest::Status status() const;
    
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;
    Q_INVOKABLE void fetchMore(const QModelIndex &parent = QModelIndex());
    
    QVariant data(const QModelIndex &index, int role) const;
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    Q_INVOKABLE QVariant data(int row, const QByteArray &role) const;
    Q_INVOKABLE QVariantMap itemData(int row) const;
    
    Q_INVOKABLE SoundCloudActivity* get(int row) const;
    
    Q_INVOKABLE void get(const QString &resourcePath, const QVariantMap &filters = QVariantMap());

public Q_SLOTS:
    void clear();
    void cancel();
    void reload();
    
private:
    void append(SoundCloudActivity *activity);
    void insert(int row, SoundCloudActivity *activity);
    void remove(int row);
    
private Q_SLOTS:
    void onRequestFinished();
    
Q_SIGNALS:
    void countChanged(int c);
    void statusChanged(QSoundCloud::ResourcesRequest::Status s);
    
private:
    QSoundCloud::ResourcesRequest *m_request;
    
    QString m_resourcePath;
    QVariantMap m_filters;
    QString m_nextHref;
        
    QList<SoundCloudActivity*> m_items;
    
    QHash<int, QByteArray> m_roles;
};
    
#endif // SOUNDCLOUDACTIVITYMODEL_H
