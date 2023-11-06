/*
 * Copyright © 2015 Robert Metsäranta <therealestrob@gmail.com>
 *
 * Modifications to support color translucency effect.
 * Copyright (c) 2023 Aaron Kirschen
 * 
 * This file is part of Color Translucency Effect.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "ColorTranslucencyEffect.h"
#include <kwingltexture.h>
#include <QtDBus/QDBusConnection>
#include <QDBusError>


#if KWIN_EFFECT_API_VERSION >= 235
#include <KX11Extras>
#else
#include <kwindowsystem.h>
#endif

ColorTranslucencyEffect::ColorTranslucencyEffect()
#if KWIN_EFFECT_API_VERSION >= 236
    : KWin::OffscreenEffect()
#else
    : KWin::DeformEffect()
#endif
{
    reconfigure(ReconfigureAll);

    auto connection = QDBusConnection::sessionBus();
    if (!connection.isConnected())
    {
        qWarning("ColorTranslucency: Cannot connect to the D-Bus session bus.\n");
    }
    else
    {
        if (!connection.registerService("org.kde.ColorTranslucency"))
        {
            qWarning("%s\n", qPrintable(connection.lastError().message()));
        }
        else
        {
            if (!connection.registerObject("/ColorTranslucencyEffect", this, QDBusConnection::ExportAllSlots))
            {
                qWarning("%s\n", qPrintable(connection.lastError().message()));
            }
        }
    }

    if (m_shaderManager.IsValid())
    {
#if KWIN_EFFECT_API_VERSION >= 235
        const auto &windowList = KX11Extras::windows();
#else
        const auto &windowList = KWindowSystem::windows();
#endif
        for (const auto &id : windowList)
            if (auto win = KWin::effects->findWindow(id))
                windowAdded(win);
        connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &ColorTranslucencyEffect::windowAdded);
        connect(KWin::effects, &KWin::EffectsHandler::windowDeleted, this, &ColorTranslucencyEffect::windowRemoved);
    }
}

ColorTranslucencyEffect::~ColorTranslucencyEffect() = default;

void ColorTranslucencyEffect::windowAdded(KWin::EffectWindow *w)
{
    qDebug() << w->windowRole() << w->windowType() << w->windowClass();
    auto name = w->windowClass();
    qDebug() << "Window Added: " << name;
    auto r = m_managed.insert(w);
    if (r.second)
    {
        redirect(w);
        setShader(w, m_shaderManager.GetShader().get());
    }
}

void ColorTranslucencyEffect::windowRemoved(KWin::EffectWindow *w)
{
    m_managed.erase(w);
    unredirect(w);
}

void ColorTranslucencyEffect::reconfigure(ReconfigureFlags flags)
{
    qDebug() << "ColorTranslucencyEffect::reconfigure - Reconfiguring effect";
    QColor targetColor = ColorTranslucencyConfig::targetColor();
    qDebug() << "ColorTranslucencyEffect::reconfigure - TargetColor:" << targetColor;
            
    Q_UNUSED(flags)
    ColorTranslucencyConfig::self()->read();
}

bool ColorTranslucencyEffect::isMaximized(const KWin::EffectWindow *w)
{
    auto screenGeometry = KWin::effects->findScreen(w->screen()->name())->geometry();
    return (w->x() == screenGeometry.x() && w->width() == screenGeometry.width()) ||
           (w->y() == screenGeometry.y() && w->height() == screenGeometry.height());
}

QRectF operator*(QRect r, qreal scale) { return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale}; }
QRectF operator*(QRectF r, qreal scale) { return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale}; }
QRect toRect(const QRectF &r) { return {(int)r.x(), (int)r.y(), (int)r.width(), (int)r.height()}; }
const QRect &toRect(const QRect &r) { return r; }

void ColorTranslucencyEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
{
    if (!hasEffect(w))
    {
        Effect::prePaintWindow(w, data, time);
        return;
    }

#if KWIN_EFFECT_API_VERSION >= 234
    const auto geo_ex = w->expandedGeometry() * KWin::effects->renderTargetScale();
    const auto geo = w->frameGeometry() * KWin::effects->renderTargetScale();
    data.setTranslucent();
#else
    const auto &geo_ex = w->expandedGeometry();
    const auto &geo = w->expandedGeometry();
#endif
    QRegion reg{};
    reg += toRect(geo_ex);
    reg -= toRect(geo);
    reg += QRect(geo.x(), geo.y(), 0, 0);
    reg += QRect(geo.x() + geo.width(), geo.y(), 0, 0);
    reg += QRect(geo.x(), geo.y() + geo.height(), 0, 0);
    reg += QRect(geo.x() + geo.width(), geo.y() + geo.height(), 0, 0);
#if KWIN_EFFECT_API_VERSION >= 234
    data.opaque -= reg;
#endif
    data.paint += reg;

#if KWIN_EFFECT_API_VERSION >= 236
    OffscreenEffect::prePaintWindow(w, data, time);
#else
    DeformEffect::prePaintWindow(w, data, time);
#endif
}

bool ColorTranslucencyEffect::supported()
{
    return KWin::effects->isOpenGLCompositing();
}

void ColorTranslucencyEffect::drawWindow(KWin::EffectWindow *w, int mask, const QRegion &region,
                                         KWin::WindowPaintData &data)
{
    if (!hasEffect(w))
    {
        unredirect(w);
#if KWIN_EFFECT_API_VERSION >= 236
        OffscreenEffect::drawWindow(w, mask, region, data);
#else
        DeformEffect::drawWindow(w, mask, region, data);
#endif
        return;
    }
    redirect(w);
    setShader(w, m_shaderManager.GetShader().get());
    m_shaderManager.Bind(w);
    glActiveTexture(GL_TEXTURE0);

#if KWIN_EFFECT_API_VERSION >= 236
    OffscreenEffect::drawWindow(w, mask, region, data);
#else
    DeformEffect::drawWindow(w, mask, region, data);
#endif
    m_shaderManager.Unbind();
}

QString ColorTranslucencyEffect::get_window_title(const KWin::EffectWindow *w) const
{
    auto fullClass = w->windowClass();
    QString windowRole = w->windowRole();

    // qDebug() << "ColorTranslucency::get_window_title: windowRole=" << windowRole;

    // qDebug() << "ColorTranslucency::get_window_title: fullClass=" << fullClass;

    QStringList parts = fullClass.split(' ');
    // qDebug() << "ColorTranslucency::get_window_title: parts=" << parts;

    QString windowTitle;
    if (parts.size() > 1 && parts[0] == parts[1])
    {
        windowTitle = parts[0]; // Use just one part if both are the same
        // qDebug() << "ColorTranslucency::get_window_title: parts the same; windowTitle=" << windowTitle;
    }
    else
    {
        windowTitle = fullClass; // Use the full string otherwise
        // qDebug() << "ColorTranslucency::get_window_title: parts different; windowTitle=" << windowTitle;

    }
    return windowTitle; // Just return first part
}

bool ColorTranslucencyEffect::hasEffect(const KWin::EffectWindow *w) const
{

    if (!m_shaderManager.IsValid()) {
        // qDebug() << "ColorTranslucency::hasEffect: m_shaderManager is not valid, so skipping match.";
        return false;
    }

    QStringList inclusions = ColorTranslucencyConfig::inclusions();
    QStringList exclusions = ColorTranslucencyConfig::exclusions();

    QString windowTitle = get_window_title(w);

    // qDebug() << "ColorTranslucency::hasEffect: Checking window with windowTitle=" << windowTitle;

    if (!m_managed.contains(w)) {
        // qDebug() << "ColorTranslucency::hasEffect: Excluding window (not in m_managed):" << windowTitle;
        return false;
    } 

    if (inclusions.contains(windowTitle, Qt::CaseInsensitive))
    {
        // qDebug() << "ColorTranslucency::hasEffect: Including window (on inclusion list):" << windowTitle;
        return true;
    }

    if (exclusions.contains(windowTitle, Qt::CaseInsensitive))
    {
        // qDebug() << "ColorTranslucency::hasEffect: Excluding window (on exclusion list):" << windowTitle;
        return false;
    }
    // qDebug() << "colorTranslucency::hasEffect: Excluding window (no checks returned true):" << windowTitle;
    return false;
    
}

QString ColorTranslucencyEffect::get_window_titles()
{
    QSet<QString> response;
    for (const auto &win : m_managed)
    {
        QString windowTitle = get_window_title(win);

        if (windowTitle.contains("plasmashell", Qt::CaseInsensitive))
            continue;

        response.insert(windowTitle);
    }
    return response.values().join("\n");
}
