/*
 * Modifications to support color translucency effect.
 * Copyright (c) 2024 Aaron Kirschen
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

#ifndef COLORTRANSLUCENCYKCM_H
#define COLORTRANSLUCENCYKCM_H

#include "ColorTranslucencyConfig.h"
#include "ui_ColorTranslucencyKCM.h"
#include <kcmodule.h>

class ColorTranslucencyKCM : public KCModule {
  Q_OBJECT
public:
  explicit ColorTranslucencyKCM(QObject *parent, const KPluginMetaData &args);

public slots:
  void defaults() override;
  void load() override;
  void save() override;
  void updateColor(const QColor &color);
  void updateWindows();

private slots:
  void includeButtonClicked();
  void excludeButtonClicked();
  void deleteIncludeButtonClicked();
  void deleteExcludeButtonClicked();

private:
  Ui::Form *ui;
  QTimer *windowListUpdateTimer;
};

#endif // COLORTRANSLUCENCYKCM_H