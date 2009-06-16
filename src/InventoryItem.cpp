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
#include "InventoryItem.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "ResourceManager.h"
#include "Sound.h"

/**
 * Creates a new inventory item.
 * @param item_id id of the item to create
 */
InventoryItem::InventoryItem(InventoryItemId item_id):
  item_id(item_id) {

}

/**
 * Destructor.
 */
InventoryItem::~InventoryItem(void) {

}

/**
 * Returns whether the specified item can be assigned to icon X or V.
 * @param item_id id of a item
 * @return true if this item item can be assigned to an icon
 */
bool InventoryItem::can_be_assigned(InventoryItemId item_id) {
  return item_id < INVENTORY_ROCK_KEY;
}

/**
 * Returns whether a counter is associated to the specified item.
 * This is equivalent to get_counter_index(item_id) != -1.
 * @param item_id id of an item
 * @return true if this item has a counter
 */
bool InventoryItem::has_counter(InventoryItemId item_id) {
  return get_counter_index(item_id) != -1;
}

/**
 * If the specified item has a counter, returns the index of the savegame
 * variable indicating the counter's value. Otherwise, returns -1.
 * @return the index of the savegame variable indicating the counter's value
 */
int InventoryItem::get_counter_index(InventoryItemId item_id) {

  int counter;

  switch(item_id) {

  case INVENTORY_BOMBS:
    counter = Savegame::CURRENT_BOMBS;
    break;

  case INVENTORY_BOW:
    counter = Savegame::CURRENT_ARROWS;
    break;

  case INVENTORY_APPLES:
    counter = Savegame::CURRENT_APPLES;
    break;

  case INVENTORY_PAINS_AU_CHOCOLAT:
    counter = Savegame::CURRENT_PAINS_AU_CHOCOLAT;
    break;

  case INVENTORY_CROISSANTS:
    counter = Savegame::CURRENT_CROISSANTS;
    break;

  case INVENTORY_FIRE_STONES:
    counter = Savegame::NB_FIRE_STONES;
    break;

  default:
    counter = -1;
    break;
  }

  return counter;
}

/**
 * Starts using this item.
 * @param game the game
 */
void InventoryItem::start(Game *game) {

  this->game = game;
  this->variant = game->get_equipment()->has_inventory_item(item_id);

  if (variant == 0) {
    DIE("Trying to use inventory item #" << item_id << " without having it");
  }

  if (is_bottle()) {
    start_bottle();
  }
}

/**
 * Updates this item when it is being used.
 */
void InventoryItem::update(void) {

}

/**
 * Returns whether this item has finished to be used.
 * @return true if this item has finished to be used
 */
bool InventoryItem::is_finished(void) {
  return true;
}

/**
 * Returns whether this item is a bottle.
 * @return true if this item is a bottle
 */
bool InventoryItem::is_bottle(void) {

  return item_id == INVENTORY_BOTTLE_1
    || item_id == INVENTORY_BOTTLE_2
    || item_id == INVENTORY_BOTTLE_3
    || item_id == INVENTORY_BOTTLE_4;
}

/**
 * Starts using this item when it is a bottle.
 */
void InventoryItem::start_bottle(void) {

  switch (variant) {

    // empty
  case 1:
    ResourceManager::get_sound("wrong")->play();
    break;

    // water
  case 2:

    break;

    // red potion
  case 3:
    // TODO
    break;

    // green potion
  case 4:
    // TODO
    break;

    // red potion
  case 5:
    // TODO
    break;

    // fairy
  case 6:
    // TODO
    break;

  }
}
