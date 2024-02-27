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

#version 140

#define MAX_SETS 10

uniform sampler2D sampler;
uniform vec4 targetColor[MAX_SETS];
uniform float targetAlpha[MAX_SETS];
uniform int numberOfColors;

in vec2 texcoord0;
out vec4 fragColor;

void main() {

  vec4 tex = texture(sampler, texcoord0);

  for (int i = 0; i < numberOfColors; ++i) {
    if (tex.rgb == targetColor[i].rgb) {
      tex.a = targetAlpha[i];
      break;
    }
  }

  tex.rgb *= tex.a;
  fragColor = tex;
}