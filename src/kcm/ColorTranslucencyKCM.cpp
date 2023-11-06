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

#include <QDialog>
#include <QList>

#include <kwineffects.h>
#include "kwineffects_interface.h"
#include "ColorTranslucencyKCM.h"

#include <QDBusConnection>


ColorTranslucencyKCM::ColorTranslucencyKCM(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args), ui(new Ui::Form)
{
    ui->setupUi(this);

    addConfig(ColorTranslucencyConfig::self(), this);

    ui->kcfg_TargetColor->setDefaultColor(Qt::black);
    ui->kcfg_TargetColor->setColor(ui->kcfg_TargetColor->defaultColor());

    connect(ui->kcfg_TargetColor, &KColorButton::changed, this, &ColorTranslucencyKCM::update_colors);

    connect(ui->refreshButton, &QPushButton::pressed, this, &ColorTranslucencyKCM::update_windows);
    connect(ui->includeButton, &QPushButton::pressed, [=, this]()
            {
        auto s = ui->currentWindowList->currentItem();
        if (s && ui->InclusionList->findItems(s->text(), Qt::MatchExactly).empty())
            ui->InclusionList->addItem(s->text()); });
    connect(ui->excludeButton, &QPushButton::pressed, [=, this]()
            {
        auto s = ui->currentWindowList->currentItem();
        if (s && ui->ExclusionList->findItems(s->text(), Qt::MatchExactly).empty())
            ui->ExclusionList->addItem(s->text()); });
    connect(ui->deleteIncludeButton, &QPushButton::pressed, [=, this]()
            { ui->InclusionList->takeItem(ui->InclusionList->currentRow()); });
    connect(ui->deleteExcludeButton, &QPushButton::pressed, [=, this]()
            { ui->ExclusionList->takeItem(ui->ExclusionList->currentRow()); });
}

void ColorTranslucencyKCM::save()
{
    qDebug() << "ColorTranslucencyKCM::save - Saving config";
    QColor targetColor = ui->kcfg_TargetColor->color();
    qDebug() << "ColorTranslucencyKCM::save - Saving TargetColor from UI:" << targetColor;
   
    QStringList inclusions, exclusions;
    for (int i = 0; i < ui->InclusionList->count(); ++i)
        inclusions.push_back(ui->InclusionList->item(i)->text());
    for (int i = 0; i < ui->ExclusionList->count(); ++i)
        exclusions.push_back(ui->ExclusionList->item(i)->text());

    ColorTranslucencyConfig::setInclusions(inclusions);
    ColorTranslucencyConfig::setExclusions(exclusions);
    ColorTranslucencyConfig::self()->save();
    KCModule::save();
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("kwin4_effect_colortranslucency"));
        
}

ColorTranslucencyKCM::~ColorTranslucencyKCM()
{
    delete ui;
}

void ColorTranslucencyKCM::update_colors()
{
    QColor color;
    qDebug() << "ColorTranslucencyKCM::update_colors - TargetColor:" << color;
    color = ui->kcfg_TargetColor->color();
    ui->kcfg_TargetAlpha->setSecondColor(color);
}

void ColorTranslucencyKCM::update_windows()
{
    QList<QString> windowList;
    ui->currentWindowList->clear();

    auto connection = QDBusConnection::sessionBus();
    if (connection.isConnected())
    {
        QDBusInterface interface("org.kde.ColorTranslucency", "/ColorTranslucencyEffect");
        if (interface.isValid())
        {
            QDBusReply<QString> reply = interface.call("get_window_titles");
            if (reply.isValid())
                windowList = reply.value().split("\n");
        }
    }

    for (const auto &w : windowList)
        if (!w.isEmpty())
            ui->currentWindowList->addItem(w);
}

void ColorTranslucencyKCM::load()
{
    KCModule::load();
    ColorTranslucencyConfig::self()->load();
    ui->InclusionList->addItems(ColorTranslucencyConfig::inclusions());
    ui->ExclusionList->addItems(ColorTranslucencyConfig::exclusions());
}

void ColorTranslucencyKCM::defaults()
{
    
    KCModule::defaults();

    ui->kcfg_TargetAlpha->setValue(0);
    ui->kcfg_TargetColor->setColor(ui->kcfg_TargetColor->defaultColor());

    ColorTranslucencyConfig::self()->setDefaults();

    ui->InclusionList->clear();
    ui->InclusionList->addItems(ColorTranslucencyConfig::inclusions());
    ui->ExclusionList->clear();
    ui->ExclusionList->addItems(ColorTranslucencyConfig::exclusions());
        qDebug() << "ColorTranslucencyKCM::defaults - Resetting to defaults";
}
