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
#include "hud/HudElement.h"
#include "lowlevel/Color.h"
#include "lowlevel/Surface.h"
#include "lowlevel/System.h"
#include "Savegame.h"

/**
 * Constructor.
 * @param x x coordinate of the top-left corner of the hud element surface on the destination surface
 * @param y y coordinate of the top-left corner of the hud element surface on the destination surface
 * @param width width of the hud element surface
 * @param height height of the hud element surface
 */
HudElement::HudElement(int x, int y, int width, int height):
  visible(true), opacity(255), blinking(false) {

  surface_drawn = new Surface(width, height);
  surface_drawn->set_transparency_color(Color::get_black());

  set_position(x, y);
}

/**
 * Destructor.
 */
HudElement::~HudElement(void) {
  delete surface_drawn;
}

/**
 * Changes the position of this hud element.
 */
void HudElement::set_position(int x, int y) {
  
  destination_position.set_x(x);
  destination_position.set_y(y);
}

/**
 * Redefine this method to redraw the element on the surface.
 * You have to call this method from the subclasses to initialize
 * the surface with transparent color.
 */
void HudElement::rebuild(void) {
  surface_drawn->fill_with_color(Color::get_black());
}

/**
 * Shows or hides this element.
 * @param visible true to make it visible, false to hide it
 */
void HudElement::set_visible(bool visible) {
  this->visible = visible;
}

/**
 * Returns whether this hud element is visible.
 * The display() function does nothing if this function
 * returns false.
 * @return true if this hud element is visible, false otherwise
 */
bool HudElement::is_visible(void) {
  return visible;
}

/**
 * Returns the current opacity of this element.
 * @return the current opacity, between 0 and 255
 */
int HudElement::get_opacity(void) {
  return opacity;
}

/**
 * Sets the opacity of this element.
 * @param opacity the opacity, between 0 and 255
 */
void HudElement::set_opacity(int opacity) {

  if (opacity != this->opacity) {
    this->opacity = opacity;
    surface_drawn->set_opacity(opacity);
    rebuild();
  }
}

/**
 * Makes this element blink.
 */
void HudElement::set_blinking(int blinking) {

  if (blinking != this->blinking) {

    this->blinking = blinking;
    if (blinking) {
      next_blink_date = System::now();
      blinking_is_visible = false;
    }
  }
}

/**
 * Returns whether this element is blinking.
 * @return true if this element is blinking
 */
bool HudElement::is_blinking(void) {
  return blinking;
}

/**
 * Updates this HUD element.
 */
void HudElement::update(void) {

  uint32_t now = System::now();
  if (blinking && now >= next_blink_date) {
    blinking_is_visible = !blinking_is_visible;
    next_blink_date = now + 250;
  }
}

/**
 * Displays the hud element on a surface.
 * If the savegame is empty, nothing is done.
 * @param destination the destination surface
 */
void HudElement::display(Surface *destination) {

  if (is_visible() && (!blinking || blinking_is_visible)) {
    surface_drawn->blit(destination, destination_position);
  }
}

