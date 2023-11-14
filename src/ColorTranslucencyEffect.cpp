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

QVector<QColor> ColorTranslucencyEffect::m_activeColors;
QVector<int> ColorTranslucencyEffect::m_activeAlphas;

ColorTranslucencyEffect::ColorTranslucencyEffect()
#if KWIN_EFFECT_API_VERSION >= 236
    : KWin::OffscreenEffect()
#else
    : KWin::DeformEffect()
#endif
{
    qDebug() << "ColorTranslucencyEffect::ColorTranslucencyEffect: effect created";
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
    auto name = w->windowClass();
    qDebug() << "ColorTranslucencyEffect::windowAdded: " << name;
    auto r = m_managed.insert(w);
    if (r.second)
    {
        redirect(w);
        setShader(w, m_shaderManager.GetShader().get());
    }
}

void ColorTranslucencyEffect::windowRemoved(KWin::EffectWindow *w)
{
    qDebug() << "ColorTranslucencyEffect::windowRemoved: " << w->windowClass();
    m_managed.erase(w);
    unredirect(w);
}

QVector<QColor> activeTargetColors()
{
    QVector<QColor> colors;

    if (ColorTranslucencyConfig::enableColor_1())
    {
        colors.push_back(ColorTranslucencyConfig::targetColor_1());
    }
    if (ColorTranslucencyConfig::enableColor_2())
    {
        colors.push_back(ColorTranslucencyConfig::targetColor_2());
    }
    if (ColorTranslucencyConfig::enableColor_3())
    {
        colors.push_back(ColorTranslucencyConfig::targetColor_3());
    }
    if (ColorTranslucencyConfig::enableColor_4())
    {
        colors.push_back(ColorTranslucencyConfig::targetColor_4());
    }
    if (ColorTranslucencyConfig::enableColor_5())
    {
        colors.push_back(ColorTranslucencyConfig::targetColor_5());
    }
    if (ColorTranslucencyConfig::enableColor_6())
    {
        colors.push_back(ColorTranslucencyConfig::targetColor_6());
    }
    if (ColorTranslucencyConfig::enableColor_7())
    {
        colors.push_back(ColorTranslucencyConfig::targetColor_7());
    }
    if (ColorTranslucencyConfig::enableColor_8())
    {
        colors.push_back(ColorTranslucencyConfig::targetColor_8());
    }
    if (ColorTranslucencyConfig::enableColor_9())
    {
        colors.push_back(ColorTranslucencyConfig::targetColor_9());
    }
    if (ColorTranslucencyConfig::enableColor_10())
    {
        colors.push_back(ColorTranslucencyConfig::targetColor_10());
    }

    return colors;
}

QVector<int> activeTargetAlphas()
{
    QVector<int> alphas;

    if (ColorTranslucencyConfig::enableColor_1())
    {
        alphas.push_back(ColorTranslucencyConfig::targetAlpha_1());
    }
    if (ColorTranslucencyConfig::enableColor_2())
    {
        alphas.push_back(ColorTranslucencyConfig::targetAlpha_2());
    }
    if (ColorTranslucencyConfig::enableColor_3())
    {
        alphas.push_back(ColorTranslucencyConfig::targetAlpha_3());
    }
    if (ColorTranslucencyConfig::enableColor_4())
    {
        alphas.push_back(ColorTranslucencyConfig::targetAlpha_4());
    }
    if (ColorTranslucencyConfig::enableColor_5())
    {
        alphas.push_back(ColorTranslucencyConfig::targetAlpha_5());
    }
    if (ColorTranslucencyConfig::enableColor_6())
    {
        alphas.push_back(ColorTranslucencyConfig::targetAlpha_6());
    }
    if (ColorTranslucencyConfig::enableColor_7())
    {
        alphas.push_back(ColorTranslucencyConfig::targetAlpha_7());
    }
    if (ColorTranslucencyConfig::enableColor_8())
    {
        alphas.push_back(ColorTranslucencyConfig::targetAlpha_8());
    }
    if (ColorTranslucencyConfig::enableColor_9())
    {
        alphas.push_back(ColorTranslucencyConfig::targetAlpha_9());
    }
    if (ColorTranslucencyConfig::enableColor_10())
    {
        alphas.push_back(ColorTranslucencyConfig::targetAlpha_10());
    }

    return alphas;
}

QVector<QColor> ColorTranslucencyEffect::getActiveColors()
{
    return m_activeColors;
}

QVector<int> ColorTranslucencyEffect::getActiveAlphas()
{
    return m_activeAlphas;
}

void ColorTranslucencyEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    ColorTranslucencyConfig::self()->read();

    m_activeColors = activeTargetColors();
    m_activeAlphas = activeTargetAlphas();
    qDebug() << "ColorTranslucencyEffect::reconfigure: config reloaded,";
    qDebug() << "ColorTranslucencyEffect::reconfigure: m_activeColors: " << m_activeColors;
    qDebug() << "ColorTranslucencyEffect::reconfigure: m_activeAlphas: " << m_activeAlphas;
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

    QStringList parts = fullClass.split(' ');

    QString windowTitle;
    if (parts.size() > 1 && parts[0] == parts[1])
    {
        windowTitle = parts[0];
    }
    else
    {
        windowTitle = fullClass;
    }
    return windowTitle;
}

bool ColorTranslucencyEffect::hasEffect(const KWin::EffectWindow *w) const
{

    if (!m_shaderManager.IsValid())
    {
        return false;
    }

    QStringList inclusions = ColorTranslucencyConfig::inclusionList();
    QStringList exclusions = ColorTranslucencyConfig::exclusionList();

    QString windowTitle = get_window_title(w);
    if (!m_managed.contains(w))
    {
        return false;
    }

    if (inclusions.contains(windowTitle, Qt::CaseInsensitive))
    {
        return true;
    }

    if (exclusions.contains(windowTitle, Qt::CaseInsensitive))
    {
        return false;
    }
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
    qDebug() << "ColorTranslucencyEffect::get_window_titles: found" << response.size() << " window titles,";
    qDebug() << "ColorTranslucencyEffect::get_window_titles: window titles:" << response.values().join("\n");

    return response.values().join("\n");
}
