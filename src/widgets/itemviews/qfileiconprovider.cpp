/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qfileiconprovider.h"
#include "qfileiconprovider_p.h"

#include <qapplication.h>
#include <qdir.h>
#include <qpixmapcache.h>
#include <private/qfunctions_p.h>
#include <private/qguiapplication_p.h>
#include <private/qicon_p.h>
#include <private/qfilesystementry_p.h>
#include <qpa/qplatformintegration.h>
#include <qpa/qplatformservices.h>
#include <qpa/qplatformtheme.h>

#if defined(Q_OS_WIN)
#  include <qt_windows.h>
#  include <commctrl.h>
#  include <objbase.h>
#endif

QT_BEGIN_NAMESPACE

/*!
  \class QFileIconProvider

  \inmodule QtWidgets

  \brief The QFileIconProvider class provides file icons for the QFileSystemModel class.
*/

QFileIconProviderPrivate::QFileIconProviderPrivate(QFileIconProvider *q)
    : QAbstractFileIconProviderPrivate(q), homePath(QDir::home().absolutePath())
{
}

QIcon QFileIconProviderPrivate::getIcon(QStyle::StandardPixmap name) const
{
    switch (name) {
    case QStyle::SP_FileIcon:
        if (file.isNull())
            file = QApplication::style()->standardIcon(name);
        return file;
    case QStyle::SP_FileLinkIcon:
        if (fileLink.isNull())
            fileLink = QApplication::style()->standardIcon(name);
        return fileLink;
    case QStyle::SP_DirIcon:
        if (directory.isNull())
            directory = QApplication::style()->standardIcon(name);
        return directory;
    case QStyle::SP_DirLinkIcon:
        if (directoryLink.isNull())
            directoryLink = QApplication::style()->standardIcon(name);
        return directoryLink;
    case QStyle::SP_DriveHDIcon:
        if (harddisk.isNull())
            harddisk = QApplication::style()->standardIcon(name);
        return harddisk;
    case QStyle::SP_DriveFDIcon:
        if (floppy.isNull())
            floppy = QApplication::style()->standardIcon(name);
        return floppy;
    case QStyle::SP_DriveCDIcon:
        if (cdrom.isNull())
            cdrom = QApplication::style()->standardIcon(name);
        return cdrom;
    case QStyle::SP_DriveNetIcon:
        if (network.isNull())
            network = QApplication::style()->standardIcon(name);
        return network;
    case QStyle::SP_ComputerIcon:
        if (computer.isNull())
            computer = QApplication::style()->standardIcon(name);
        return computer;
    case QStyle::SP_DesktopIcon:
        if (desktop.isNull())
            desktop = QApplication::style()->standardIcon(name);
        return desktop;
    case QStyle::SP_TrashIcon:
        if (trashcan.isNull())
            trashcan = QApplication::style()->standardIcon(name);
        return trashcan;
    case QStyle::SP_DirHomeIcon:
        if (home.isNull())
            home = QApplication::style()->standardIcon(name);
        return home;
    default:
        return QIcon();
    }
    return QIcon();
}

/*!
  Constructs a file icon provider.
*/

QFileIconProvider::QFileIconProvider()
    : QAbstractFileIconProvider(*new QFileIconProviderPrivate(this))
{
}

/*!
  Destroys the file icon provider.
*/

QFileIconProvider::~QFileIconProvider() = default;

/*!
  \reimp
*/

QIcon QFileIconProvider::icon(IconType type) const
{
    Q_D(const QFileIconProvider);
    switch (type) {
    case Computer:
        return d->getIcon(QStyle::SP_ComputerIcon);
    case Desktop:
        return d->getIcon(QStyle::SP_DesktopIcon);
    case Trashcan:
        return d->getIcon(QStyle::SP_TrashIcon);
    case Network:
        return d->getIcon(QStyle::SP_DriveNetIcon);
    case Drive:
        return d->getIcon(QStyle::SP_DriveHDIcon);
    case Folder:
        return d->getIcon(QStyle::SP_DirIcon);
    case File:
        return d->getIcon(QStyle::SP_FileIcon);
    default:
        break;
    };
    return QIcon();
}

static inline QPlatformTheme::IconOptions toThemeIconOptions(QFileIconProvider::Options options)
{
    QPlatformTheme::IconOptions result;
    if (options & QFileIconProvider::DontUseCustomDirectoryIcons)
        result |= QPlatformTheme::DontUseCustomDirectoryIcons;
    return result;
}

QIcon QFileIconProviderPrivate::getIcon(const QFileInfo &fi) const
{
    return QGuiApplicationPrivate::platformTheme()->fileIcon(fi, toThemeIconOptions(options));
}

/*!
  \reimp
*/

QIcon QFileIconProvider::icon(const QFileInfo &info) const
{
    Q_D(const QFileIconProvider);

    QIcon retIcon = d->getIcon(info);
    if (!retIcon.isNull())
        return retIcon;

    const QString &path = info.absoluteFilePath();
    if (path.isEmpty() || QFileSystemEntry::isRootPath(path))
#if defined (Q_OS_WIN)
    {
        UINT type = GetDriveType(reinterpret_cast<const wchar_t *>(path.utf16()));

        switch (type) {
        case DRIVE_REMOVABLE:
            return d->getIcon(QStyle::SP_DriveFDIcon);
        case DRIVE_FIXED:
            return d->getIcon(QStyle::SP_DriveHDIcon);
        case DRIVE_REMOTE:
            return d->getIcon(QStyle::SP_DriveNetIcon);
        case DRIVE_CDROM:
            return d->getIcon(QStyle::SP_DriveCDIcon);
        case DRIVE_RAMDISK:
        case DRIVE_UNKNOWN:
        case DRIVE_NO_ROOT_DIR:
        default:
            return d->getIcon(QStyle::SP_DriveHDIcon);
        }
    }
#else
    return d->getIcon(QStyle::SP_DriveHDIcon);
#endif

    if (info.isFile()) {
        if (info.isSymLink())
            return d->getIcon(QStyle::SP_FileLinkIcon);
        else
            return d->getIcon(QStyle::SP_FileIcon);
    }
  if (info.isDir()) {
    if (info.isSymLink()) {
      return d->getIcon(QStyle::SP_DirLinkIcon);
    } else {
      if (info.absoluteFilePath() == d->homePath) {
        return d->getIcon(QStyle::SP_DirHomeIcon);
      } else {
        return d->getIcon(QStyle::SP_DirIcon);
      }
    }
  }
  return QIcon();
}

QT_END_NAMESPACE
