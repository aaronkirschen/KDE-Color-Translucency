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
#include "ColorTranslucencyConfig.h"

#include <QtDBus/QDBusConnection>
#include <QDBusError>
#include <KX11Extras>

#if QT_VERSION_MAJOR >= 6
    #include <opengl/glutils.h>
    #include <effect/effecthandler.h>
    #include <core/output.h>
    #include <core/renderviewport.h>
#else
    #include <kwinglutils.h>
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
    qInfo() << "ColorTranslucencyEffect::ColorTranslucencyEffect: Constructor called";

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

    if(m_shaderManager.IsValid()) {
        for (const auto& win: KWin::effects->stackingOrder())
            windowAdded(win);
        connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &ColorTranslucencyEffect::windowAdded);
        connect(KWin::effects, &KWin::EffectsHandler::windowDeleted, this, &ColorTranslucencyEffect::windowRemoved);
#if QT_VERSION_MAJOR < 6
        connect(KWin::effects, &KWin::EffectsHandler::windowFrameGeometryChanged, this, &ColorTranslucencyEffect::windowResized);
#endif
    } else {
        qInfo() << "shader manager is not valid!!";
    }
}

ColorTranslucencyEffect::~ColorTranslucencyEffect() = default;

void ColorTranslucencyEffect::windowAdded(KWin::EffectWindow *w)
{
    qInfo() << "ColorTranslucencyEffect::windowAdded: windowRole: " << w->windowRole() << "windowType: " << w->windowType() << "windowClass: " << w->windowClass();
    const QSet<QString> hardExceptions { "plasmashell", "kscreenlocker_greet", "ksmserver", "krunner" };
    const auto name = w->windowClass().split(QChar::Space).first();
    if (hardExceptions.contains(name))
        return;
    auto r = m_managed.insert(w);

    if (r.second)
    {
        qInfo() << "ColorTranslucencyEffect::windowAdded: setShader called on window:" << w;

        redirect(w);
        setShader(w, m_shaderManager.GetShader().get());
    } else {
        qInfo() << "ColorTranslucencyEffect::windowAdded: not applying shader to window:" << w;

    }
}

void ColorTranslucencyEffect::windowRemoved(KWin::EffectWindow *w)
{
    qInfo() << "ColorTranslucencyEffect::windowRemoved: " << w->windowClass();
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

    qInfo() << "activeTargetColors:" << colors;
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
    qInfo() << "ColorTranslucencyEffect::getActiveColors:" << m_activeColors;

    return m_activeColors;
}

QVector<int> ColorTranslucencyEffect::getActiveAlphas()
{
    qInfo() << "ColorTranslucencyEffect::getActiveAlphas:" << m_activeAlphas;

    return m_activeAlphas;
}

void ColorTranslucencyEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    ColorTranslucencyConfig::self()->read();

    m_activeColors = activeTargetColors();
    m_activeAlphas = activeTargetAlphas();
    qInfo() << "ColorTranslucencyEffect::reconfigure: config reloaded,";
    qInfo() << "ColorTranslucencyEffect::reconfigure: m_activeColors: " << m_activeColors;
    qInfo() << "ColorTranslucencyEffect::reconfigure: m_activeAlphas: " << m_activeAlphas;
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
    qInfo() << "ColorTranslucencyEffect::prePaintWindow";

    if (!hasEffect(w))
    {
        Effect::prePaintWindow(w, data, time);
        return;
    }

#if QT_VERSION_MAJOR >= 6
    const auto geo = w->frameGeometry() * w->screen()->scale();
    data.setTranslucent();
#else
    const auto geo = w->frameGeometry() * KWin::effects->renderTargetScale();
    data.setTranslucent();
#endif


    QRegion reg{};
    reg -= toRect(geo);
    reg += QRect(geo.x(), geo.y(), 0, 0);
    reg += QRect(geo.x() + geo.width(), geo.y(), 0, 0);
    reg += QRect(geo.x(), geo.y() + geo.height(), 0, 0);
    reg += QRect(geo.x() + geo.width(), geo.y() + geo.height(), 0, 0);
    data.opaque -= reg;
    data.paint += reg;

    OffscreenEffect::prePaintWindow(w, data, time);

    qInfo() << "ColorTranslucencyEffect::prePaintWindow: geo:" << geo;
    qInfo() << "ColorTranslucencyEffect::prePaintWindow: reg:" << reg;

    
}

bool ColorTranslucencyEffect::supported()
{
    bool isSupported = KWin::effects->isOpenGLCompositing();
    qInfo() << "ColorTranslucencyEffect::supported: OpenGL compositing is" << (isSupported ? "enabled" : "disabled");
    return isSupported;
}

#if QT_VERSION_MAJOR >= 6
void ColorTranslucencyEffect::drawWindow(const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport,
                                    KWin::EffectWindow *w, int mask, const QRegion &region,
                                    KWin::WindowPaintData &data) {
#else
void ColorTranslucencyEffect::drawWindow(KWin::EffectWindow *w, int mask, const QRegion &region,
                                    KWin::WindowPaintData &data) {
#endif

    // qInfo() << "ColorTranslucencyEffect::drawWindow: window:" << w; 
    // qInfo() << "ColorTranslucencyEffect::drawWindow: mask:" << mask; 
    // qInfo() << "ColorTranslucencyEffect::drawWindow: region:" << region;

    if (!hasEffect(w))
    {
        unredirect(w);
#if QT_VERSION_MAJOR >= 6
        OffscreenEffect::drawWindow(renderTarget, viewport, w, mask, region, data);
#else
        OffscreenEffect::drawWindow(w, mask, region, data);
#endif


        return;
    }

    redirect(w);
    setShader(w, m_shaderManager.GetShader().get());
    m_shaderManager.Bind(w);
    glActiveTexture(GL_TEXTURE0);

#if QT_VERSION_MAJOR >= 6
    OffscreenEffect::drawWindow(renderTarget, viewport, w, mask, region, data);
#else
    OffscreenEffect::drawWindow(w, mask, region, data);
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
    qInfo() << "\n";

    if (!m_shaderManager.IsValid())
    {
        return false;
    }

    QStringList inclusions = ColorTranslucencyConfig::inclusionList();
    QStringList exclusions = ColorTranslucencyConfig::exclusionList();

    qInfo() << "ColorTranslucencyEffect::hasEffect: inclusions:" << inclusions;
    qInfo() << "ColorTranslucencyEffect::hasEffect: exclusions:" << exclusions;

    QString windowTitle = get_window_title(w);

    qInfo() << "ColorTranslucencyEffect::hasEffect called on window: " << w << "windowTitle:" << windowTitle;


    if (!m_managed.contains(w))
    {
        qInfo() << "ColorTranslucencyEffect::hasEffect: FALSE, m_managed.contains(w)";
        return false;
    }

    if (inclusions.contains(windowTitle, Qt::CaseInsensitive))
    {
        qInfo() << "ColorTranslucencyEffect::hasEffect: TRUE, inclusions.contains(windowTitle, Qt::CaseInsensitive)";
        return true;
    }

    if (exclusions.contains(windowTitle, Qt::CaseInsensitive))
    {
        qInfo() << "ColorTranslucencyEffect::hasEffect: FALSE, exclusions.contains(windowTitle, Qt::CaseInsensitive)";

        return false;
    }
    qInfo() << "ColorTranslucencyEffect::hasEffect: FALSE returning false";
    return false;
}

QString ColorTranslucencyEffect::get_window_titles() const {
    QStringList response;
    for (const auto& win: m_managed) {
        const auto name = win->windowClass();
        if (!response.contains(name))
            response.push_back(name);
    }
    qInfo() << "ColorTranslucencyEffect::get_window_titles: found" << response.size() << " window titles,";
    qInfo() << "ColorTranslucencyEffect::get_window_titles: window titles:" << response.join("\n");
    return response.join("\n");
}
