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

precision mediump float;

uniform sampler2D sampler;
uniform vec4 targetColor;
uniform float targetAlpha;

varying vec2 texcoord0;

void main() {
    vec4 tex = texture2D(sampler, texcoord0);
    if(tex.rgb == targetColor.rgb) {
        tex.a = targetAlpha;
    }
    tex.rgb *= tex.a; // Multiply the color by the alpha
    gl_FragColor = tex;
}
