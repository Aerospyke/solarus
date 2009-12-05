/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_RECTANGLE_H
#define ZSDX_RECTANGLE_H

#include "Common.h"
#include <SDL/SDL.h>

/**
 * Represents a rectangle with its top-left corner, width and height.
 * This class can also be used to represent a single point
 * (then the width and the height are not used).
 * This class basically encapsulates a library-dependent rectangle object.
 */
class Rectangle {

  private:

    SDL_Rect rect; /**< the SDL_Rect encapsulated */

  public:

    Rectangle(void);
    Rectangle(int x, int y, int width, int height);
    Rectangle(const Rectangle &other);
    ~Rectangle(void);

    inline int get_x(void) const      { return rect.x; }
    inline int get_y(void) const      { return rect.y; }
    inline int get_width(void)  const { return rect.w; }
    inline int get_height(void) const { return rect.h; }

    bool contains(int x, int y) const;
    bool overlaps(const Rectangle &other) const;

    // for low-level classes use only
    SDL_Rect & get_internal_rect(void);
};

#endif

