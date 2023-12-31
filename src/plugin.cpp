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

KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED(  ColorTranslucencyEffect,
                                        "metadata.json",
                                        return ColorTranslucencyEffect::supported();,
                                        return ColorTranslucencyEffect::enabledByDefault();)

#include "plugin.moc"
