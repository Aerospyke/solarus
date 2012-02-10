/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * This module defines a type for the colors and provides some
 * color related functions.
 */
#include "lowlevel/Color.h"

SDL_PixelFormat * Color::format = NULL;

Color Color::black;
Color Color::white;
Color Color::red;
Color Color::green;
Color Color::blue;
Color Color::yellow;

/**
 * Initializes the color static fields.
 */
void Color::initialize(void) {

  format = SDL_GetVideoSurface()->format;

  black =    Color(  0,   0,   0); 
  white =    Color(255, 255, 255);
  red =      Color(255,   0,   0);
  green =    Color(  0, 255,   0);
  blue =     Color(  0,   0, 255);
  yellow =   Color(255, 255,   0);
}

/**
 * Uninitializes the color system.
 */
void Color::quit(void) {

}

/**
 * Creates a default color with unspecified RGB values.
 */
Color::Color(void) {
  internal_color.r = 0;
  internal_color.g = 0;
  internal_color.b = 0;
  internal_value = 0;
}

/**
 * Copy constructor.
 * @param other another color
 */
Color::Color(const Color &other):
  internal_value(other.internal_value), internal_color(other.internal_color) {

}

/**
 * Creates a color with the specified RGB values.
 * @param r the red component (from 0 to 255)
 * @param g the green component (from 0 to 255)
 * @param b the blue component (from 0 to 255)
 */
Color::Color(int r, int g, int b) {
  internal_color.r = r;
  internal_color.g = g;
  internal_color.b = b;

  internal_value = SDL_MapRGB(format, r, g, b);
}

/**
 * Returns the 32-bit value representing this color.
 * This function must be used only by low-level classes.
 * @return the 32-bit value of this color
 */
uint32_t Color::get_internal_value(void) {
  return internal_value;
}

/**
 * Returns the internal color encapsulated by this object.
 * This function must be used only by low-level classes.
 * @return the SDL color encapsulated
 */
SDL_Color * Color::get_internal_color(void) {
  return &internal_color;
}

