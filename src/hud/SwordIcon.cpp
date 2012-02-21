/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "hud/SwordIcon.h"
#include "Equipment.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "lowlevel/Surface.h"

/**
 * @brief Constructor.
 * @param game the current game
 * @param x x coordinate of the top-left corner of the icon on the destination surface
 * @param y y coordinate of the top-left corner of the icon on the destination surface
 */
SwordIcon::SwordIcon(Game &game, int x, int y):
  HudElement(game, x, y, 72, 24) {

  // static icon
  this->sword_number_displayed = (equipment != NULL) ? equipment->get_ability("sword") : 0;
  this->sword_key_effect_displayed = keys_effect->get_sword_key_effect();
  this->img_sword_icon = new Surface("sword_icon.png", Surface::DIR_LANGUAGE);

  // flipping icon
  this->is_flipping = false;
  this->sprite_sword_icon = new Sprite("hud/sword_icon_flip");

  rebuild();
}

/**
 * @brief Destructor.
 */
SwordIcon::~SwordIcon() {
  delete sprite_sword_icon;
  delete img_sword_icon;
}

/**
 * @brief Returns whether the sword icon is visible.
 * @return true if the sword icon is visible.
 */
bool SwordIcon::is_visible() {
  return HudElement::is_visible() && keys_effect->get_sword_key_effect() != KeysEffect::SWORD_KEY_HIDDEN;
}

/**
 * @brief This function is called repeatedly.
 *
 * It checks whether the icon should change.
 */
void SwordIcon::update() {

  HudElement::update();

  bool need_rebuild = false;

  // text or sword shown
  if (!is_flipping) {

    KeysEffect::SwordKeyEffect sword_key_effect = keys_effect->get_sword_key_effect();

    int sword_number = (equipment != NULL) ? equipment->get_ability("sword") : 0;

    if (sword_key_effect_displayed != sword_key_effect
	|| (sword_key_effect_displayed == KeysEffect::SWORD_KEY_SWORD
	    && sword_number_displayed != sword_number)) {

      sword_key_effect_displayed = sword_key_effect;
      sword_number_displayed = sword_number;

      sprite_sword_icon->start_animation();
      is_flipping = true;
      need_rebuild = true;
    }
  }
  else {
    sprite_sword_icon->update();
    need_rebuild = true;

    if (sprite_sword_icon->is_animation_finished()) {
      is_flipping = false;
    }
  }

  // icon opacity
  if (keys_effect->is_sword_key_enabled() && get_opacity() == 128) {
    set_opacity(255);
  }
  else if (!keys_effect->is_sword_key_enabled() && get_opacity() == 255) {
    set_opacity(128);
  }

  // redraw the surface if something has changed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * @brief Redraws the icon on the surface.
 */
void SwordIcon::rebuild() {

  HudElement::rebuild();
  
  if (!is_flipping) {

    // draw the static image of the icon, with the current
    // sword key effect

    Rectangle icon_position(0, 0, 72, 24);

    if (sword_key_effect_displayed != KeysEffect::SWORD_KEY_SWORD) {
      // draw the sword icon with some text (Save, Return...)
      icon_position.set_y(24 * sword_key_effect_displayed);
    }
    else {
      // draw the sword icon with the current sword
      icon_position.set_y(24 * (sword_key_effect_displayed + sword_number_displayed - 1));
    }

    img_sword_icon->display_region(icon_position, surface_drawn);
  }
  else {
    // draw the flipping sprite
    sprite_sword_icon->display(surface_drawn, 24, 0);
  }
}

