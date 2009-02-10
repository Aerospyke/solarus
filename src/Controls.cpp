#include "Controls.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "menus/PauseMenu.h"
#include "entities/Hero.h"

// TODO: load this from some external file (for future translation)
static const string key_names[] = {
  "Action",
  "Epée",
  "Objet 1",
  "Objet 2",
  "Pause",
  "Droite",
  "Haut",
  "Gauche",
  "Bas",
};

static const string direction_strings[] = {"right", "up", "left", "down"};

/**
 * Constructor.
 * @param game the game
 */
Controls::Controls(Game *game):
  game(game), savegame(game->get_savegame()), customizing(false) {

  if (SDL_NumJoysticks() > 0) {
    joystick = SDL_JoystickOpen(0);
  }
  else {
    joystick = NULL;
  }

  // load the controls from the savegame
  for (int i = 0; i < 9; i++) {

    GameKey game_key = (GameKey) (i + 1);

    // keyboard
    int index = Savegame::KEYBOARD_ACTION_KEY + i;
    SDLKey keyboard_symbol = (SDLKey) savegame->get_integer(index);
    keyboard_mapping[keyboard_symbol] = game_key;

    // joypad
    index = Savegame::JOYPAD_ACTION_KEY + i;
    string joypad_string = savegame->get_string(index);
    joypad_mapping[joypad_string] = game_key;

    // game key
    keys_pressed[i] = false;
  }
}

/**
 * Destructor.
 */
Controls::~Controls(void) {

  if (SDL_JoystickOpened(0)) {
    SDL_JoystickClose(joystick);
  }
}

/**
 * Returns the name of a game key.
 * @param key a game key
 * @return the corresponding name
 */
string Controls::get_key_name(GameKey key) {
  return key_names[key - 1];
}

/**
 * Returns a string representing the keyboard key associated to
 * the specified game key.
 * @param game_key a game key
 * @return a string representing the corresponding keyboard key
 */
string Controls::get_keyboard_string(GameKey game_key) {
  return SDL_GetKeyName(get_keyboard_key(game_key));
}

/**
 * Returns whether the specified game key is pressed.
 * The key can be pressed from the keyboard or the joypad.
 */
bool Controls::is_key_pressed(GameKey game_key) {
  return keys_pressed[game_key - 1];
}

/**
 * This function is called by the game when an SDL event occurs.
 * @param event an SDL event
 */
void Controls::handle_event(const SDL_Event &event) {

  /*
   * If no key is being customized, we look for a binding
   * for this SDL event and we ignore the event if no binding was found.
   * If a key is being customized, we consider instead this event as
   * the new binding for this game key.
   */

  switch (event.type) {

    // a key is pressed
  case SDL_KEYDOWN:
    key_pressed(event.key.keysym);
    break;

    // a key is released
  case SDL_KEYUP:
    key_released(event.key.keysym);
    break;

    // joypad
  case SDL_JOYAXISMOTION:
    joypad_axis_moved(event.jaxis.axis, event.jaxis.value);
    break;

  case SDL_JOYHATMOTION:
    // TODO test the hat handling
    // joypad_hat_moved(event.jhat.hat, event.jhat.value);
    break;

  case SDL_JOYBUTTONDOWN:
    joypad_button_pressed(event.jbutton.button);
    break;

  case SDL_JOYBUTTONUP:
    joypad_button_released(event.jbutton.button);
    break;

  default:
    break;
  }

  /*
   * TODO temporary (test code only)
   */
  if (event.type == SDL_KEYDOWN) {

    Equipment *equipment = game->get_equipment();

    switch (event.key.keysym.unicode) {

    case SDLK_p:
      game->get_equipment()->add_hearts(2);
      break;
	  
    case SDLK_m:
      game->get_equipment()->remove_hearts(1);
      break;
	  
    case SDLK_o:
      game->get_equipment()->add_rupees(23);
      break;
	  
    case SDLK_l:
      game->get_equipment()->remove_rupees(14);
      break;
	  
    case SDLK_i:
      game->get_equipment()->add_magic(10);
      break;
	  
    case SDLK_k:
      game->get_equipment()->remove_magic(4);
      break;
	  
    case SDLK_j:
      if (!game->get_equipment()->is_magic_decreasing()) {
	game->get_equipment()->start_removing_magic(200);
      }
      else {
	game->get_equipment()->stop_removing_magic();
      }
      break;

    case SDLK_t:
      equipment->give_inventory_item(InventoryItem::BOW);
      equipment->set_max_arrows(10);
      equipment->set_item_assigned(0, InventoryItem::BOW);
      equipment->give_inventory_item(InventoryItem::BOTTLE_2, 6);
      equipment->set_item_assigned(1, InventoryItem::BOTTLE_2);
      equipment->give_inventory_item(InventoryItem::BOMBS);
      equipment->set_max_bombs(10);
      equipment->set_bombs(10);
      equipment->give_inventory_item(InventoryItem::BOOMERANG);
      equipment->give_inventory_item(InventoryItem::LAMP);
      equipment->give_inventory_item(InventoryItem::HOOK_SHOT);
      equipment->give_inventory_item(InventoryItem::PEGASUS_SHOES);
      equipment->give_inventory_item(InventoryItem::BOTTLE_1);
      equipment->give_inventory_item(InventoryItem::GLOVE);
      equipment->give_inventory_item(InventoryItem::PAINS_AU_CHOCOLAT);
      equipment->set_inventory_item_amount(InventoryItem::PAINS_AU_CHOCOLAT, 3);
      equipment->give_inventory_item(InventoryItem::CROISSANTS);
      equipment->give_inventory_item(InventoryItem::FLIPPERS);
      equipment->give_inventory_item(InventoryItem::RED_KEY);
      equipment->give_inventory_item(InventoryItem::CLAY_KEY);
      break;
 
    case SDLK_g:
      equipment->add_arrows(7);
      break;

    case SDLK_b:
      equipment->remove_arrow();
      break;

    default:
      break;
    }

    switch (event.key.keysym.sym) {
    case SDLK_KP7:
      equipment->set_max_magic(0);
      break;
	  
    case SDLK_KP8:
      equipment->set_max_magic(42);
      break;
	  
    case SDLK_KP9:
      equipment->set_max_magic(84);
      break;
	  
    case SDLK_KP1:
      equipment->set_tunic(MAX(equipment->get_tunic() - 1, 0));
      game->get_hero()->rebuild_equipment();
      break;

    case SDLK_KP4:
      equipment->set_tunic(MIN(equipment->get_tunic() + 1, 2));
      game->get_hero()->rebuild_equipment();
      break;
	  
    case SDLK_KP2:
      equipment->set_sword(MAX(equipment->get_sword() - 1, 0));
      game->get_hero()->rebuild_equipment();
      break;
	  
    case SDLK_KP5:
      equipment->set_sword(MIN(equipment->get_sword() + 1, 4));
      game->get_hero()->rebuild_equipment();
      break;
	  
    case SDLK_KP3:
      equipment->set_shield(MAX(equipment->get_shield() - 1, 0));
      game->get_hero()->rebuild_equipment();
      break;
	  
    case SDLK_KP6:
      equipment->set_shield(MIN(equipment->get_shield() + 1, 3));
      game->get_hero()->rebuild_equipment();
      break;
	  
    default:
      break;
    }
  }
}

/**
 * This function is called when a keyboard key is pressed.
 * @param keysym the key pressed
 */
void Controls::key_pressed(const SDL_keysym &keysym) {

  // retrieve the game key corresponding to this keyboard key
  GameKey game_key = keyboard_mapping[keysym.sym];

  if (!customizing) {

    // if the key is mapped (otherwise we just ignore it)
    if (game_key != 0) {

      // notify the game
      game_key_pressed(game_key);
    }
  }
  else {
    customizing = false;

    if (game_key != key_to_customize) {
      // consider this key as the new mapping for the game key being customized

      SDLKey previous_keyboard_key = get_keyboard_key(key_to_customize);
      if (game_key != 0) {
	// this keyboard key was already assigned to a game key
	keyboard_mapping[previous_keyboard_key] = game_key;
      }
      else {
	keyboard_mapping.erase(previous_keyboard_key);
      }
      keyboard_mapping[keysym.sym] = key_to_customize;
      keys_pressed[key_to_customize - 1] = true;

      // save this new mapping
      int index = Savegame::KEYBOARD_ACTION_KEY + key_to_customize - 1;
      savegame->set_integer(index, keysym.sym);
    }
  }
}

/**
 * This function is called when a keyboard key is released.
 * @param keysym the key released
 */
void Controls::key_released(const SDL_keysym &keysym) {

  // retrieve the game key corresponding to this keyboard key
  GameKey game_key = keyboard_mapping[keysym.sym];

  // if the key is mapped (otherwise we just ignore it)
  if (game_key != 0) {

    // notify the game
    game_key_released(game_key);
  }
}

/**
 * This function is called when a joypad button is pressed.
 * @param button the button pressed
 */
void Controls::joypad_button_pressed(int button) {

  // retrieve the game key corresponding to this joypad button
  std::ostringstream oss;
  oss << "button " << button;
  string joypad_string = oss.str();
  GameKey game_key = joypad_mapping[joypad_string];

  if (!customizing) {

    // if the key is mapped (otherwise we just ignore it)
    if (game_key != 0) {

      // notify the game
      game_key_pressed(game_key);
    }
  }
  else {
    customizing = false;

    if (game_key != key_to_customize) {
      // consider this button as the new mapping for the game key being customized

      string previous_joypad_string = get_joypad_string(key_to_customize);
      if (game_key != 0) {
	// this button was already assigned to a game key
	joypad_mapping[previous_joypad_string] = game_key;
      }
      else {
	joypad_mapping.erase(previous_joypad_string);
      }
      joypad_mapping[joypad_string] = key_to_customize;
      keys_pressed[key_to_customize - 1] = true;

      // save this new mapping
      int index = Savegame::JOYPAD_ACTION_KEY + key_to_customize - 1;
      savegame->set_string(index, joypad_string);
    }
  }
}

/**
 * This function is called when a joypad button is released.
 * @param button the button released
 */
void Controls::joypad_button_released(int button) {

  // retrieve the game key corresponding to this joypad button
  std::ostringstream oss;
  oss << "button " << button;
  GameKey game_key = joypad_mapping[oss.str()];

  // if the key is mapped (otherwise we just ignore it)
  if (game_key != 0) {

    // notify the game
    game_key_released(game_key);
  }
}

/**
 * This function is called when a joypad axis is moved.
 * @param axis the axis moved
 * @param state the new axis state
 */
void Controls::joypad_axis_moved(int axis, int state) {

  // axis in centered position
  if (state == 0) {

    std::ostringstream oss_1;
    oss_1 << "axis " << axis << " +";
    string joypad_string_1 = oss_1.str();
    GameKey game_key_1 = joypad_mapping[oss_1.str()];

    if (game_key_1 != 0) {
      game_key_released(game_key_1);
    }

    std::ostringstream oss_2;
    oss_2 << "axis " << axis << " -";
    string joypad_string_2 = oss_2.str();
    GameKey game_key_2 = joypad_mapping[oss_2.str()];

    if (game_key_2 != 0) {
      game_key_released(game_key_2);
    }
  }
  else if (state > 15000 || state < -15000) {

    std::ostringstream oss;
    oss << "axis " << axis << ((state > 0) ? " +" : " -");
    string joypad_string = oss.str();

    GameKey game_key = joypad_mapping[joypad_string];

    if (!customizing) {

      // if the key is mapped (otherwise we just ignore it)
      if (game_key != 0) {

	// notify the game
	game_key_pressed(game_key);
      }
    }
    else {
      customizing = false;

      if (game_key != key_to_customize) {
	// consider this axis movement as the new mapping for the game key being customized

	string previous_joypad_string = get_joypad_string(key_to_customize);
	if (game_key != 0) {
	  // this axis movement was already assigned to a game key
	  joypad_mapping[previous_joypad_string] = game_key;
	}
	else {
	  joypad_mapping.erase(previous_joypad_string);
	}
	joypad_mapping[joypad_string] = key_to_customize;
	keys_pressed[key_to_customize - 1] = true;

	// save this new mapping
	int index = Savegame::JOYPAD_ACTION_KEY + key_to_customize - 1;
	savegame->set_string(index, joypad_string);
      }
    }
  }
}

/**
 * This function is called when a joypad has is moved.
 * @param hat the hat moved
 * @param value the new hat position
 */
void Controls::joypad_hat_moved(int hat, int value) {

  // hat in centered position
  if (value == SDL_HAT_CENTERED) {

    for (int i = 0; i < 4; i++) {

      std::ostringstream oss;
      oss << "hat " << hat << ' ' << direction_strings[i];
      string joypad_string = oss.str();
      GameKey game_key = joypad_mapping[oss.str()];

      if (game_key != 0) {
	game_key_released(game_key);
      }
    }
  }
  else {

    int direction_1 = -1;
    int direction_2 = -1;

    switch (value) {

    case SDL_HAT_RIGHT:
      direction_1 = 0;
      break;

    case SDL_HAT_RIGHTUP:
      direction_1 = 1;
      direction_2 = 0;
      break;

    case SDL_HAT_UP:
      direction_1 = 1;
      break;

    case SDL_HAT_LEFTUP:
      direction_1 = 1;
      direction_2 = 2;
      break;

    case SDL_HAT_LEFT:
      direction_1 = 2;
      break;

    case SDL_HAT_LEFTDOWN:
      direction_1 = 3;
      direction_2 = 2;
      break;

    case SDL_HAT_DOWN:
      direction_1 = 3;
      break;

    case SDL_HAT_RIGHTDOWN:
      direction_1 = 3;
      direction_2 = 0;
    }

    std::ostringstream oss;
    oss << "hat " << hat << ' ' << direction_strings[direction_1];
    string joypad_string = oss.str();
    GameKey game_key = joypad_mapping[joypad_string];

    string joypad_string_2 = "";
    GameKey game_key_2 = NONE;

    if (direction_2 != -1) {
      std::ostringstream oss;
      oss << "hat " << hat << ' ' << direction_strings[direction_2];
      joypad_string_2 = oss.str();
      game_key_2 = joypad_mapping[joypad_string_2];
    }

    if (!customizing) {

      // if the key is mapped (otherwise we just ignore it)
      if (game_key != 0) {

	// notify the game
	game_key_pressed(game_key);
      }

      if (game_key_2 != 0) {
	game_key_pressed(game_key);
      }
    }
    else {
      customizing = false;

      if (game_key != key_to_customize) {
	// consider this hat movement as the new mapping for the game key being customized

	string previous_joypad_string = get_joypad_string(key_to_customize);
	if (game_key != 0) {
	  // this hat movement was already assigned to a game key
	  joypad_mapping[previous_joypad_string] = game_key;
	}
	else {
	  joypad_mapping.erase(previous_joypad_string);
	}
	joypad_mapping[joypad_string] = key_to_customize;
	keys_pressed[key_to_customize - 1] = true;

	// save this new mapping
	int index = Savegame::JOYPAD_ACTION_KEY + key_to_customize - 1;
	savegame->set_string(index, joypad_string);
      }
    }
  }
}

/**
 * This function is called when a game key is pressed.
 * This event may come from the keyboard or the joypad.
 * @param key the game key pressed
 */
void Controls::game_key_pressed(GameKey key) {

  int index = key - 1;
  if (keys_pressed[index]) {
    return;
  }

  keys_pressed[index] = true;

  if (!game->is_suspended()) {    

    // if the game is not suspended, most of the keys apply to the hero
    if (key == PAUSE) {
      game->set_paused(true);
    }
    else {
      game->get_hero()->key_pressed(key);
    }
  }

  // is a message being shown?
  else if (game->is_showing_message()) {
    game->get_dialog_box()->key_pressed(key);
  }

  // is the game paused?
  else if (game->is_paused()) {
    game->get_pause_menu()->key_pressed(key);
  }

}

/**
 * This function is called when a game key is pressed.
 * This event may come from the keyboard or the joypad.
 * @param key the game key pressed
 */
void Controls::game_key_released(GameKey key) {

  keys_pressed[key - 1] = false;

  if (!game->is_suspended()) {
    // if the game is not suspended, the keys apply to the hero
    game->get_hero()->key_released(key);
  }  
}


/**
 * Returns the keyboard key where the specified game key
 * is currently mapped.
 * @param game_key a game key
 * @return the keyboard key corresponding this game key
 */
SDLKey Controls::get_keyboard_key(GameKey game_key) {

  bool found = false;
  SDLKey keyboard_key = (SDLKey) 0;
  std::map<SDLKey, GameKey>::const_iterator it;
  for (it = keyboard_mapping.begin(); it != keyboard_mapping.end(); it++) {

    if (it->second == game_key) {
      keyboard_key = it->first;
      found = true;
    }
  }

  if (!found) {
    DIE("No keyboard key is defined for game key '" << get_key_name(game_key) << "'");
  }

  return keyboard_key;
}

/**
 * Returns a string representing the joypad action where the
 * specified game key is currently mapped.
 * @return the joypad action corresponding this game key
 */
string Controls::get_joypad_string(GameKey game_key) {

  bool found = false;
  string joypad_string = "";
  std::map<string, GameKey>::const_iterator it;
  for (it = joypad_mapping.begin(); it != joypad_mapping.end(); it++) {

    if (it->second == game_key) {
      joypad_string = it->first;
      found = true;
    }
  }

  if (!found) {
    DIE("No joypad action is defined for game key '" << get_key_name(game_key) << "'");
  }

  return joypad_string;
}

// customization

/**
 * Notifies the control manager that the specified key is going to be customized.
 * After this function is called, the next keyboard or joypad event received will
 * not be treated normally; it will be considered as the new keyboard or joypad
 * binding for this game key. Then, the keyboard and joypad events will be treated
 * normally again. Call is_customization_done() to know when this happens.
 * @param key the game key to customize
 */
void Controls::customize(GameKey key) {
  this->customizing = true;
  this->key_to_customize = key;
}

/**
 * Returns whether the customization process started by calling the customize()
 * function is done, i.e. whether the key has received a new binding
 * and the controls are now treated normally again.
 * @return true if no game key is being customized
 */
bool Controls::is_customization_done(void) {
  return !customizing;
}
