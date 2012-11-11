/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "hud/ItemIcon.h"
#include "Counter.h"
#include "Equipment.h"
#include "EquipmentItem.h"
#include "Game.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "lowlevel/Surface.h"

/**
 * @brief Name of the image file for each item slot.
 */
const std::string ItemIcon::background_file_names[2] = {
  "hud/item_x_icon.png",
  "hud/item_v_icon.png",
};

/**
 * @brief Creates an item icon.
 * @param game the current game
 * @param slot the slot of this icon (0 for the X icon, 1 for the Y icon)
 * @param x x position of the icon on the screen
 * @param y y position of the icon on the screen
 */
ItemIcon::ItemIcon(Game &game, int slot, int x, int y):
  HudElement(game, x, y, 32, 28) {

  this->slot = slot;
  this->background_img = new Surface(background_file_names[slot]);
  this->item_sprite = new Sprite("entities/items");

  this->item_displayed = "";
  this->item_variant_displayed = 0;
  this->counter = new Counter(2, false, 8, 16);
  this->counter_value_displayed = -1;
  this->counter_maximum_displayed = -1;

  rebuild();
}

/**
 * @brief Destructor.
 */
ItemIcon::~ItemIcon() {

  delete background_img;
  delete item_sprite;  
  delete counter;
}

/**
 * @brief Updates the item image displayed and the counter's value.
 */
void ItemIcon::update() {

  HudElement::update();

  bool need_rebuild = false;

  KeysEffect& keys_effect = game->get_keys_effect();

  // item assigned
  const std::string& current_item = equipment->get_item_assigned(slot);
  if (item_displayed != current_item) {

    need_rebuild = true;
    item_displayed = current_item;
    item_variant_displayed = 0;

    if (current_item.size() > 0) {
      item_sprite->set_current_animation(current_item);
    }
  }

  if (current_item.size() > 0) {
    // variant of the item
    int current_item_variant = equipment->get_item_variant(current_item);
    if (item_variant_displayed != current_item_variant) {

      need_rebuild = true;
      item_variant_displayed = current_item_variant;
      item_sprite->set_current_direction(current_item_variant - 1);
    }

    // counter
    if (equipment->get_item(current_item).has_counter()) {

      int current_counter_value = equipment->get_item_amount(current_item);
      int current_counter_maximum = equipment->get_item_maximum(current_item);

      if (counter_value_displayed != current_counter_value || counter_maximum_displayed != current_counter_maximum) {
        need_rebuild = true;
        counter_maximum_displayed = current_counter_maximum;
        counter_value_displayed = current_counter_value;
        counter->set_maximum(current_counter_maximum);
        counter->set_value(counter_value_displayed);
      }
    }
    else if (counter_value_displayed != -1) {
      need_rebuild = true;
      counter_value_displayed = -1;
    }
  }

  // icon opacity
  if (keys_effect.are_item_keys_enabled() && get_opacity() == 128) {
    set_opacity(255);
  }
  else if (!keys_effect.are_item_keys_enabled() && get_opacity() == 255) {
    set_opacity(128);
  }

  // redraw the icon if needed
  if (need_rebuild) {
    rebuild();
  }
}

/**
 * @brief Redraws the icon on the surface.
 */
void ItemIcon::rebuild() {

  HudElement::rebuild();

  // background image
  background_img->draw(surface_drawn);

  // item
  if (item_displayed.size() > 0) {

    item_sprite->draw(surface_drawn, 12, 17);

    // counter
    if (counter_value_displayed != -1) {
      counter->draw(surface_drawn);
    }
  }
}

