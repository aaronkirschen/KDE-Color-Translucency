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

ColorTranslucencyKCM::ColorTranslucencyKCM(QObject *parent,
                                           const KPluginMetaData &args)
    : KCModule(parent, args), ui(new Ui::Form) {
  ui->setupUi(widget());
  addConfig(ColorTranslucencyConfig::self(), widget());


  connect(ui->kcfg_TargetColor_1, &KColorButton::changed, this,
          &ColorTranslucencyKCM::updateColor);
  connect(ui->kcfg_TargetColor_2, &KColorButton::changed, this,
          &ColorTranslucencyKCM::updateColor);
  connect(ui->kcfg_TargetColor_3, &KColorButton::changed, this,
          &ColorTranslucencyKCM::updateColor);
  connect(ui->kcfg_TargetColor_4, &KColorButton::changed, this,
          &ColorTranslucencyKCM::updateColor);
  connect(ui->kcfg_TargetColor_5, &KColorButton::changed, this,
          &ColorTranslucencyKCM::updateColor);
  connect(ui->kcfg_TargetColor_6, &KColorButton::changed, this,
          &ColorTranslucencyKCM::updateColor);
  connect(ui->kcfg_TargetColor_7, &KColorButton::changed, this,
          &ColorTranslucencyKCM::updateColor);
  connect(ui->kcfg_TargetColor_8, &KColorButton::changed, this,
          &ColorTranslucencyKCM::updateColor);
  connect(ui->kcfg_TargetColor_9, &KColorButton::changed, this,
          &ColorTranslucencyKCM::updateColor);
  connect(ui->kcfg_TargetColor_10, &KColorButton::changed, this,
          &ColorTranslucencyKCM::updateColor);

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

  connect(ui->includeButton, &QPushButton::pressed, this,
          &ColorTranslucencyKCM::includeButtonClicked);
  connect(ui->excludeButton, &QPushButton::pressed, this,
          &ColorTranslucencyKCM::excludeButtonClicked);
  connect(ui->deleteIncludeButton, &QPushButton::pressed, this,
          &ColorTranslucencyKCM::deleteIncludeButtonClicked);
  connect(ui->deleteExcludeButton, &QPushButton::pressed, this,
          &ColorTranslucencyKCM::deleteExcludeButtonClicked);

  // Update the window list every 1 second
  windowListUpdateTimer = new QTimer(this);
  connect(windowListUpdateTimer, &QTimer::timeout, this,
          &ColorTranslucencyKCM::updateWindows);
  windowListUpdateTimer->start(1000);
}

void ColorTranslucencyKCM::includeButtonClicked() {
  auto s = ui->currentWindowList->currentItem();
  if (s && ui->InclusionList->findItems(s->text(), Qt::MatchExactly).empty()) {
    ui->InclusionList->addItem(s->text());
    markAsChanged();
  }
}

void ColorTranslucencyKCM::excludeButtonClicked() {
  auto s = ui->currentWindowList->currentItem();
  if (s && ui->ExclusionList->findItems(s->text(), Qt::MatchExactly).empty()) {
    ui->ExclusionList->addItem(s->text());
    markAsChanged();
  }
}

void ColorTranslucencyKCM::deleteIncludeButtonClicked() {
  int row = ui->InclusionList->currentRow();
  if (row >= 0) {
    delete ui->InclusionList->takeItem(row);
    markAsChanged();
  }
}

void ColorTranslucencyKCM::deleteExcludeButtonClicked() {
  int row = ui->ExclusionList->currentRow();
  if (row >= 0) {
    delete ui->ExclusionList->takeItem(row);
    markAsChanged();
  }
}

void ColorTranslucencyKCM::updateColor(const QColor &color) {
  // Assumes KColorButton and KGradientSelector widgets follow naming
  // convention:
  //   kcfg_TargetColor_<index>
  //   kcfg_TargetAlpha_<index>
  // where index starts at 1.
  KColorButton *senderButton = qobject_cast<KColorButton *>(sender());
  if (!senderButton) {
    qWarning() << "Sender is not a KColorButton";
    return;
  }

  QString senderName = senderButton->objectName();
  int index = senderName.mid(senderName.lastIndexOf('_') + 1).toInt();

  QString alphaWidgetName = QString("kcfg_TargetAlpha_%1").arg(index);
  KGradientSelector *alphaWidget =
      ui->tab_1->findChild<KGradientSelector *>(alphaWidgetName);

  if (alphaWidget) {
    alphaWidget->setSecondColor(color);
  } else {
    qWarning() << "Alpha widget" << alphaWidgetName
               << "not found for index:" << index;
  }
}

void ColorTranslucencyKCM::updateWindows() {

  // Save currently selected item
  QListWidgetItem *currentlySelectedItem = ui->currentWindowList->currentItem();
  QString currentlySelectedText;
  if (currentlySelectedItem) {
    currentlySelectedText = currentlySelectedItem->text();
  }

  // Clear the list
  ui->currentWindowList->clear();

  // Get window list
  QList<QString> windowList;
  auto connection = QDBusConnection::sessionBus();
  if (connection.isConnected()) {
    QDBusInterface interface("org.kde.ColorTranslucency",
                             "/ColorTranslucencyEffect");
    if (interface.isValid()) {
      QDBusReply<QString> reply = interface.call("get_window_titles");
      if (reply.isValid())
        windowList = reply.value().split("\n");
      qDebug() << "ColorTranslucencyKCM::updateWindows: windowList:"
              << windowList;
    }
  }

  // Update window list
  for (const auto &w : windowList) {
    if (!w.isEmpty()) {
      QListWidgetItem *item = new QListWidgetItem(w);
      ui->currentWindowList->addItem(item);
      qDebug() << "ColorTranslucencyKCM::updateWindows: adding window:" << w;

      // Reselect the previously selected item if it exists in the new list
      if (w == currentlySelectedText) {
        ui->currentWindowList->setCurrentItem(item);
      }
    }
  }
}

void ColorTranslucencyKCM::save() {
  qDebug() << "ColorTranslucencyKCM::save: saving config";
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
  qDebug() << "ColorTranslucencyKCM::load: loading config, inclusions:"
          << ColorTranslucencyConfig::inclusionList()
          << ", exclusions: " << ColorTranslucencyConfig::exclusionList();
  updateWindows();
}

void ColorTranslucencyKCM::defaults() {
  qDebug() << "ColorTranslucencyKCM::defaults: reset to defaults";
  KCModule::defaults();
  ColorTranslucencyConfig::self()->setDefaults();

  ui->InclusionList->clear();
  ui->InclusionList->addItems(ColorTranslucencyConfig::inclusionList());
  ui->ExclusionList->clear();
  ui->ExclusionList->addItems(ColorTranslucencyConfig::exclusionList());
}