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

#include <kwinglplatform.h>
#include <QFile>
#include <QStandardPaths>
#include <kwineffects.h>
#include <QWidget>
#include "ColorTranslucencyEffect.h"
#include "ColorTranslucencyShader.h"

#include <QElapsedTimer>

ColorTranslucencyShader::ColorTranslucencyShader() : m_manager(KWin::ShaderManager::instance()),
                                                     m_palette(QWidget().palette())
{
    const QString shadersDir = "kwin/shaders/";
    const QString fragmentshader = QStandardPaths::locate(QStandardPaths::GenericDataLocation, shadersDir + QStringLiteral("colortranslucency.frag"));
    QFile file(fragmentshader);
    if (file.open(QFile::ReadOnly))
    {
        QByteArray frag = file.readAll();
        // auto shader = m_manager->generateCustomShader(KWin::ShaderTrait::MapTexture, QByteArray(), frag);
        auto shader = m_manager->generateShaderFromFile(KWin::ShaderTrait::MapTexture, QString(), fragmentshader);

#if KWIN_EFFECT_API_VERSION >= 235
        m_shader = std::move(shader);
#else
        m_shader.reset(shader);
#endif
        file.close();
        if (m_shader->isValid())
        {
            m_shader_targetColor = m_shader->uniformLocation("targetColor");
            m_shader_targetAlpha = m_shader->uniformLocation("targetAlpha");

            qDebug() << "ColorTranslucency: shaders loaded.";
        }
        else
            qCritical() << "ColorTranslucency: no valid shaders found! ColorTranslucency will not work.";
    }
    else
    {
        qCritical() << "ColorTranslucency: no shaders found! Exiting...";
    }
}

bool ColorTranslucencyShader::IsValid() const
{
    return m_shader && m_shader->isValid();
}

const std::unique_ptr<KWin::GLShader> &
ColorTranslucencyShader::Bind(KWin::EffectWindow *window) const
{
    QColor targetColor = ColorTranslucencyConfig::targetColor();
    float targetAlpha = ColorTranslucencyConfig::targetAlpha();
    m_manager->pushShader(m_shader.get());

    // Set normalized target color
    QVector4D normalizedColor(targetColor.redF(), targetColor.greenF(), targetColor.blueF(), targetColor.alphaF());
    m_shader->setUniform(m_shader_targetColor, normalizedColor);

    // Set normalized target alpha
    float normalizedAlpha = targetAlpha / 255.0f;
    m_shader->setUniform(m_shader_targetAlpha, normalizedAlpha);

    return m_shader;
}


void ColorTranslucencyShader::Unbind() const
{
    m_manager->popShader();
}
