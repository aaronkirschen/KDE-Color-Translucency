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

#include "ColorTranslucencyEffect.h"
#include "ColorTranslucencyShader.h"

#include <QFile>
#include <QWidget>

#if QT_VERSION_MAJOR >= 6
    #include <opengl/glutils.h>
#else
    #include <kwinglutils.h>
#endif

ColorTranslucencyShader::ColorTranslucencyShader() : m_manager(KWin::ShaderManager::instance()),
                                                     m_widget(new QWidget)

{
    const QString shadersDir = "kwin/shaders/";
    const QString fragmentshader = QStandardPaths::locate(QStandardPaths::GenericDataLocation, shadersDir + QStringLiteral("colortranslucency.frag"));
    QFile file(fragmentshader);
    if (file.open(QFile::ReadOnly))
    {
        QByteArray frag = file.readAll();
        auto shader = m_manager->generateShaderFromFile(KWin::ShaderTrait::MapTexture, QString(), fragmentshader);

#if KWIN_EFFECT_API_VERSION >= 235
        m_shader = std::move(shader);
#else
        m_shader.reset(shader);
#endif
        file.close();
        if (m_shader->isValid())
        {

            for (int i = 0; i < MAX_SETS; i++)
            {
                QString colorName = "targetColor[" + QString::number(i) + "]";
                m_shader_targetColor_locations[i] = m_shader->uniformLocation(colorName.toStdString().c_str());

                QString alphaName = "targetAlpha[" + QString::number(i) + "]";
                m_shader_targetAlpha_locations[i] = m_shader->uniformLocation(alphaName.toStdString().c_str());
                qInfo() << "ColorTranslucencyShader::ColorTranslucencyShader: fragment shader m_shader_targetColor_locations: " << m_shader_targetColor_locations;
                qInfo() << "ColorTranslucencyShader::ColorTranslucencyShader: fragment shader m_shader_targetAlpha_locations: " << m_shader_targetAlpha_locations;
            }

            m_shader_numberOfColors_location = m_shader->uniformLocation("numberOfColors");
            qInfo() << "ColorTranslucencyShader::ColorTranslucencyShader: shader created";
            qInfo() << "ColorTranslucencyShader::ColorTranslucencyShader: fragment shader path: " << fragmentshader;
        }
        else
            qCritical() << "ColorTranslucencyShader::ColorTranslucencyShader: no valid shaders found! ColorTranslucency will not work.";
    }
    else
    {
        qCritical() << "ColorTranslucencyShader::ColorTranslucencyShader: no shaders found!";
    }
}

bool ColorTranslucencyShader::IsValid() const
{
    bool isValid = m_shader && m_shader->isValid();
    qInfo() << "ColorTranslucencyShader::IsValid:" << isValid;
    return isValid;
}

const std::unique_ptr<KWin::GLShader> &
ColorTranslucencyShader::Bind(KWin::EffectWindow *) const
{
    qInfo() << "ColorTranslucencyShader::Bind called";

    QVector<QColor> targetColors = ColorTranslucencyEffect::getActiveColors();
    QVector<int> targetAlphas = ColorTranslucencyEffect::getActiveAlphas();

    qInfo() << "targetColors:";
    for (const auto& color : targetColors) {
        qInfo() << color;
    }
    qInfo() << "targetAlphas:";
    for (const auto& alpha : targetAlphas) {
        qInfo() << alpha;
    }

    m_manager->pushShader(m_shader.get());

    // Set normalized target colors
    for (int i = 0; i < targetColors.size(); i++)
    {
        QVector4D normalizedColor(targetColors[i].redF(), targetColors[i].greenF(), targetColors[i].blueF(), targetColors[i].alphaF());
        qInfo() << "Set target color" << i << "to" << normalizedColor << "at uniform location"<< m_shader_targetColor_locations[i];        
        m_shader->setUniform(m_shader_targetColor_locations[i], normalizedColor);
    }

    // Set normalized target alphas
    for (int i = 0; i < targetAlphas.size(); i++)
    {
        float normalizedAlpha = targetAlphas[i] / 255.0f;
        qInfo() << "Set target alpha" << i << "to" << normalizedAlpha  << "at uniform location" << m_shader_targetColor_locations[i];        
        m_shader->setUniform(m_shader_targetAlpha_locations[i], normalizedAlpha);
    }

    // Set number of active colors
    m_shader->setUniform(m_shader_numberOfColors_location, static_cast<int>(targetColors.size()));

    return m_shader;
}

void ColorTranslucencyShader::Unbind() const
{
    qInfo() << "ColorTranslucencyShader::Unbind called";
    m_manager->popShader();
}
