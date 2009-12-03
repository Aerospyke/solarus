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
#include "Controls.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "DebugKeys.h"
#include "StringResource.h"
#include "menus/PauseMenu.h"
#include "entities/Hero.h"

const std::string Controls::direction_names[] = {"right", "up", "left", "down"};

const uint16_t Controls::arrows_masks[4] = {
  0x0001,
  0x0002,
  0x0004,
  0x0008
};

/**
 * Associates to each possible combination of arrows
 * an angle in degrees: 0 to 359, or -1 to indicate
 * that the movement is stopped.
 *
 * For example:
 *   uint16_t arrows_pressed = right_mask | up_mask;
 *   int angle = directions[arrows_pressed];
 * Here the angle is 45°.
*/
static const int arrows_angles[] = {
  -1,  // none: stop
  0,   // right
  90,  // up
  45,  // right + up
  180, // left
  -1,  // left + right: stop
  135, // left + up
  -1,  // left + right + up: stop
  270, // down
  315, // down + right
  -1,  // down + up: stop
  -1,  // down + right + up: stop
  225, // down + left
  -1,  // down + left + right: stop
  -1,  // down + left + up: stop
  -1,  // down + left + right + up: stop
};

/**
 * Constructor.
 * @param game the game
 */
Controls::Controls(Game *game):
  game(game), savegame(game->get_savegame()), debug_keys(new DebugKeys(game)), customizing(false) {

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
    const std::string &joypad_string = savegame->get_string(index);
    joypad_mapping[joypad_string] = game_key;

    // game key state
    keys_pressed[i] = false;

    // load the game key name in the current language
    std::ostringstream oss;
    oss << "controls.game_key_" << game_key;
    game_key_names[i] = StringResource::get_string(oss.str());
  }
}

/**
 * Destructor.
 */
Controls::~Controls(void) {

  if (SDL_JoystickOpened(0)) {
    SDL_JoystickClose(joystick);
  }

  delete debug_keys;
}

/**
 * Returns the name of a game key.
 * @param key a game key
 * @return the corresponding name
 */
const std::string& Controls::get_key_name(GameKey key) {
  return game_key_names[key - 1];
}

/**
 * Returns a string representing the keyboard key associated to
 * the specified game key.
 * @param game_key a game key
 * @return a string representing the corresponding keyboard key
 */
const std::string Controls::get_keyboard_string(GameKey game_key) {
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
 * Returns the direction corresponding to the arrow keys
 * currently pressed by the player.
 * @return the arrows direction (0 to 360), or -1
 * if no arrow is pressed
 */
int Controls::get_arrows_direction(void) {

  uint16_t arrows_mask = 0x0000;
  if (is_key_pressed(RIGHT)) {
    arrows_mask |= 0x0001;
  }
  if (is_key_pressed(UP)) {
    arrows_mask |= 0x0002;
  }
  if (is_key_pressed(LEFT)) {
    arrows_mask |= 0x0004;
  }
  if (is_key_pressed(DOWN)) {
    arrows_mask |= 0x0008;
  }
  return arrows_angles[arrows_mask];
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
    joypad_hat_moved(event.jhat.hat, event.jhat.value);
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
}

/**
 * This function is called when a keyboard key is pressed.
 * @param keysym the key pressed
 */
void Controls::key_pressed(const SDL_keysym &keysym) {

#if ZSDX_DEBUG_LEVEL >= 2
  // don't consider the debug keys in release mode
  debug_keys->key_pressed(keysym);
#endif

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
	int index = Savegame::KEYBOARD_ACTION_KEY + game_key - 1;
	savegame->set_integer(index, previous_keyboard_key);
      }
      else {
	keyboard_mapping.erase(previous_keyboard_key);
      }
      keyboard_mapping[keysym.sym] = key_to_customize;
      int index = Savegame::KEYBOARD_ACTION_KEY + key_to_customize - 1;
      savegame->set_integer(index, keysym.sym);

      keys_pressed[key_to_customize - 1] = true;
    }
  }
}

/**
 * This function is called when a keyboard key is released.
 * @param keysym the key released
 */
void Controls::key_released(const SDL_keysym &keysym) {

#if ZSDX_DEBUG_LEVEL >= 2
  debug_keys->key_released(keysym);
#endif

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
  const std::string &joypad_string = oss.str();
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

      const std::string &previous_joypad_string = get_joypad_string(key_to_customize);
      if (game_key != 0) {
	// this button was already assigned to a game key
	joypad_mapping[previous_joypad_string] = game_key;
	int index = Savegame::JOYPAD_ACTION_KEY + game_key - 1;
	savegame->set_string(index, previous_joypad_string);
      }
      else {
	joypad_mapping.erase(previous_joypad_string);
      }
      joypad_mapping[joypad_string] = key_to_customize;
      int index = Savegame::JOYPAD_ACTION_KEY + key_to_customize - 1;
      savegame->set_string(index, joypad_string);

      keys_pressed[key_to_customize - 1] = true;
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
  if (state <= 1000 && state >= -1000) {

    std::ostringstream oss_1;
    oss_1 << "axis " << axis << " +";
    GameKey game_key_1 = joypad_mapping[oss_1.str()];

    if (game_key_1 != 0) {
      game_key_released(game_key_1);
    }

    std::ostringstream oss_2;
    oss_2 << "axis " << axis << " -";
    GameKey game_key_2 = joypad_mapping[oss_2.str()];

    if (game_key_2 != 0) {
      game_key_released(game_key_2);
    }
  }
  else if (state > 15000 || state < -15000) {

    std::ostringstream oss;
    oss << "axis " << axis << ((state > 0) ? " +" : " -");
    const std::string &joypad_string = oss.str();

    std::ostringstream oss2;
    oss2 << "axis " << axis << ((state > 0) ? " -" : " +");
    const std::string &inverse_joypad_string = oss2.str();

    GameKey game_key = joypad_mapping[joypad_string];
    GameKey inverse_game_key = joypad_mapping[inverse_joypad_string];

    if (!customizing) {

      // if the key is mapped (otherwise we just ignore it)
      if (game_key != 0) {

	// notify the game
	if (is_key_pressed(inverse_game_key)) {
	  game_key_released(inverse_game_key);
	}
	game_key_pressed(game_key);
      }
    }
    else {
      customizing = false;

      if (game_key != key_to_customize) {
	// consider this axis movement as the new mapping for the game key being customized

	const std::string &previous_joypad_string = get_joypad_string(key_to_customize);
	if (game_key != 0) {
	  // this axis movement was already assigned to a game key
	  joypad_mapping[previous_joypad_string] = game_key;
	  int index = Savegame::JOYPAD_ACTION_KEY + game_key - 1;
	  savegame->set_string(index, previous_joypad_string);
	}
	else {
	  joypad_mapping.erase(previous_joypad_string);
	}
	joypad_mapping[joypad_string] = key_to_customize;
	int index = Savegame::JOYPAD_ACTION_KEY + key_to_customize - 1;
	savegame->set_string(index, joypad_string);

	keys_pressed[key_to_customize - 1] = true;
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
      oss << "hat " << hat << ' ' << direction_names[i];
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
    oss << "hat " << hat << ' ' << direction_names[direction_1];
    const std::string &joypad_string = oss.str();
    GameKey game_key = joypad_mapping[joypad_string];

    std::ostringstream oss_inv;
    oss_inv << "hat " << hat << ' ' << direction_names[(direction_1 + 2) % 4];
    const std::string &inverse_joypad_string = oss_inv.str();
    GameKey inverse_game_key = joypad_mapping[inverse_joypad_string];

    GameKey game_key_2 = NONE;
    GameKey inverse_game_key_2 = NONE;

    if (direction_2 != -1) {
      std::ostringstream oss;
      oss << "hat " << hat << ' ' << direction_names[direction_2];
      const std::string &joypad_string_2 = oss.str();
      game_key_2 = joypad_mapping[joypad_string_2];

      std::ostringstream oss_inv;
      oss_inv << "hat " << hat << ' ' << direction_names[(direction_2 + 2) % 4];
      const std::string &inverse_joypad_string_2 = oss_inv.str();
      inverse_game_key_2 = joypad_mapping[inverse_joypad_string_2];
    }
    else {
      std::ostringstream oss;
      oss << "hat " << hat << ' ' << direction_names[(direction_1 + 1) % 4];
      const std::string &joypad_string_2 = oss.str();
      game_key_2 = joypad_mapping[joypad_string_2];

      std::ostringstream oss_inv;
      oss_inv << "hat " << hat << ' ' << direction_names[(direction_1 + 3) % 4];
      const std::string &inverse_joypad_string_2 = oss_inv.str();
      inverse_game_key_2 = joypad_mapping[inverse_joypad_string_2];
    }

    if (!customizing) {

      // if the key is mapped (otherwise we just ignore it)
      if (game_key != 0) {

	// notify the game
	if (is_key_pressed(inverse_game_key)) {
	  game_key_released(inverse_game_key);
	}
	game_key_pressed(game_key);
      }

      if (direction_2 != -1) {
	if (is_key_pressed(inverse_game_key_2)) {
	  game_key_released(inverse_game_key_2);
	}
	game_key_pressed(game_key_2);
      }
      else {
	if (is_key_pressed(game_key_2)) {
	  game_key_released(game_key_2);
	}
	if (is_key_pressed(inverse_game_key_2)) {
	  game_key_released(inverse_game_key_2);
	}
      }
    }
    else {
      customizing = false;

      if (game_key != key_to_customize) {
	// consider this hat movement as the new mapping for the game key being customized

	const std::string &previous_joypad_string = get_joypad_string(key_to_customize);
	if (game_key != 0) {
	  // this hat movement was already assigned to a game key
	  joypad_mapping[previous_joypad_string] = game_key;
	  int index = Savegame::JOYPAD_ACTION_KEY + game_key - 1;
	  savegame->set_string(index, previous_joypad_string);
	}
	else {
	  joypad_mapping.erase(previous_joypad_string);
	}
	joypad_mapping[joypad_string] = key_to_customize;
	int index = Savegame::JOYPAD_ACTION_KEY + key_to_customize - 1;
	savegame->set_string(index, joypad_string);

	keys_pressed[key_to_customize - 1] = true;
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

  game->key_pressed(key);
}

/**
 * This function is called when a game key is pressed.
 * This event may come from the keyboard or the joypad.
 * @param key the game key pressed
 */
void Controls::game_key_released(GameKey key) {

  keys_pressed[key - 1] = false;
  game->key_released(key);
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
const std::string& Controls::get_joypad_string(GameKey game_key) {

  std::map<std::string, GameKey>::const_iterator it;
  for (it = joypad_mapping.begin(); it != joypad_mapping.end(); it++) {

    if (it->second == game_key) {
      return it->first;
    }
  }

  DIE("No joypad action is defined for game key '" << get_key_name(game_key) << "'");
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
 * Returns whether the player is currently customizing a key.
 * @return true if the player is currently customizing a key 
 */
bool Controls::is_customizing(void) {
  return customizing;
}

/**
 * When the player is customizing a key, returns the key that is being customized.
 * @return the key being customize
 */
Controls::GameKey Controls::get_key_to_customize(void) {
  if (!is_customizing()) {
    DIE("The player is not customizing a key");
  }
  return key_to_customize;
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

