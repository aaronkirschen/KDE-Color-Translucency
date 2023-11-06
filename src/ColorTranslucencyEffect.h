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

#include <kwineffects.h>
#include <set>
#include "ColorTranslucencyShader.h"

#if KWIN_EFFECT_API_VERSION >= 236
#include <kwinoffscreeneffect.h>
class ColorTranslucencyEffect : public KWin::OffscreenEffect
#else
#include <kwindeformeffect.h>
class ColorTranslucencyEffect : public KWin::DeformEffect
#endif
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
    void drawWindow(KWin::EffectWindow *window, int mask, const QRegion &region, KWin::WindowPaintData &data) override;

    int requestedEffectChainPosition() const override { return 99; }

public Q_SLOTS:
    QString get_window_titles();
    

protected Q_SLOTS:
    void windowAdded(KWin::EffectWindow *window);
    void windowRemoved(KWin::EffectWindow *window);

public:
    QString get_window_title(const KWin::EffectWindow* w) const;

private:
    std::set<const KWin::EffectWindow*> m_managed;
    ColorTranslucencyShader m_shaderManager;

    bool hasEffect(const KWin::EffectWindow *w) const;
};

