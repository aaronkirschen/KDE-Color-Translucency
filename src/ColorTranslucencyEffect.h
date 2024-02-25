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

#pragma once

#include "ColorTranslucencyShader.h"
#include <set>

#if QT_VERSION_MAJOR >= 6
    #include <effect/effecthandler.h>
    #include <effect/offscreeneffect.h>
#else
    #include <kwineffects.h>
    #include <kwinoffscreeneffect.h>
#endif

class ColorTranslucencyEffect final : public KWin::OffscreenEffect
{
    Q_OBJECT
public:
    ColorTranslucencyEffect();
    ~ColorTranslucencyEffect() override;

    static bool supported();
    static bool enabledByDefault() { return supported(); }
    static bool isMaximized(const KWin::EffectWindow *w);
    static bool isWindowActive(const KWin::EffectWindow *w) { return KWin::effects->activeWindow() == w; }

    void reconfigure(ReconfigureFlags flags) override;

    void prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time) override;

#if QT_VERSION_MAJOR >= 6
    void drawWindow(const KWin::RenderTarget &RenderTarget, const KWin::RenderViewport& viewport,
                    KWin::EffectWindow *w, int mask, const QRegion &region, KWin::WindowPaintData &data) override;
#else
    void drawWindow(KWin::EffectWindow *w, int mask, const QRegion &region, KWin::WindowPaintData &data) override;
#endif

    [[nodiscard]] int requestedEffectChainPosition() const override { return 99; }
    [[nodiscard]] bool blocksDirectScanout() const override { return false; }

public Q_SLOTS:
    [[nodiscard]] QString get_window_titles() const;

protected Q_SLOTS:
    void windowAdded(KWin::EffectWindow *window);
    void windowRemoved(KWin::EffectWindow *window);

public:
    QString get_window_title(const KWin::EffectWindow *w) const;
    static QVector<QColor> getActiveColors();
    static QVector<int> getActiveAlphas();

private:
    std::set<const KWin::EffectWindow *> m_managed;
    ColorTranslucencyShader m_shaderManager;
    static QVector<QColor> m_activeColors;
    static QVector<int> m_activeAlphas;

    bool hasEffect(const KWin::EffectWindow *w) const;
};
