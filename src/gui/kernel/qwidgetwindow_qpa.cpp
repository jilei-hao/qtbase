/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwidgetwindow_qpa_p.h"

#include "private/qwidget_p.h"
#include "private/qapplication_p.h"

QT_BEGIN_NAMESPACE

QWidgetWindow::QWidgetWindow(QWidget *widget)
    : m_widget(widget)
{
}

bool QWidgetWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Close:
        handleCloseEvent(static_cast<QCloseEvent *>(event));
        return true;

    case QEvent::Enter:
    case QEvent::Leave:
        handleEnterLeaveEvent(event);
        return true;

    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        handleKeyEvent(static_cast<QKeyEvent *>(event));
        return true;

    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
        handleMouseEvent(static_cast<QMouseEvent *>(event));
        return true;

    case QEvent::Move:
        handleMoveEvent(static_cast<QMoveEvent *>(event));
        return true;

    case QEvent::Resize:
        handleResizeEvent(static_cast<QResizeEvent *>(event));
        return true;

    case QEvent::Wheel:
        handleWheelEvent(static_cast<QWheelEvent *>(event));
        return true;

    default:
        break;
    }

    return m_widget->event(event) || QWindow::event(event);
}

QPointer<QWidget> qt_last_mouse_receiver = 0;

void QWidgetWindow::handleEnterLeaveEvent(QEvent *event)
{
    if (event->type() == QEvent::Leave) {
        QApplicationPrivate::dispatchEnterLeave(0, m_widget);
        qt_last_mouse_receiver = 0;
    } else {
        QApplicationPrivate::dispatchEnterLeave(m_widget, 0);
        qt_last_mouse_receiver = m_widget;
        printf("Enter event: %p\n", m_widget);
    }
}

void QWidgetWindow::handleMouseEvent(QMouseEvent *event)
{
    // which child should have it?
    QWidget *widget = m_implicit_mouse_grabber ? m_implicit_mouse_grabber.data() : m_widget->childAt(event->pos());

    // TODO: make sure mouse release is delivered to same widget that got the press event

    if (!widget)
        widget = m_widget;

    if (event->type() == QEvent::MouseButtonPress && !m_implicit_mouse_grabber)
        m_implicit_mouse_grabber = widget;

    if (event->buttons() == Qt::NoButton)
        m_implicit_mouse_grabber.clear();

    QPoint mapped = widget->mapFrom(m_widget, event->pos());

    if (widget != qt_last_mouse_receiver) {
        QApplicationPrivate::dispatchEnterLeave(widget, qt_last_mouse_receiver);
        qt_last_mouse_receiver = widget;
    }

    QMouseEvent translated(event->type(), mapped, event->globalPos(), event->button(), event->buttons(), event->modifiers());
    QGuiApplication::sendSpontaneousEvent(widget, &translated);

    if (event->type() == QEvent::MouseButtonPress && event->button() == Qt::RightButton) {
        QContextMenuEvent e(QContextMenuEvent::Mouse, mapped, event->globalPos(), event->modifiers());
        QGuiApplication::sendSpontaneousEvent(widget, &e);
    }
}

void QWidgetWindow::handleKeyEvent(QKeyEvent *event)
{
    QWidget *widget = m_widget->focusWidget();

    if (!widget)
        widget = m_widget;

    QGuiApplication::sendSpontaneousEvent(widget, event);
}

void QWidgetWindow::handleMoveEvent(QMoveEvent *event)
{
    m_widget->data->crect = geometry();
    QGuiApplication::sendSpontaneousEvent(m_widget, event);
}

void QWidgetWindow::handleResizeEvent(QResizeEvent *event)
{
    m_widget->data->crect = geometry();
    QGuiApplication::sendSpontaneousEvent(m_widget, event);
}

void QWidgetWindow::handleCloseEvent(QCloseEvent *)
{
    m_widget->d_func()->close_helper(QWidgetPrivate::CloseWithSpontaneousEvent);
}

void QWidgetWindow::handleWheelEvent(QWheelEvent *event)
{
    // which child should have it?
    QWidget *widget = m_widget->childAt(event->pos());

    if (!widget)
        widget = m_widget;

    QPoint mapped = widget->mapFrom(m_widget, event->pos());

    QWheelEvent translated(mapped, event->globalPos(), event->delta(), event->buttons(), event->modifiers(), event->orientation());
    QGuiApplication::sendSpontaneousEvent(widget, &translated);
}

QT_END_NAMESPACE
