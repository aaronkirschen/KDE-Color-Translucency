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

precision mediump float;

#include "colormanagement.glsl"

uniform sampler2D sampler;

#define MAX_SETS 10 // Maximum of 10 colors
uniform vec4 targetColor[MAX_SETS];
uniform float targetAlpha[MAX_SETS];
uniform int numberOfColors;

varying vec2 texcoord0;

void main() {
    vec4 tex = texture2D(sampler, texcoord0);

    for (int i = 0; i < numberOfColors; ++i) {
        if (tex.rgb == targetColor[i].rgb) {
            tex.a = targetAlpha[i]; // Set the alpha to the matched target alpha
            break;                  // Exit the loop early since we found a match
        }
    }

    tex.rgb *= tex.a;   // Premultiply color by alpha

    tex = sourceEncodingToNitsInDestinationColorspace(tex); // Apply HDR color management
    gl_FragColor = nitsToDestinationEncoding(tex); // Set the final color of the pixel
}