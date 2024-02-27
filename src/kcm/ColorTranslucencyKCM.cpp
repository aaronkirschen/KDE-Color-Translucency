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

#include "ColorTranslucencyKCM.h"
#include "ui_ColorTranslucencyKCM.h"

#include "kwineffects_interface.h"
#include <QDialog>
#include <QListWidgetItem>

#if (QT_VERSION_MAJOR >= 6)
ColorTranslucencyKCM::ColorTranslucencyKCM(QObject *parent,
                                           const KPluginMetaData &args)
    : KCModule(parent, args), ui(new Ui::Form) {
  ui->setupUi(widget());
  addConfig(ColorTranslucencyConfig::self(), widget());
#else
ColorTranslucencyKCM::ColorTranslucencyKCM(QWidget *parent,
                                           const QVariantList &args)
    : KCModule(parent, args), ui(new Ui::Form) {
  ui->setupUi(this);
  addConfig(ColorTranslucencyConfig::self(), this);
#endif

  connect(ui->kcfg_TargetColor_1, &KColorButton::changed, this,
          [this]() { updateColor(1); });
  connect(ui->kcfg_TargetColor_2, &KColorButton::changed, this,
          [this]() { updateColor(2); });
  connect(ui->kcfg_TargetColor_3, &KColorButton::changed, this,
          [this]() { updateColor(3); });
  connect(ui->kcfg_TargetColor_4, &KColorButton::changed, this,
          [this]() { updateColor(4); });
  connect(ui->kcfg_TargetColor_5, &KColorButton::changed, this,
          [this]() { updateColor(5); });
  connect(ui->kcfg_TargetColor_6, &KColorButton::changed, this,
          [this]() { updateColor(6); });
  connect(ui->kcfg_TargetColor_7, &KColorButton::changed, this,
          [this]() { updateColor(7); });
  connect(ui->kcfg_TargetColor_8, &KColorButton::changed, this,
          [this]() { updateColor(8); });
  connect(ui->kcfg_TargetColor_9, &KColorButton::changed, this,
          [this]() { updateColor(9); });
  connect(ui->kcfg_TargetColor_10, &KColorButton::changed, this,
          [this]() { updateColor(10); });

  // the gradient selector is not marking as changed, so I am manually
  // connecting it to markAsChanged
  connect(ui->kcfg_TargetAlpha_1, &KGradientSelector::valueChanged, this,
          &ColorTranslucencyKCM::markAsChanged);
  connect(ui->kcfg_TargetAlpha_2, &KGradientSelector::valueChanged, this,
          &ColorTranslucencyKCM::markAsChanged);
  connect(ui->kcfg_TargetAlpha_3, &KGradientSelector::valueChanged, this,
          &ColorTranslucencyKCM::markAsChanged);
  connect(ui->kcfg_TargetAlpha_4, &KGradientSelector::valueChanged, this,
          &ColorTranslucencyKCM::markAsChanged);
  connect(ui->kcfg_TargetAlpha_5, &KGradientSelector::valueChanged, this,
          &ColorTranslucencyKCM::markAsChanged);
  connect(ui->kcfg_TargetAlpha_6, &KGradientSelector::valueChanged, this,
          &ColorTranslucencyKCM::markAsChanged);
  connect(ui->kcfg_TargetAlpha_7, &KGradientSelector::valueChanged, this,
          &ColorTranslucencyKCM::markAsChanged);
  connect(ui->kcfg_TargetAlpha_8, &KGradientSelector::valueChanged, this,
          &ColorTranslucencyKCM::markAsChanged);
  connect(ui->kcfg_TargetAlpha_9, &KGradientSelector::valueChanged, this,
          &ColorTranslucencyKCM::markAsChanged);
  connect(ui->kcfg_TargetAlpha_10, &KGradientSelector::valueChanged, this,
          &ColorTranslucencyKCM::markAsChanged);

  connect(ui->refreshButton, &QPushButton::pressed, this,
          &ColorTranslucencyKCM::updateWindows);

  // It may be cleaner to have slots for these, like includeButtonClicked,
  // excludeButtonClicked, deleteIncludeClicked, deleteExcludeClicked the
  // inclusion/exclusion lists are not marking as changed, so I am manually
  // connecting it to markAsChanged

  connect(ui->includeButton, &QPushButton::pressed, [=, this]() {
    auto s = ui->currentWindowList->currentItem();
    if (s &&
        ui->InclusionList->findItems(s->text(), Qt::MatchExactly).empty()) {
      ui->InclusionList->addItem(s->text());
      markAsChanged();
    }
  });

  connect(ui->excludeButton, &QPushButton::pressed, [=, this]() {
    auto s = ui->currentWindowList->currentItem();
    if (s &&
        ui->ExclusionList->findItems(s->text(), Qt::MatchExactly).empty()) {
      ui->ExclusionList->addItem(s->text());
      markAsChanged();
    }
  });

  connect(ui->deleteIncludeButton, &QPushButton::pressed, [=, this]() {
    int row = ui->InclusionList->currentRow();
    if (row >= 0) {
      delete ui->InclusionList->takeItem(row);
      markAsChanged();
    }
  });
  connect(ui->deleteExcludeButton, &QPushButton::pressed, [=, this]() {
    int row = ui->ExclusionList->currentRow();
    if (row >= 0) {
      delete ui->ExclusionList->takeItem(row);
      markAsChanged();
    }
  });
}

void ColorTranslucencyKCM::updateColor(int index) {
  qInfo() << "ColorTranslucencyKCM::updateColor" << index;
  if (index < 1 || index > 10) {
    qInfo() << "Invalid index for updateColor: " << index;
    return;
  }

  // Construct the widget names based on the index
  QString colorWidgetName = QString("kcfg_TargetColor_%1").arg(index);
  QString alphaWidgetName = QString("kcfg_TargetAlpha_%1").arg(index);

  // Find the widgets by name
  KColorButton *colorWidget = findChild<KColorButton *>(colorWidgetName);
  KGradientSelector *alphaWidget =
      findChild<KGradientSelector *>(alphaWidgetName);

  // Perform the necessary operations if the widgets are found
  if (colorWidget && alphaWidget) {
    QColor color = colorWidget->color();
    alphaWidget->setSecondColor(color);
  } else {
    qInfo() << "Widgets not found for index: " << index;
    if (!colorWidget)
      qInfo() << "Color widget not found: " << colorWidgetName;
    if (!alphaWidget)
      qInfo() << "Alpha widget not found: " << alphaWidgetName;
  }
}

void ColorTranslucencyKCM::updateWindows() {

  QList<QString> windowList;
  ui->currentWindowList->clear();

  auto connection = QDBusConnection::sessionBus();
  if (connection.isConnected()) {
    QDBusInterface interface("org.kde.ColorTranslucency",
                             "/ColorTranslucencyEffect");
    if (interface.isValid()) {
      QDBusReply<QString> reply = interface.call("get_window_titles");
      if (reply.isValid())
        windowList = reply.value().split("\n");
      qInfo() << "ColorTranslucencyKCM::updateWindows: windowList:"
              << windowList;
    }
  }

  for (const auto &w : windowList)
    if (!w.isEmpty()) {
      ui->currentWindowList->addItem(w);
      qInfo() << "ColorTranslucencyKCM::updateWindows: adding window:" << w;
    }
}

void ColorTranslucencyKCM::save() {
  qInfo() << "ColorTranslucencyKCM::save: saving config";
  QStringList inclusions, exclusions;
  for (int i = 0; i < ui->InclusionList->count(); ++i)
    inclusions.push_back(ui->InclusionList->item(i)->text());
  for (int i = 0; i < ui->ExclusionList->count(); ++i)
    exclusions.push_back(ui->ExclusionList->item(i)->text());

  ColorTranslucencyConfig::setInclusionList(inclusions);
  ColorTranslucencyConfig::setExclusionList(exclusions);
  ColorTranslucencyConfig::self()->save();
  KCModule::save();
  OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                       QStringLiteral("/Effects"),
                                       QDBusConnection::sessionBus());
  interface.reconfigureEffect(QStringLiteral("kwin4_effect_colortranslucency"));
}

void ColorTranslucencyKCM::load() {
  KCModule::load();
  ColorTranslucencyConfig::self()->load();
  ui->InclusionList->addItems(ColorTranslucencyConfig::inclusionList());
  ui->ExclusionList->addItems(ColorTranslucencyConfig::exclusionList());
  qInfo() << "ColorTranslucencyKCM::load: loading config, inclusions:"
          << ColorTranslucencyConfig::inclusionList()
          << ", exclusions: " << ColorTranslucencyConfig::exclusionList();
}

void ColorTranslucencyKCM::defaults() {
  qInfo() << "ColorTranslucencyKCM::defaults: reset to defaults";
  KCModule::defaults();
  ColorTranslucencyConfig::self()->setDefaults();

  ui->InclusionList->clear();
  ui->InclusionList->addItems(ColorTranslucencyConfig::inclusionList());
  ui->ExclusionList->clear();
  ui->ExclusionList->addItems(ColorTranslucencyConfig::exclusionList());
}