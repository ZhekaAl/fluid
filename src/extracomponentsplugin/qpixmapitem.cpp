/****************************************************************************
 * This file is part of Fluid.
 *
 * Copyright (c) 2012 Pier Luigi Fiorini
 * Copyright (c) 2011 Marco Martin
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *    Marco Martin <mart@kde.org>
 *
 * $BEGIN_LICENSE:LGPL-ONLY$
 *
 * This file may be used under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation and
 * appearing in the file LICENSE.LGPL included in the packaging of
 * this file, either version 2.1 of the License, or (at your option) any
 * later version.  Please review the following information to ensure the
 * GNU Lesser General Public License version 2.1 requirements
 * will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *
 * If you have questions regarding the use of this file, please contact
 * us via http://www.maui-project.org/.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QPainter>

#include "qpixmapitem.h"

QPixmapItem::QPixmapItem(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_smooth(false),
      m_fillMode(QPixmapItem::Stretch)
{
    setFlag(QQuickItem::ItemHasContents);
}

QPixmapItem::~QPixmapItem()
{
}

void QPixmapItem::setPixmap(const QPixmap &pixmap)
{
    bool oldPixmapNull = m_pixmap.isNull();
    m_pixmap = pixmap;
    update();
    emit nativeWidthChanged();
    emit nativeHeightChanged();
    emit pixmapChanged();
    if (oldPixmapNull != m_pixmap.isNull())
        emit nullChanged();
}

QPixmap QPixmapItem::pixmap() const
{
    return m_pixmap;
}

void QPixmapItem::setSmooth(const bool smooth)
{
    if (smooth == m_smooth)
        return;
    m_smooth = smooth;
    update();
}

bool QPixmapItem::smooth() const
{
    return m_smooth;
}

int QPixmapItem::nativeWidth() const
{
    return m_pixmap.size().width();
}

int QPixmapItem::nativeHeight() const
{
    return m_pixmap.size().height();
}

QPixmapItem::FillMode QPixmapItem::fillMode() const
{
    return m_fillMode;
}

void QPixmapItem::setFillMode(QPixmapItem::FillMode mode)
{
    if (mode == m_fillMode)
        return;

    m_fillMode = mode;
    update();
    emit fillModeChanged();
}

void QPixmapItem::paint(QPainter *painter)
{
    if (m_pixmap.isNull())
        return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, m_smooth);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, m_smooth);

    QRect destRect;
    switch (m_fillMode) {
        case PreserveAspectFit: {
            QSize scaled = m_pixmap.size();

            scaled.scale(boundingRect().size().toSize(), Qt::KeepAspectRatio);
            destRect = QRect(QPoint(0, 0), scaled);
            break;
        }
        case PreserveAspectCrop: {
            painter->setClipRect(boundingRect(), Qt::IntersectClip);
            QSize scaled = m_pixmap.size();
            scaled.scale(boundingRect().size().toSize(), Qt::KeepAspectRatioByExpanding);
            destRect = QRect(QPoint(0, 0), scaled);
            break;
        }
        case TileVertically: {
            painter->scale(width() / (qreal)m_pixmap.width(), 1);
            destRect = boundingRect().toRect();
            destRect.setWidth(destRect.width() / (width() / (qreal)m_pixmap.width()));
            break;
        }
        case TileHorizontally: {
            painter->scale(1, height() / (qreal)m_pixmap.height());
            destRect = boundingRect().toRect();
            destRect.setHeight(destRect.height() / (height() / (qreal)m_pixmap.height()));
            break;
        }
        case Stretch:
        case Tile:
        default:
            destRect = boundingRect().toRect();
    }

    if (m_fillMode >= Tile)
        painter->drawTiledPixmap(destRect, m_pixmap);
    else
        painter->drawPixmap(destRect, m_pixmap, m_pixmap.rect());

    painter->restore();
}

bool QPixmapItem::isNull() const
{
    return m_pixmap.isNull();
}

#include "moc_qpixmapitem.cpp"