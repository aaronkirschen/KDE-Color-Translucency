/*
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

#ifndef KWIN4_COLORTRANSLUCENCY_CONFIG_SHADERMANAGER_H
#define KWIN4_COLORTRANSLUCENCY_CONFIG_SHADERMANAGER_H

#include <kwinglutils.h>
#include <memory>
#include <QPalette>
#include "ColorTranslucencyConfig.h"

namespace KWin {
    class GLShader;
    class ShaderManager;
}

class ColorTranslucencyShader {
public:
    ColorTranslucencyShader();

    bool IsValid() const;
    const std::unique_ptr<KWin::GLShader>& Bind(KWin::EffectWindow *w) const;
    const std::unique_ptr<KWin::GLShader>& Bind(QMatrix4x4 mvp, KWin::EffectWindow *w) const;
    void Unbind() const;
    std::unique_ptr<KWin::GLShader>& GetShader() { return m_shader; }

private:
    std::unique_ptr<KWin::GLShader> m_shader;
    KWin::ShaderManager* m_manager;
    QPalette m_palette;
    int m_shader_targetColor = 0;
    int m_shader_targetAlpha = 0;
    int m_shader_tex = 0;
};


#endif //KWIN4_COLORTRANSLUCENCY_CONFIG_SHADERMANAGER_H