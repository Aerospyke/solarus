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
#include "Equipment.h"
#include "Game.h"
#include "Savegame.h"
#include "DialogBox.h"
#include "Map.h"
#include "Treasure.h"
#include "lowlevel/System.h"

/**
 * @brief Constructor.
 * @param savegame the savegame to encapsulate
 */
Equipment::Equipment(Savegame *savegame):
  savegame(savegame), game(NULL), magic_decrease_delay(0), 
  giving_fairy(false), giving_water(false) {

}

/**
 * @brief Destructor.
 */
Equipment::~Equipment(void) {

}

/**
 * @brief Sets the current game.
 * @param game the game
 */
void Equipment::set_game(Game *game) {

  this->game = game;
}

/**
 * @brief This function is be called repeatedly by the game.
 *
 * Most of the time, there is nothing to update in this class.
 * The only elements updated here are some dynamic things
 * such as equipment-related animations or dialogs.
 */
void Equipment::update(void) {

  if (game == NULL) {
    // nothing dynamic when there is no game
    return;
  }

  // magic bar
  if (magic_decrease_delay != 0) {
    // the magic bar is decreasing

    if (!game->is_suspended()) {

      if (System::now() > next_magic_decrease_date) {

	remove_magic(1);

	if (get_magic() > 0) {
	  next_magic_decrease_date += magic_decrease_delay;
	}
	else {
	  stop_removing_magic();
	}
      }
    }
    else {
      // delay the next decrease while the game is suspended
      next_magic_decrease_date = System::now();
    }
  }

  // fairy
  if (giving_fairy) {

    if (!game->is_showing_message()) {

      giving_fairy = false;
      int answer = game->get_dialog_box()->get_last_answer();

      if (answer != 1) {
	// restore the hearts
	add_hearts(7 * 4);
      }
      else {
	// keep the fairy in a bottle
	if (!has_empty_bottle()) {
	  game->get_dialog_box()->start_dialog("_found_fairy.no_empty_bottle");
	  game->play_sound("wrong");
	  add_hearts(7 * 4);
	}
	else {
	  give_inventory_item(get_first_empty_bottle(), 6);
	  game->play_sound("danger");
	}
      }
    }
  }
  else if (giving_water && !game->is_showing_message()) {

    giving_water = false;
    int answer = game->get_dialog_box()->get_last_answer();

    if (answer == 0) {
      Treasure *treasure = new Treasure(game, Treasure::INVENTORY_ITEM, -1);
      treasure->set_inventory_item(get_first_empty_bottle(), 2);
      game->give_treasure(treasure);
    }
  }
}

// tunic

/**
 * @brief Returns the number of the player's tunic.
 * @return the player's tunic number (0 to 2)
 */
int Equipment::get_tunic(void) {
  return savegame->get_integer(Savegame::HERO_TUNIC);
}

/**
 * @brief Sets the player's tunic.
 * @param tunic the player's tunic number (0 to 2)
 */
void Equipment::set_tunic(int tunic) {

  if (tunic != get_tunic()) {
    // the tunic has changed

    if (tunic < 0 || tunic > 2) {
      DIE("Illegal tunic number: " << tunic);
    }

    savegame->set_integer(Savegame::HERO_TUNIC, tunic);
  }
}

// sword

/**
 * @brief Returns whether the player has a sword.
 * @return true if the player has a sword, i.e. if get_sword_number() > 0
 */
bool Equipment::has_sword(void) {
  return get_sword() > 0;
}

/**
 * @brief Returns the number of the player's sword.
 * @return the player's sword number (0: no sword,
 * 1 to 4: sword 1 to 4)
 */
int Equipment::get_sword(void) {
  return savegame->get_integer(Savegame::HERO_SWORD);
}

/**
 * @brief Sets the player's sword.
 * @param sword the player's sword number (0: no sword,
 * 1 to 4: sword 1 to 4)
 */
void Equipment::set_sword(int sword) {

  if (sword != get_sword()) {
    // the sword has changed

    if (sword < 0 || sword > 4) {
      DIE("Illegal sword number: " << sword);
    }

    savegame->set_integer(Savegame::HERO_SWORD, sword);
  }
}

// shield

/**
 * @brief Returns whether the player has a shield.
 * @return true if the player has a shield, i.e. if get_shield_number() > 0
 */
bool Equipment::has_shield(void) {
  return get_shield() > 0;
}

/**
 * @brief Returns the number of the player's shield.
 * @return the player's shield number (0: no shield,
 * 1 to 3: shield 1 to 3)
 */
int Equipment::get_shield(void) {
  return savegame->get_integer(Savegame::HERO_SHIELD);
}

/**
 * @brief Sets the player's shield.
 * @param shield the player's shield number (0: no shield,
 * 1 to 3: shield 1 to 3)
 */
void Equipment::set_shield(int shield) {

  if (shield != get_shield()) {
    // the shield has changed

    if (shield < 0 || shield > 3) {
      DIE("Illegal shield number: " << shield);
    }

    savegame->set_integer(Savegame::HERO_SHIELD, shield);
  }
}

// rupees

/**
 * @brief Returns the maximum number of rupees of the player.
 * @return the player's maximum number of rupees (100, 300 or 999)
 */
int Equipment::get_max_rupees(void) {
  return savegame->get_integer(Savegame::MAX_RUPEES);
}

/**
 * @brief Sets the maximum number of rupees of the player.
 * @param max_rupees the player's maximum number of rupees (100, 300 or 999)
 */
void Equipment::set_max_rupees(int max_rupees) {

  if (max_rupees != 100 && max_rupees != 300 && max_rupees != 999) {
    DIE("Illegal maximum number of rupees: " << max_rupees);
  }

  savegame->set_integer(Savegame::MAX_RUPEES, max_rupees);
}

/**
 * @brief Returns the player's current number of rupees.
 * @return the player's current number of rupees
 */
int Equipment::get_rupees(void) {
  return savegame->get_integer(Savegame::CURRENT_RUPEES);
}

/**
 * @brief Sets the player's current number of rupees.
 *
 * Exits with an error message if the given number of rupees is not valid.
 *
 * @param rupees the player's new number of rupees
 */
void Equipment::set_rupees(int rupees) {

  if (rupees < 0 || rupees > get_max_rupees()) {
    DIE("Illegal number of rupees: " << rupees);
  }

  savegame->set_integer(Savegame::CURRENT_RUPEES, rupees);
}

/**
 * @brief Adds some rupees to the player.
 *
 * If the maximum number of rupees is reached, no more rupees are added.
 * 
 * @param rupees_to_add number of rupees to add
 */
void Equipment::add_rupees(int rupees_to_add) {

  int max_rupees = get_max_rupees();
  int total = get_rupees() + rupees_to_add;

  set_rupees(std::min(total, max_rupees));
}

/**
 * @brief Removes some rupees from the player.
 *
 * If the number of rupees reaches zero, no more rupees are removed.
 *
 * @param rupees_to_remove number of rupees to remove
 */
void Equipment::remove_rupees(int rupees_to_remove) {

  int total = get_rupees() - rupees_to_remove;

  set_rupees(std::max(total, 0));
}

// hearts

/**
 * @brief Returns the maximum number of hearts of the player.
 * @return the player's maximum number of rupees
 */
int Equipment::get_max_hearts(void) {
  return savegame->get_integer(Savegame::MAX_HEARTS);
}

/**
 * @brief Sets the maximum number of hearts of the player.
 *
 * The program exits with an error message if the given maximum
 * number of hearts is not valid.
 * 
 * @param max_hearts the player's maximum number of hearts
 */
void Equipment::set_max_hearts(int max_hearts) {

  if (max_hearts <= 0 || max_hearts > 20) {
    DIE("Illegal maximum number of hearts: " << max_hearts);
  }

  savegame->set_integer(Savegame::MAX_HEARTS, max_hearts);
}

/**
 * @brief Adds a heart container to the player.
 */
void Equipment::add_heart_container(void) {
  set_max_hearts(get_max_hearts() + 1);
  restore_all_hearts();
}

/**
 * @brief Returns the current number of hearts of the player.
 *
 * The value returned is actually the number of heart quarters,
 * so a value of 4 means that the player has 1 heart left.
 * 
 * @return the player's current number of hearts (in heart quarters)
 */
int Equipment::get_hearts(void) {
  return savegame->get_integer(Savegame::CURRENT_HEARTS);
}

/**
 * @brief Sets the current number of hearts of the player.
 *
 * The given value is actually the number of heart quarters,
 * so a value of 4 means that the player has 1 heart left.
 * The program exits with an error message if the given number
 * of hearts is not valid.
 *
 * @param hearts the player's new number of hearts (in heart quarters)
 */
void Equipment::set_hearts(int hearts) {

  if (hearts < 0 || hearts > get_max_hearts() * 4) {
    DIE("Illegal number of hearts: " << hearts);
  }

  savegame->set_integer(Savegame::CURRENT_HEARTS, hearts);
}

/**
 * @brief Gives some hearts to the player.
 *
 * If the maximum number of hearts is reached, no more hearts are added.
 *
 * @param hearts_to_add number of hearts to add (in heart quarters)
 */
void Equipment::add_hearts(int hearts_to_add) {

  int max_hearts = get_max_hearts() * 4;
  int total = get_hearts() + hearts_to_add;

  set_hearts(std::min(total, max_hearts));
}

/**
 * @brief Removes some hearts from the player.
 *
 * If the number of hearts reaches zero, the game over sequence is started.
 *
 * @param hearts_to_remove number of hearts to remove
 */
void Equipment::remove_hearts(int hearts_to_remove) {

  int total = get_hearts() - hearts_to_remove;

  set_hearts(std::max(total, 0));
}

/**
 * @brief Restores all the hearts.
 */
void Equipment::restore_all_hearts(void) {
  set_hearts(get_max_hearts() * 4);
}

/**
 * @brief Returns whether the player is running out of hearts.
 *
 * The function returns true if the number of hearts is
 * greater than zero and lower
 * than or equal to 25% of the maximum.
 *
 * @return true if the player is running out of hearts
 */
bool Equipment::needs_hearts(void) {
  return get_hearts() > 0 && get_hearts() <= get_max_hearts();
}

/**
 * @brief Returns the current number of pieces of heart of the player.
 * @returns the player's current number of pieces of heart, between 0 and 3
 */
int Equipment::get_nb_pieces_of_heart(void) {
  return savegame->get_integer(Savegame::PIECES_OF_HEART);
}

/**
 * @brief Adds a piece of heart to the player.
 */
void Equipment::add_piece_of_heart(void) {

  // note: the piece_of_heart sound and the message are handled
  // by the Treasure class

  // check whether the player has a new heart

  int nb_pieces_of_heart = get_nb_pieces_of_heart() + 1;
  if (nb_pieces_of_heart < 4) {
    // no new heart
    savegame->set_integer(Savegame::PIECES_OF_HEART, nb_pieces_of_heart);
  }
  else {
    // new heart container
    savegame->set_integer(Savegame::PIECES_OF_HEART, 0);
    add_heart_container();
  }

  restore_all_hearts();
}

/**
 * @brief Gives a fairy to the player.
 *
 * Shows the dialog box and does the action
 * chosen by the player: restore his hearts or keep the fairy in a bottle.
 */
void Equipment::found_fairy(void) {

  if (has_bottle()) {
    game->get_dialog_box()->start_dialog("_found_fairy");
    giving_fairy = true;
    // the next messages will be handled by the update() function
  }
  else {
    add_hearts(7 * 4);
  }
}

/**
 * @brief Gives some water to the player.
 *
 * Shows the dialog box and give the water
 * to the player if he wants and if this is possible.
 * The water will fill the first empty bottle.
 */
void Equipment::found_water(void) {

  if (has_bottle()) {
    if (has_empty_bottle()) {
      found_water(get_first_empty_bottle());
    }
    else {
      game->get_dialog_box()->start_dialog("_found_water.no_empty_bottle");
    }
  }
  else {
    game->get_dialog_box()->start_dialog("_found_water.no_bottle");
  }  
}

/**
 * @brief Gives some water to the player.
 *
 * Shows the dialog box and gives the water
 * to the player if he wants to and if this is possible.
 *
 * @param bottle_index index of the bottle to fill if the player wants to
 */
void Equipment::found_water(int bottle_index) {

  if (get_inventory_item_function(bottle_index) != "bottle") {
    DIE("The inventory item #" << bottle_index << " is not a bottle");
  }
  else if (!has_inventory_item(bottle_index)) {
    DIE("Cannot fill the specified bottle (#" << bottle_index << ") because the player does not have it");
  }

  this->destination_bottle_index = bottle_index;

  game->get_dialog_box()->start_dialog("_found_water");
  giving_water = true;
  // the next messages will be handled by the update() function
}

// magic

/**
 * @brief Returns the maximum level of the player's magic bar.
 * @return the maximum level of magic (0, 42 or 84 points)
 */
int Equipment::get_max_magic(void) {
  return savegame->get_integer(Savegame::MAX_MAGIC);
}

/**
 * @brief Sets the maximum level of the player's magic bar.
 *
 * Exits with an error message if the value specified
 * if not valid.
 * 
 * @param max_magic the maximum level of magic (0, 42 or 84 points)
 */
void Equipment::set_max_magic(int max_magic) {

  if (max_magic != 0 && max_magic != 42 && max_magic != 84) {
    DIE("Illegal maximum number of magic points: " << max_magic);
  }

  savegame->set_integer(Savegame::MAX_MAGIC, max_magic);

  restore_all_magic();
}

/**
 * @brief Returns the current number of magic points of the player.
 * @return the player's current number of magic points (0 to 28)
 */
int Equipment::get_magic(void) {
  return savegame->get_integer(Savegame::CURRENT_MAGIC);
}

/**
 * @brief Sets the current number of magic points of the player.
 *
 * The program exits with an error message if the given value
 * is not valid.
 * 
 * @param magic the player's new number of magic points (0 to 84)
 */
void Equipment::set_magic(int magic) {

  if (magic < 0 || magic > get_max_magic()) {
    DIE("Illegal number of magic points: " << magic);
  }

  savegame->set_integer(Savegame::CURRENT_MAGIC, magic);
}

/**
 * @brief Adds some magic points to the player.
 *
 * If the maximum value is reached, no more magic points are added.
 * 
 * @param magic_to_add number of magic points to add
 */
void Equipment::add_magic(int magic_to_add) {

  int max_magic = get_max_magic();
  int total = get_magic() + magic_to_add;

  set_magic(std::min(total, max_magic));
}

/**
 * @brief Removes some magic points from the player.
 *
 * If the number of magic points reaches zero, no more magic points
 * are removed.
 *
 * @param magic_to_remove number of magic poits to remove
 */
void Equipment::remove_magic(int magic_to_remove) {

  int total = get_magic() - magic_to_remove;

  set_magic(std::max(total, 0));
}

/**
 * @brief Restores all magic points.
 */
void Equipment::restore_all_magic(void) {
  set_magic(get_max_magic());
}

/**
 * @brief Returns whether the player needs magic.
 *
 * The function returns true if the player has a magic bar
 * and has 0 magic points left.
 * 
 * @return true if the player needs magic
 */
bool Equipment::needs_magic(void) {
  return get_magic() == 0 && get_max_magic() != 0;
}

/**
 * @brief Returns whether the magic bar is decreasing continuously.
 * @return true if the magic bar is decreasing, false otherwise
 */
bool Equipment::is_magic_decreasing(void) {
  return this->magic_decrease_delay != 0;
}

/**
 * @brief Starts removing magic continuously.
 * @param delay delay in miliseconds between two decreases
 */
void Equipment::start_removing_magic(uint32_t delay) {

  if (delay <= 0) {
    DIE("Illegal magic bar decrease delay: " << delay);
  }

  if (get_magic() > 0) {
    this->magic_decrease_delay = delay;
    this->next_magic_decrease_date = System::now();

    // the magic points will be removed by the update() function
  }
}

/**
 * S@brief tops removing magic continuously.
 */
void Equipment::stop_removing_magic(void) {
  this->magic_decrease_delay = 0;
}

// bombs

/**
 * @brief Returns the maximum number of bombs of the player.
 * @return the player's maximum number of bombs (0, 10, 30 or 99)
 */
int Equipment::get_max_bombs(void) {
  return savegame->get_integer(Savegame::MAX_BOMBS);
}

/**
 * @brief Sets the maximum number of bombs of the player.
 * @param max_bombs the player's maximum number of bombs (0, 10, 30 or 99)
 */
void Equipment::set_max_bombs(int max_bombs) {

  if (max_bombs != 0 && max_bombs != 10 && max_bombs != 30 && max_bombs != 99) {
    DIE("Illegal maximum number of bombs: " << max_bombs);
  }

  savegame->set_integer(Savegame::MAX_BOMBS, max_bombs);
}

/**
 * @brief Returns the current number of bombs.
 * @return the player's current number of bombs
 */
int Equipment::get_bombs(void) {
  return savegame->get_integer(Savegame::CURRENT_BOMBS);
}

/**
 * @brief Sets the current number of bombs of the player.
 *
 * The program exits with an error message if the given number
 * of bombs is not valid.
 * 
 * @param bombs the player's new number of bombs
 */
void Equipment::set_bombs(int bombs) {

  if (bombs < 0 || bombs > get_max_bombs()) {
    DIE("Illegal number of bombs: " << bombs);
  }

  savegame->set_integer(Savegame::CURRENT_BOMBS, bombs);
}

/**
 * @brief Adds some bombs to the player.
 *
 * If the maximum number of bombs is reached, no more bombs are added.
 * 
 * @param bombs_to_add number of bombs to add
 */
void Equipment::add_bombs(int bombs_to_add) {

  int max_bombs = get_max_bombs();
  int total = get_bombs() + bombs_to_add;

  set_bombs(std::min(total, max_bombs));
}

/**
 * @brief Removes a bomb from the player.
 *
 * If the number of bombs is already zero, nothing happens.
 */
void Equipment::remove_bomb(void) {

  int total = get_bombs() - 1;

  set_bombs(std::max(total, 0));
}

/**
 * @brief Returns whether the player needs bombs.
 *
 * The function returns true if the player has obtained the bombs
 * but has 0 bombs left.
 * 
 * @return true if the player needs bombs
 */
bool Equipment::needs_bombs(void) {
  return has_inventory_item("bombs")
    && get_bombs() == 0;
}

/**
 * @brief Returns the maximum number of arrows of the player.
 * @return the player's maximum number of arrows (0, 10, 30 or 99)
 */
int Equipment::get_max_arrows(void) {
  return savegame->get_integer(Savegame::MAX_ARROWS);
}

/**
 * @brief Sets the maximum number of arrows of the player.
 * @param max_arrows the player's maximum number of arrows (0, 10, 30 or 99)
 */
void Equipment::set_max_arrows(int max_arrows) {

  if (max_arrows != 0 && max_arrows != 10 && max_arrows != 30 && max_arrows != 99) {
    DIE("Illegal maximum number of arrows: " << max_arrows);
  }

  savegame->set_integer(Savegame::MAX_ARROWS, max_arrows);
}

/**
 * @brief Returns the current number of arrows.
 * @return the player's current number of arrows
 */
int Equipment::get_arrows(void) {
  return savegame->get_integer(Savegame::CURRENT_ARROWS);
}

/**
 * @brief Sets the current number of arrows of the player.
 *
 * The program exits with an error message if the given number
 * of arrows is not valid.
 * 
 * @param arrows the player's new number of arrows
 */
void Equipment::set_arrows(int arrows) {

  if (arrows < 0 || arrows > get_max_arrows()) {
    DIE("Illegal number of arrows: " << arrows);
  }

  savegame->set_integer(Savegame::CURRENT_ARROWS, arrows);

  if (has_inventory_item("bow")) {

    // the bow item changes depending on whether the player has arrows
    if (arrows == 0) {
      set_inventory_item_possession_state("bow", 1); // bow without arrows
    }
    else {
      set_inventory_item_possession_state("bow", 2); // bow with arrows
    }
  }
}

/**
 * @brief Adds some arrows to the player.
 *
 * If the maximum number of arrows is reached, no more arrows are added.
 * 
 * @param arrows_to_add number of arrows to add
 */
void Equipment::add_arrows(int arrows_to_add) {

  int max_arrows = get_max_arrows();
  int total = get_arrows() + arrows_to_add;

  set_arrows(std::min(total, max_arrows));
}

/**
 * @brief Removes an arrow from the player.
 *
 * If the number of arrows is already zero, nothing happens.
 */
void Equipment::remove_arrow(void) {

  int total = get_arrows() - 1;

  set_arrows(std::max(total, 0));
}

/**
 * @brief Returns whether the player needs arrows.
 *
 * The function returns true if the player has obtained the bow
 * but has 0 arrows left.
 * 
 * @return true if the player needs arrows
 */
bool Equipment::needs_arrows(void) {
  return has_inventory_item("bow")
    && get_arrows() == 0;
}

// inventory items

/**
 * @brief Returns whether the player has obtained the specified inventory item.
 *
 * This is equivalent to get_inventory_item_possession_state(item_index) > 0.
 *
 * @param item_index index of the item in the inventory
 * @return true if the player has at least the first variant of this item
 */
bool Equipment::has_inventory_item(int item_index) {
  return get_inventory_item_possession_state(item_index) > 0;
}


/**
 * @brief Returns what variant of the specified inventory item
 * the player has.
 *
 * For most items, the value returned is always 0 or 1.
 * Some items have several variants (for example the glove or a bottle):
 * for such items, the value returned may be greater than 1.
 *
 * @param item_index index of the item in the inventory
 * @return a value indicating the possession state of this item
 */
int Equipment::get_inventory_item_possession_state(int item_index) {

  int savegame_variable = Savegame::FIRST_INVENTORY_ITEM + item_index;
  return savegame->get_integer(savegame_variable);
}

/**
 * @brief Gives the first variant of an inventory item to the player.
 *
 * This is equivalent to set_inventory_item_possession_state(item_index, 1).
 * 
 * @param item_index index of the item in the inventory
 */
void Equipment::give_inventory_item(int item_index) {
  set_inventory_item_possession_state(item_index, 1);
}

/**
 * @brief Sets the possession state of an item.
 * @param item_index index of the item in the inventory
 * @param variant the variant of the item to give to the player,
 * or zero to remove the item
 */
void Equipment::set_inventory_item_possession_state(int item_index, int possession_state) {

  // set the possession state in the savegame
  int savegame_variable = Savegame::FIRST_INVENTORY_ITEM + item_index;
  savegame->set_integer(savegame_variable, variant);

  // if we are removing the item, unassign it
  if (possession_state == 0) {

    int slot = get_item_slot(item_index);
    if (slot != -1) {
      set_item_assigned(slot, -1); // unassigned the item
    }
  }
}

/**
 * @brief Removes from the player an item of the inventory.
 *
 * This is equivalent to set_inventory_item_possession_state(item_index, 0).
 *
 * @param item_index index of the item in the inventory
 */
void Equipment::remove_inventory_item(int item_index) {
  set_inventory_item_possession_state(item_index, 0);
}

/**
 * @brief Gives an empty bottle to the player.
 *
 * This function calls give_inventory_item() with the item index
 * corresponding to the first bottle that the player
 * doesn't have yet.
 * The number of bottles and their place in the inventory is specified
 * for you quest in the file info.dat.
 * If the player already has all bottles, the program stops on an error message.
 */
void Equipment::add_bottle(void) {

  // let's find a place for a new bottle in the inventory
  bool found = false;
  std::list<int>::iterator it;
  for (it = bottles.begin(); it != bottles.end() && !found; it++) {

    int bottle_index = *it;
    if (!has_inventory_item(bottle_index)) {
      give_inventory_item(bottle_index);
      found = true;
    }
  }
  else {
    DIE("The player already has all bottles");
  }
}

/**
 * @brief Returns whether the player has at least one bottle.
 * @return true if the player has at least one bottle
 */
bool Equipment::has_bottle(void) {

  bool found = false;
  std::list<int>::iterator it;
  for (it = bottles.begin(); it != bottles.end() && !found; it++) {

    found = has_inventory_item(*it);
  }

  return found;
}

/**
 * @brief Returns whether the player has at least one empty bottle.
 * @return true if the player has at least one empty bottle
 */
bool Equipment::has_empty_bottle(void) {
  return has_bottle_with(1);
}

/**
 * @brief Returns the index of the first empty bottle.
 * @return the index of the first empty bottle
 */
int Equipment::get_first_empty_bottle(void) {
  return get_first_bottle_with(1);
}

/**
 * @brief Returns the index of a bottle previously selected.
 * @return the index of a bottle previously selected
 */
int Equipment::get_destination_bottle(void) {
  return destination_bottle_index;
}

/**
 * @brief Returns whether the player has at least one bottle having the specified content.
 * @param content the content seeked (must be a valid bottle content)
 * @return true if the player has at least one bottle with this content
 */
bool Equipment::has_bottle_with(int bottle_content) {
  return get_first_bottle_with(bottle_content) != -1;
}

/**
 * @brief Returns the index of the first bottle having the specified content.
 * @param content the content seeked (must be a valid bottle content)
 * @return the index of the first bottle with this content,
 * or -1 if the player has no bottle with this content
 */
int Equipment::get_first_bottle_with(int bottle_content) {

  int index_found = -1;
  std::list<int>::iterator it;
  for (it = bottles.begin(); it != bottles.end() && !found; it++) {

    int bottle_index = *it;
    if (get_inventory_item_possession_state(bottle_index) == bottle_content) {
      index_found = bottle_index;
    }
  }

  return index_found;
}

/**
 * @brief Sets the content of a bottle.
 * @param bottle_index index of the bottle in the inventory
 * (must be the index of a bottle)
 * @param content content to set (must be a valid bottle content)
 */
void Equipment::set_bottle_content(int bottle_index, int bottle_content) {

  if (get_inventory_item_function(bottle_index) != "bottle") {
    DIE("The inventory item #" << bottle_index << " is not a bottle");
  }
  set_inventory_item_possession_state(bottle_index, bottle_content);
}

/**
 * @brief Sets a bottle empty.
 * @param bottle_index item index of the bottle to make empty in the inventory
 * (must be the index of a bottle)
 */
void Equipment::set_bottle_empty(InventoryItemId bottle_index) {
  set_bottle_content(bottle_index, 1);
}

/**
 * @brief Returns the current amount of a specified inventory item.
 * @param item_index index of the inventory item to get
 * @return the player's current amount of this item
 */
int Equipment::get_inventory_item_amount(int item_index) {

  const std::string &function = get_inventory_item_function(item_index);

  if (function == "bombs") {
    return get_bombs();
  }

  if (function == "bow") {
    return get_arrows();
  }

  int counter_savegame_variable = get_inventory_item_counter_savegame_variable(item_index);

  return savegame->get_integer(counter_savegame_variable);
}

/**
 * @brief Sets the current amount of a specified item.
 *
 * The program exits with an error message if the given number is not valid.
 *
 * @param item_index index of the inventory item to set
 * @param amount the new amount
 */
void Equipment::set_inventory_item_amount(int item_index, int amount) {

  const std::string &function = get_inventory_item_function(item_index);

  if (function == "bombs") {
    set_bombs(amount);
  }
  else if (function == "bow") {
    set_arrows(amount);
  }
  else {

    int counter_savegame_variable = get_inventory_item_counter_savegame_variable(item_index);
    int max = get_inventory_item_maximum(item_index);

    if (amount < 0 || amount > max) {
      DIE("Illegal amount for item " << item_index << ": " << amount);
    }

    savegame->set_integer(counter_savegame_variable, amount);
  }
}

/**
 * @brief Adds an amount of a specified inventory item to the player.
 *
 * If the maximum amount is reached, no more items are added.
 * 
 * @param item_index index of the inventory item to set
 * @param amount_to_add the amount to add
 */
void Equipment::add_inventory_item_amount(int item_index, int amount_to_add) {

  const std::string &function = get_inventory_item_function(item_index);

  if (function == "bombs") {
    add_bombs(amount_to_add);
  }
  else if (function == "bow") {
    add_arrows(amount_to_add);
  }
  else {
    int total = get_inventory_item_amount(item_index) + amount_to_add;
    int max = get_inventory_item_maximum(item_index);

    set_inventory_item_amount(item_index, std::min(total, max));
  }
}

/**
 * @brief Removes an amount of a specified inventory item to the player.
 *
 * If the amount reaches zero, no more items are removed.
 * 
 * @param item_index index of the inventory item to set
 * @param amount_to_remove the amount to remove
 */
void Equipment::remove_inventory_item_amount(int item_index, int amount_to_remove) {

  int total = get_inventory_item_amount(item_index) - amount_to_remove;
  set_inventory_item_amount(item_index, std::max(total, 0));
}

/**
 * @brief Returns the maximum amount value of the specified item.
 * @param item_index index of the inventory item to get (must be an item with an amount)
 * @return the maximum amount value of this item
 */
int Equipment::get_inventory_item_maximum(int index) {

  int maximum;
  const std::string &function = get_inventory_item_function(item_index);

  if (function == "bombs") {
    maximum = get_max_bombs();
  }
  else if (function == "arrows") {
    maximum = get_max_arrows();
  }
  else {
    maximum = inventory_item_properties[item_index].maximum_amount;
  }

  if (maximum == 0) {
    DIE("The inventory item '" << item_id << "' has no amount");
  }

  return maximum;
}

/**
 * @brief Returns whether the player has reached the maximum amount of the specified item.
 * @param item_index index of the inventory item to get (must be an item with an amount)
 * @return true if the player has the maximum amount of this item
 */
bool Equipment::has_inventory_item_maximum(int item_index) {
  return get_inventory_item_amount(item_index) >= get_inventory_item_maximum(item_index);
}

/**
 * @brief Returns the index of the inventory item currently assigned to a slot.
 * @param slot slot of the item to get (0 for X or 1 for V)
 * @return the index of the inventory item currently assigned to this slot,
 * or -1 if there is no item in this slot
 */
int Equipment::get_item_assigned(int slot) {

  int savegame_variable = Savegame::ITEM_SLOT_0 + slot;
  return savegame->get_integer(index);
}

/**
 * @brief Assigns an inventory item to a slot.
 *
 * The program exits with an error message if the specified item
 * cannot be equiped or if the player does not have it.
 * 
 * @param slot the slot to set (0 for X or 1 for V)
 * @param item_index index of the item to assign to this slot,
 * or -1 to assign no item
 */
void Equipment::set_item_assigned(int slot, int item_index) {

  if (item_index != -1) {
    if (!has_inventory_item(item_index)) {
      DIE("Cannot assign item " << item_index << " because the player does not have it");
    }

    if (!can_inventory_item_be_assigned(item_index)) {
      DIE("Cannot assign item " << item_index << " because it is not attributable");
    }
  }

  int savegame_variable = Savegame::ITEM_SLOT_0 + slot;
  savegame->set_integer(savegame_variable, item_index);
}

/**
 * @brief Returns the slot (0 or 1) where the specified inventory item is currently assigned.
 * @param item_index index an inventory item
 * @return the slot where this item is, or -1 if this item is not assigned
 */
int Equipment::get_slot(int item_index) {
  
  if (get_item_assigned(0) == item_index) {
    return 0;
  }

  if (get_item_assigned(1) == item_index) {
    return 1;
  }

  return -1;
}

/**
 * @brief Returns whether the player has got the world map.
 * @return true if the player has got the world map
 */
bool Equipment::has_world_map(void) {
  return savegame->get_integer(Savegame::WORLD_MAP) != 0;
}

/**
 * @brief Gives the world map to the player.
 */
void Equipment::add_world_map(void) {
  savegame->set_integer(Savegame::WORLD_MAP, 1);
}

// small keys

/**
 * @brief Returns whether the small keys are enabled in the current map.
 * @return true if the small keys are enabled in the current map
 */
bool Equipment::are_small_keys_enabled(void) {
  return game->get_current_map()->has_small_keys();
}

/**
 * @brief Returns the savegame variable where the number of small keys
 * is stored for the current map.
 *
 * Stops with an error message if the small keys are not enabled in the current map.
 */
int Equipment::get_small_keys_variable(void) {

  if (!are_small_keys_enabled()) {
    DIE("The small keys are not enabled on this map");
  }

  return game->get_current_map()->get_small_keys_variable();
}

/**
 * @brief Returns whether the player has got at least one small key in the current map.
 *
 * Stops with an error message if the small keys are not enabled in the current map.
 * 
 * @return true if the player has got at least one small key
 */
bool Equipment::has_small_key(void) {
  return get_small_keys() > 0;
}

/**
 * @brief Returns the current number of small keys of the player in the current map.
 *
 * Stops with an error message if the small keys are not enabled in the current map.
 * 
 * @return the current number of small keys
 */
int Equipment::get_small_keys(void) {

  int index = get_small_keys_variable();
  return savegame->get_integer(index);
}

/**
 * @brief Adds a small key to the player in the current map.
 *
 * Stops with an error message if the player has no more small keys.
 */
void Equipment::add_small_key(void) {

  int index = get_small_keys_variable();
  savegame->set_integer(index, get_small_keys() + 1);
}

/**
 * @brief Removes a small key from the player in the current dungeon.
 *
 * Stops with an error message if the player has no more small keys.
 */
void Equipment::remove_small_key(void) {

  if (!has_small_key()) {
    DIE("The player has no small keys");
  }

  int index = get_small_keys_variable();
  savegame->set_integer(index, get_small_keys() - 1);
}

// ability to lift items

/**
 * @brief Returns whether the player can lift the specified weight.
 *
 * - If the weight is 0, true is always returned.
 * - If the weight is 1, the function returns true if the player has the first variant of the glove.
 * - If the weight is 2, the function returns true if the player has the second variant of the glove.
 *
 * @param weight the weight of an item (0 to 2)
 * @return true if the player can lift this weight.
 */
bool Equipment::can_lift(int weight) {

  int item_index = get_inventory_item_index("glove");
  return get_inventory_item_possession_state(item_index) >= weight;
}

