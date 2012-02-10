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
#include "hud/HUD.h"
#include "hud/HeartsView.h"
#include "hud/RupeesCounter.h"
#include "hud/MagicBar.h"
#include "hud/ActionIcon.h"
#include "hud/SwordIcon.h"
#include "hud/PauseIcon.h"
#include "hud/ItemIcon.h"
#include "hud/SmallKeysCounter.h"
#include "hud/FloorView.h"
#include "Game.h"
#include "Controls.h"

/**
 * Constructor.
 */
HUD::HUD(Game *game):
  game(game), nb_elements(0), showing_message(false) {

  Savegame *savegame = game->get_savegame();
  Equipment *equipment = game->get_equipment();
  KeysEffect *keys_effect = game->get_keys_effect();

  elements[nb_elements++] = new HeartsView(equipment, 216, 14);
  elements[nb_elements++] = new RupeesCounter(equipment, 8, 216);
  elements[nb_elements++] = new MagicBar(equipment, 216, 35);
  elements[nb_elements++] = new ItemIcon(0, savegame, 8, 29);
  elements[nb_elements++] = new ItemIcon(1, savegame, 60, 29);
  elements[nb_elements++] = new SwordIcon(keys_effect, equipment, 10, 29);
  elements[nb_elements++] = new PauseIcon(keys_effect, -3, 7);
  elements[nb_elements++] = new ActionIcon(keys_effect, 23, 51);
  elements[nb_elements++] = new SmallKeysCounter(equipment, 284, 218);
  elements[nb_elements++] = new FloorView(game, 5, 70);
}

/**
 * Destructor.
 */
HUD::~HUD(void) {

  for (int i = 0; i < nb_elements; i++) {
    delete elements[i];
  }
}

/**
 * Makes an icon blink if the corresponding game key is being customized.
 */
void HUD::update_blinking(void) {

  // detect whether a key is being customized
  int index = -1;
  Controls *controls = game->get_controls();
  if (controls->is_customizing()) {
    Controls::GameKey key = controls->get_key_to_customize();
    switch (key) {

      case Controls::ACTION:
        index = 7;
	break;

      case Controls::SWORD:
	index = 5;
	break;

      case Controls::PAUSE:
	index = 6;
	break;

      case Controls::ITEM_1:
	index = 3;
	break;

      case Controls::ITEM_2:
	index = 4;
	break;

      default:
	break;
    }
  }

  if (index != -1) {
    if (!elements[index]->is_blinking()) {
      elements[index]->set_blinking(true);    
    }
  }
  for (int i = 3; i <= 7; i++) {
    if (i != index && elements[i]->is_blinking()) {
      elements[i]->set_blinking(false);
    }
  }
}

/**
 * Updates the information displayed by HUD.
 */
void HUD::update(void) {

  // detect when the game is showing a message
  if (game->is_showing_message() && !showing_message) {
      showing_message = true;

      // a message is shown: hide or move the top-left icons
      elements[3]->set_visible(false); // item 0
      elements[4]->set_visible(false); // item 1
      elements[6]->set_visible(false); // pause icon
      elements[5]->set_position(-18, 17); // sword icon
      elements[7]->set_position(-5, 39); // action icon
  }
  else if (!game->is_showing_message()) {

    if (showing_message) {
      showing_message = false;

      // a message is finished: restore the top-left icons
      elements[3]->set_visible(true); // item 0
      elements[4]->set_visible(true); // item 1
      elements[6]->set_visible(true); // pause icon
      elements[5]->set_position(10, 29); // sword icon
      elements[7]->set_position(23, 51); // action icon
    }
    else {

      // if the hero is below the top-left icons, make them semi-transparent
      const Rectangle &hero_xy = game->get_hero_xy();
      if (elements[6]->get_opacity() == 255 && !game->is_suspended()
	  && hero_xy.get_x() < 88 && hero_xy.get_y() < 80) {
	elements[3]->set_opacity(96); // item 0
	elements[4]->set_opacity(96); // item 1
	elements[6]->set_opacity(96); // pause icon
	elements[5]->set_opacity(96); // sword icon
	elements[7]->set_opacity(96); // action icon
      }
      else if (elements[6]->get_opacity() == 96
	       && (hero_xy.get_x() >= 88 || hero_xy.get_y() >= 80 || game->is_suspended())) {
	elements[3]->set_opacity(255); // item 0
	elements[4]->set_opacity(255); // item 1
	elements[6]->set_opacity(255); // pause icon
	elements[5]->set_opacity(255); // sword icon
	elements[7]->set_opacity(255); // action icon
      }
    }
  }

  update_blinking();

  // update each element
  for (int i = 0; i < nb_elements; i++) {
    elements[i]->update();
  }
}

/**
 * Displays the HUD.
 * @param destination the destination surface
 */
void HUD::display(Surface *destination) {

  for (int i = 0; i < nb_elements; i++) {
    elements[i]->display(destination);
  }
}

