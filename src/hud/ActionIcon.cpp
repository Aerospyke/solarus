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
#include "hud/ActionIcon.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Sprite.h"

/**
 * Constructor.
 * @param keys_effect current effect of the main keys
 * (here we are interested in the action key)
 * @param x x coordinate of the top-left corner of the icon on the destination surface
 * @param y y coordinate of the top-left corner of the icon on the destination surface
 */
ActionIcon::ActionIcon(KeysEffect *keys_effect, int x, int y):
  HudElement(x, y, 72, 24), keys_effect(keys_effect) {

  // static icon
  this->action_key_effect_displayed = keys_effect->get_action_key_effect();
  this->img_action_icon = ResourceManager::load_image("hud/action_icon.png");

  this->is_enabled = true;

  // flipping icon
  this->is_flipping = false;
  this->sprite_action_icon = new Sprite("hud/action_icon_flip");

  rebuild();
}

/**
 * Destructor.
 */
ActionIcon::~ActionIcon(void) {
  delete sprite_action_icon;
  SDL_FreeSurface(img_action_icon);
}

/**
 * Returns whether the action icon is visible.
 * Unlike the other icons, the action icon is hidden when
 * the key effect is KeysEffect::ACTION_KEY_NONE.
 * @return true if the action key effect is not KeysEffect::ACTION_KEY_NONE
 */
bool ActionIcon::is_visible(void) {
  return action_key_effect_displayed != KeysEffect::ACTION_KEY_NONE
    || is_flipping;
}

/**
 * This function is called repeatedly, it checks whether
 * the icon should change.
 */
void ActionIcon::update(void) {

  bool need_rebuild = false;

  // text shown
  if (!is_flipping) {

    KeysEffect::ActionKeyEffect action_key_effect = keys_effect->get_action_key_effect();

    if (action_key_effect_displayed != action_key_effect) {

      // determine the appropriate animation
      std::string animation;
      if (action_key_effect_displayed != KeysEffect::ACTION_KEY_NONE) {

	if (action_key_effect != KeysEffect::ACTION_KEY_NONE) {
	  animation = "flip";
	}
	else {
	  animation = "disappearing";
	}
      }
      else {
	animation = "appearing";
      }
      sprite_action_icon->set_current_animation(animation);

      action_key_effect_displayed = action_key_effect;

      is_flipping = true;
      need_rebuild = true;
    }
  }
  else {
    sprite_action_icon->update();
    need_rebuild = true;

    if (sprite_action_icon->is_animation_finished()) {
      is_flipping = false;
    }
  }

  // icon opacity
  if (keys_effect->is_action_key_enabled() && !is_enabled) {
    SDL_SetAlpha(surface_drawn, SDL_SRCALPHA, 255);
    is_enabled = true;
    need_rebuild = true;
  }
  else if (!keys_effect->is_action_key_enabled() && is_enabled) {
    SDL_SetAlpha(surface_drawn, SDL_SRCALPHA, 128);
    is_enabled = false;
    need_rebuild = true;
  }

  // redraw the surface if something has changed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * Redraws the icon on the surface.
 */
void ActionIcon::rebuild(void) {
  
  HudElement::rebuild();
  
  if (is_visible()) {
    
    if (!is_flipping) {
      
      // draw the static image of the icon, with the current
      // action key effect
      
      SDL_Rect icon_position = {0, 0, 72, 24};
    
      // draw the action icon with some text (Action, Throw...)
      icon_position.y = 24 * action_key_effect_displayed;

      SDL_BlitSurface(img_action_icon, &icon_position, surface_drawn, NULL);
    }
    else {
      
      // draw the flipping sprite
      sprite_action_icon->display(surface_drawn, 24, 0);
    }
  }
}
