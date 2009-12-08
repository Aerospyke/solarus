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
#include "lowlevel/Rectangle.h"

/**
 * Creates a rectangle without initializing its properties.
 */
Rectangle::Rectangle(void) {

}

/**
 * Creates a point (i.e. a rectangle with width and height set to zero).
 * @param x x coordinate of the point
 * @param y y coordinate of the point
 */
Rectangle::Rectangle(int x, int y) {
  rect.x = x;
  rect.y = y;
  rect.w = 0;
  rect.h = 0;
}


/**
 * Creates a rectangle, specifying its properties.
 * @param x x coordinate of the top-left corner
 * @param y y coordinate of the top-left corner
 * @param width the rectangle's width
 * @param height the rectangle's height
 */
Rectangle::Rectangle(int x, int y, int width, int height) {
  rect.x = x;
  rect.y = y;
  rect.w = width;
  rect.h = height;
}

/**
 * Copy constructor.
 * @param other the rectangle to copy
 */
Rectangle::Rectangle(const Rectangle &other):
  rect(other.rect) {

}

/**
 * Destructor.
 */
Rectangle::~Rectangle(void) {

}

/**
 * Returns whether the specified point is inside this rectangle.
 * @param x x coordinate of the point
 * @param y y coordinate of the point
 * @return true if the point is in this rectangle
 */
bool Rectangle::contains(int x, int y) const {
  return x >= get_x() && x < get_x() + get_width() && y >= get_y() && y < get_y() + get_height();
}

/**
 * Returns whether or not another rectangle overlaps this one.
 * @param other another rectangle
 * @return true if the two rectangles overlap
 */
bool Rectangle::overlaps(const Rectangle &other) const {

  int x1 = get_x();
  int x2 = x1 + get_width();
  int x3 = other.get_x();
  int x4 = x3 + other.get_width();

  bool overlap_x = (x3 < x2 && x1 < x4);

  int y1 = get_y();
  int y2 = y1 + get_height();
  int y3 = other.get_y();
  int y4 = y3 + other.get_height();

  bool overlap_y = (y3 < y2 && y1 < y4);

  return overlap_x && overlap_y;
}

/**
 * Returns the center point of this rectangle
 * @return the center point (represented as a zero-sized rectangle)
 */
Rectangle Rectangle::get_center(void) {
  return Rectangle(get_x() + get_width() / 2, get_y() + get_height() / 2);
}

/**
 * Returns the SDL rectangle encapsulated by this object.
 * This function must be used only by other low-level classes (typically Surface)
 * as it is library dependent.
 * @return the internal rectangle encapsulated
 */
SDL_Rect & Rectangle::get_internal_rect(void) {
  return rect;
}

