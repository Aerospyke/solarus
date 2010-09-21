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
#ifndef SOLARUS_COLOR_H
#define SOLARUS_COLOR_H

#include "Common.h"
#include "SDL/SDL.h"

/**
 * @brief Represents a color.
 *
 * This module defines a type for the colors and provides some color related functions.
 * It encapsulates a library-dependent color.
 */
class Color {

  private:

    static SDL_PixelFormat *format; /**< the pixel format used for all colors */
    uint32_t internal_value;        /**< the SDL 32-bit value representing this color */
    SDL_Color internal_color;       /**< the SDL color encapsulated */

    // some predefined colors
    static Color black;
    static Color white;
    static Color red;
    static Color green;
    static Color blue;
    static Color yellow;

  public:

    static void initialize();
    static void quit();

    static inline Color & get_black()    { return black;    }
    static inline Color & get_white()    { return white;    }
    static inline Color & get_red()      { return red;      }
    static inline Color & get_green()    { return green;    }
    static inline Color & get_blue()     { return blue ;    }
    static inline Color & get_yellow()   { return yellow;   }

    Color();
    Color(const Color &other);
    Color(int r, int g, int b);

    uint32_t get_internal_value();
    SDL_Color * get_internal_color();

};

#endif

