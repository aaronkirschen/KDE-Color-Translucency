/*
 * Copyright © 2015 Robert Metsäranta <therealestrob@gmail.com>
 *
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
#include "ColorTranslucencyEffect.h"
#include "ColorTranslucencyConfig.h"

#include <KX11Extras>
#include <QDBusError>
#include <QtDBus/QDBusConnection>

#if QT_VERSION_MAJOR >= 6
#include <core/output.h>
#include <core/renderviewport.h>
#include <effect/effecthandler.h>
#include <opengl/glutils.h>
#else
#include <kwinglutils.h>
#endif

QVector<QColor> ColorTranslucencyEffect::m_activeColors;
QVector<int> ColorTranslucencyEffect::m_activeAlphas;
ColorTranslucencyEffect::ColorTranslucencyEffect()
#if KWIN_EFFECT_API_VERSION >= 236
    : KWin::OffscreenEffect()
#else
    : KWin::DeformEffect()
#endif
{

  reconfigure(ReconfigureAll);

  auto connection = QDBusConnection::sessionBus();
  if (!connection.isConnected()) {
    qWarning("ColorTranslucency: Cannot connect to the D-Bus session bus.\n");
  } else {
    if (!connection.registerService("org.kde.ColorTranslucency")) {
      qWarning("%s\n", qPrintable(connection.lastError().message()));
    } else {
      if (!connection.registerObject("/ColorTranslucencyEffect", this,
                                     QDBusConnection::ExportAllSlots)) {
        qWarning("%s\n", qPrintable(connection.lastError().message()));
      }
    }
  }

  if (m_shaderManager.IsValid()) {
    for (const auto &win : KWin::effects->stackingOrder())
      windowAdded(win);
    connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this,
            &ColorTranslucencyEffect::windowAdded);
    connect(KWin::effects, &KWin::EffectsHandler::windowDeleted, this,
            &ColorTranslucencyEffect::windowRemoved);
  }
}

ColorTranslucencyEffect::~ColorTranslucencyEffect() = default;

void ColorTranslucencyEffect::windowAdded(KWin::EffectWindow *w) {
  const QSet<QString> hardExceptions{"plasmashell", "kscreenlocker_greet",
                                     "ksmserver", "krunner"};
  const auto name = w->windowClass().split(QChar::Space).first();
  if (hardExceptions.contains(name))
    return;
  auto r = m_managed.insert(w);

  if (r.second) {

    redirect(w);
    setShader(w, m_shaderManager.GetShader().get());
  }
}

void ColorTranslucencyEffect::windowRemoved(KWin::EffectWindow *w) {
  m_managed.erase(w);
  unredirect(w);
}

QVector<QColor> activeTargetColors() {
  QVector<QColor> colors;

  if (ColorTranslucencyConfig::enableColor_1()) {
    colors.push_back(ColorTranslucencyConfig::targetColor_1());
  }
  if (ColorTranslucencyConfig::enableColor_2()) {
    colors.push_back(ColorTranslucencyConfig::targetColor_2());
  }
  if (ColorTranslucencyConfig::enableColor_3()) {
    colors.push_back(ColorTranslucencyConfig::targetColor_3());
  }
  if (ColorTranslucencyConfig::enableColor_4()) {
    colors.push_back(ColorTranslucencyConfig::targetColor_4());
  }
  if (ColorTranslucencyConfig::enableColor_5()) {
    colors.push_back(ColorTranslucencyConfig::targetColor_5());
  }
  if (ColorTranslucencyConfig::enableColor_6()) {
    colors.push_back(ColorTranslucencyConfig::targetColor_6());
  }
  if (ColorTranslucencyConfig::enableColor_7()) {
    colors.push_back(ColorTranslucencyConfig::targetColor_7());
  }
  if (ColorTranslucencyConfig::enableColor_8()) {
    colors.push_back(ColorTranslucencyConfig::targetColor_8());
  }
  if (ColorTranslucencyConfig::enableColor_9()) {
    colors.push_back(ColorTranslucencyConfig::targetColor_9());
  }
  if (ColorTranslucencyConfig::enableColor_10()) {
    colors.push_back(ColorTranslucencyConfig::targetColor_10());
  }

  return colors;
}

QVector<int> activeTargetAlphas() {
  QVector<int> alphas;

  if (ColorTranslucencyConfig::enableColor_1()) {
    alphas.push_back(ColorTranslucencyConfig::targetAlpha_1());
  }
  if (ColorTranslucencyConfig::enableColor_2()) {
    alphas.push_back(ColorTranslucencyConfig::targetAlpha_2());
  }
  if (ColorTranslucencyConfig::enableColor_3()) {
    alphas.push_back(ColorTranslucencyConfig::targetAlpha_3());
  }
  if (ColorTranslucencyConfig::enableColor_4()) {
    alphas.push_back(ColorTranslucencyConfig::targetAlpha_4());
  }
  if (ColorTranslucencyConfig::enableColor_5()) {
    alphas.push_back(ColorTranslucencyConfig::targetAlpha_5());
  }
  if (ColorTranslucencyConfig::enableColor_6()) {
    alphas.push_back(ColorTranslucencyConfig::targetAlpha_6());
  }
  if (ColorTranslucencyConfig::enableColor_7()) {
    alphas.push_back(ColorTranslucencyConfig::targetAlpha_7());
  }
  if (ColorTranslucencyConfig::enableColor_8()) {
    alphas.push_back(ColorTranslucencyConfig::targetAlpha_8());
  }
  if (ColorTranslucencyConfig::enableColor_9()) {
    alphas.push_back(ColorTranslucencyConfig::targetAlpha_9());
  }
  if (ColorTranslucencyConfig::enableColor_10()) {
    alphas.push_back(ColorTranslucencyConfig::targetAlpha_10());
  }

  return alphas;
}

QVector<QColor> ColorTranslucencyEffect::getActiveColors() {

  return m_activeColors;
}

QVector<int> ColorTranslucencyEffect::getActiveAlphas() {

  return m_activeAlphas;
}

void ColorTranslucencyEffect::reconfigure(ReconfigureFlags flags) {
  Q_UNUSED(flags)
  ColorTranslucencyConfig::self()->read();

  m_activeColors = activeTargetColors();
  m_activeAlphas = activeTargetAlphas();
}

void ColorTranslucencyEffect::prePaintWindow(KWin::EffectWindow *w,
                                             KWin::WindowPrePaintData &data,
                                             std::chrono::milliseconds time) {

  if (!hasEffect(w)) {
    Effect::prePaintWindow(w, data, time);
    return;
  }

  data.setTranslucent();

  OffscreenEffect::prePaintWindow(w, data, time);
}

bool ColorTranslucencyEffect::supported() {
  bool isSupported = KWin::effects->isOpenGLCompositing();
  return isSupported;
}

#if QT_VERSION_MAJOR >= 6
void ColorTranslucencyEffect::drawWindow(const KWin::RenderTarget &renderTarget,
                                         const KWin::RenderViewport &viewport,
                                         KWin::EffectWindow *w, int mask,
                                         const QRegion &region,
                                         KWin::WindowPaintData &data) {
#else
void ColorTranslucencyEffect::drawWindow(KWin::EffectWindow *w, int mask,
                                         const QRegion &region,
                                         KWin::WindowPaintData &data) {
#endif

  if (!hasEffect(w)) {
    unredirect(w);
#if QT_VERSION_MAJOR >= 6
    OffscreenEffect::drawWindow(renderTarget, viewport, w, mask, region, data);
#else
    OffscreenEffect::drawWindow(w, mask, region, data);
#endif
    return;
  }

  redirect(w);
  setShader(w, m_shaderManager.GetShader().get());
  m_shaderManager.Bind(w);
  glActiveTexture(GL_TEXTURE0);

#if QT_VERSION_MAJOR >= 6
  OffscreenEffect::drawWindow(renderTarget, viewport, w, mask, region, data);
#else
  OffscreenEffect::drawWindow(w, mask, region, data);
#endif
  m_shaderManager.Unbind();
}

QString
ColorTranslucencyEffect::get_window_title(const KWin::EffectWindow *w) const {
  auto fullClass = w->windowClass();

  QStringList parts = fullClass.split(' ');

  QString windowTitle;
  if (parts.size() > 1 && parts[0] == parts[1]) {
    windowTitle = parts[0];
  } else {
    windowTitle = fullClass;
  }

  return windowTitle;
}

bool ColorTranslucencyEffect::hasEffect(const KWin::EffectWindow *w) const {

  if (!m_shaderManager.IsValid()) {
    return false;
  }

  QStringList inclusions = ColorTranslucencyConfig::inclusionList();
  QStringList exclusions = ColorTranslucencyConfig::exclusionList();

  QString windowTitle = get_window_title(w);

  if (!m_managed.contains(w)) {
    return false;
  }

  if (inclusions.contains(windowTitle, Qt::CaseInsensitive)) {
    return true;
  }

  if (exclusions.contains(windowTitle, Qt::CaseInsensitive)) {

    return false;
  }

  return false;
}

QString ColorTranslucencyEffect::get_window_titles() const {
  QStringList response;
  for (const auto &win : m_managed) {
    const auto name = win->windowClass();
    if (!response.contains(name))
      response.push_back(name);
  }

  return response.join("\n");
}