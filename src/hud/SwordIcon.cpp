#include "hud/SwordIcon.h"
#include "Equipment.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "ResourceManager.h"

/**
 * Constructor.
 * @param keys_effect current effect of the main keys
 * (here we are interested in the sword key)
 * @param equipment the player's equipment (can be NULL, in the selection menu)
 * @param x x coordinate of the top-left corner of the icon on the destination surface
 * @param y y coordinate of the top-left corner of the icon on the destination surface
 */
SwordIcon::SwordIcon(KeysEffect *keys_effect, Equipment *equipment, int x, int y):
  HudElement(x, y, 72, 24),
  keys_effect(keys_effect), equipment(equipment) {

  // static icon
  this->sword_number_displayed = (equipment != NULL) ? equipment->get_sword() : 0;
  this->sword_key_effect_displayed = keys_effect->get_sword_key_effect();
  this->img_sword_icon = ResourceManager::load_image("hud/sword_icon.png");

  this->is_enabled = true;

  // flipping icon
  this->is_flipping = false;
  this->sprite_sword_icon = new Sprite("hud/sword_icon_flip");

  rebuild();
}

/**
 * Destructor.
 */
SwordIcon::~SwordIcon(void) {
  delete sprite_sword_icon;
  SDL_FreeSurface(img_sword_icon);
}

/**
 * Returns whether the sword icon is visible.
 * @return true if the sword icon is visible.
 */
bool SwordIcon::is_visible(void) {
  return HudElement::is_visible() && keys_effect->get_sword_key_effect() != KeysEffect::SWORD_KEY_HIDDEN;
}

/**
 * This function is called repeatedly, it checks whether
 * the icon should change.
 */
void SwordIcon::update(void) {

  bool need_rebuild = false;

  // text or sword shown
  if (!is_flipping) {

    KeysEffect::SwordKeyEffect sword_key_effect = keys_effect->get_sword_key_effect();

    int sword_number = (equipment != NULL) ? equipment->get_sword() : 0;

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
    sprite_sword_icon->update_current_frame();
    need_rebuild = true;

    if (sprite_sword_icon->is_over()) {
      is_flipping = false;
    }
  }

  // icon opacity
  if (keys_effect->is_sword_key_enabled() && !is_enabled) {
    SDL_SetAlpha(surface_drawn, SDL_SRCALPHA, 255);
    is_enabled = true;
    need_rebuild = true;
  }
  else if (!keys_effect->is_sword_key_enabled() && is_enabled) {
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
void SwordIcon::rebuild(void) {

  HudElement::rebuild();
  
  if (!is_flipping) {

    // draw the static image of the icon, with the current
    // sword key effect

    SDL_Rect icon_position = {0, 0, 72, 24};

    if (sword_key_effect_displayed != KeysEffect::SWORD_KEY_SWORD) {
      // draw the sword icon with some text (Save, Return...)
      icon_position.y = 24 * sword_key_effect_displayed;
    }
    else {
      // draw the sword icon with the current sword
      icon_position.y = 24 * (sword_key_effect_displayed + sword_number_displayed - 1);
    }

    SDL_BlitSurface(img_sword_icon, &icon_position, surface_drawn, NULL);
  }
  else {
    // draw the flipping sprite
    sprite_sword_icon->display(surface_drawn, 24, 0);
  }
}
