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

#include <kcmodule.h>
#include "ui_ColorTranslucencyKCM.h"
#include "ColorTranslucencyConfig.h"

class ColorTranslucencyKCM : public KCModule
{
    Q_OBJECT
public:
#if (QT_VERSION_MAJOR >= 6)
    explicit ColorTranslucencyKCM(QObject* parent, const KPluginMetaData& args);
#else
    explicit ColorTranslucencyKCM(QWidget* parent = nullptr, const QVariantList& args = QVariantList());
#endif

public slots:
    void defaults() override;
    void load() override;
    void save() override;
    void updateColor(int);
    void updateWindows();  

private:
    Ui::Form *ui;

    QColor m_color1;
    QColor m_color2;
    QColor m_color3;
    QColor m_color4;
    QColor m_color5;
    QColor m_color6;
    QColor m_color7;
    QColor m_color8;
    QColor m_color9;
    QColor m_color10;

    int m_alpha1;
    int m_alpha2;
    int m_alpha3;
    int m_alpha4;
    int m_alpha5;
    int m_alpha6;
    int m_alpha7;
    int m_alpha8;
    int m_alpha9;
    int m_alpha10;
};
