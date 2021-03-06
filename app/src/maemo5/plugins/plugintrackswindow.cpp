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

#include "plugintrackswindow.h"
#include "audioplayer.h"
#include "clipboard.h"
#include "imagecache.h"
#include "listview.h"
#include "nowplayingaction.h"
#include "nowplayingwindow.h"
#include "plugindownloaddialog.h"
#include "plugintrackwindow.h"
#include "settings.h"
#include "trackdelegate.h"
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QMaemo5InformationBox>

PluginTracksWindow::PluginTracksWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new PluginTrackModel(this)),
    m_cache(new ImageCache),
    m_nowPlayingAction(new NowPlayingAction(this)),
    m_view(new ListView(this)),
    m_delegate(new TrackDelegate(m_cache, PluginTrackModel::ArtistRole, PluginTrackModel::DateRole,
                                 PluginTrackModel::DurationStringRole, PluginTrackModel::ThumbnailUrlRole,
                                 PluginTrackModel::TitleRole, this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_contextMenu(new QMenu(this)),
    m_queueAction(new QAction(tr("Queue"), this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No tracks found")), this))
{
    setWindowTitle(tr("Tracks"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    m_reloadAction->setEnabled(false);
    
    m_contextMenu->addAction(m_queueAction);
    m_contextMenu->addAction(m_downloadAction);
    m_contextMenu->addAction(m_shareAction);
    
    m_label->hide();
    
    m_layout = new QVBoxLayout(centralWidget());
    m_layout->addWidget(m_view);
    m_layout->addWidget(m_label);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    menuBar()->addAction(m_reloadAction);
    menuBar()->addAction(m_nowPlayingAction);
    
    connect(m_model, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showTrack(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playTrack(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
    connect(m_queueAction, SIGNAL(triggered()), this, SLOT(queueTrack()));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadTrack()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareTrack()));
}

PluginTracksWindow::~PluginTracksWindow() {
    delete m_cache;
    m_cache = 0;
}

void PluginTracksWindow::list(const QString &service, const QString &id) {
    m_model->setService(service);
    m_model->list(id);
}

void PluginTracksWindow::search(const QString &service, const QString &query, const QString &order) {
    m_model->setService(service);
    m_model->search(query, order);
}

void PluginTracksWindow::downloadTrack() {
    if (isBusy()) {
        return;
    }
    
    if (m_view->currentIndex().isValid()) {
        QString id = m_view->currentIndex().data(PluginTrackModel::IdRole).toString();
        QString title = m_view->currentIndex().data(PluginTrackModel::TitleRole).toString();
        QUrl streamUrl = m_view->currentIndex().data(PluginTrackModel::StreamUrlRole).toString();
        
        PluginDownloadDialog *dialog = new PluginDownloadDialog(m_model->service(), id, streamUrl, title, this);
        dialog->open();
    }
}

void PluginTracksWindow::playTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (PluginTrack *track = m_model->get(index.row())) {
        AudioPlayer::instance()->playTrack(track);
        NowPlayingWindow *window = new NowPlayingWindow(this);
        window->show();
    }
}

void PluginTracksWindow::queueTrack() {
    if (isBusy()) {
        return;
    }
    
    if (PluginTrack *track = m_model->get(m_view->currentIndex().row())) {
        AudioPlayer::instance()->addTrack(track);
        QMaemo5InformationBox::information(this, tr("'%1' added to playback queue").arg(track->title()));
    }
}

void PluginTracksWindow::shareTrack() {
    if (const PluginTrack *track = m_model->get(m_view->currentIndex().row())) {
        Clipboard::instance()->setText(track->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void PluginTracksWindow::showTrack(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (PluginTrack *track = m_model->get(index.row())) {
        PluginTrackWindow *window = new PluginTrackWindow(track, this);
        window->show();
    }
}

void PluginTracksWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        m_downloadAction->setEnabled(m_model->data(m_view->currentIndex(), PluginTrackModel::DownloadableRole).toBool());
        m_contextMenu->popup(pos, m_queueAction);
    }
}

void PluginTracksWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void PluginTracksWindow::onModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        m_label->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_model->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    m_reloadAction->setEnabled(true);
    
    if (m_model->rowCount() == 0) {
        m_view->hide();
        m_label->show();
    }
}
