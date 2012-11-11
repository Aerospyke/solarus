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
#ifndef SOLARUS_GAME_CONTROLS_H
#define SOLARUS_GAME_CONTROLS_H

#include "Common.h"
#include "lowlevel/InputEvent.h"
#include <string>
#include <map>

/**
 * @brief Stores the game controls.
 *
 * This class handles the customization of the keyboard and joypad controls.
 * It receives the low-level keyboard and joypad events that occur during
 * the game and then notifies the appropriate objects that a game key was activated or released.
 * What we call a game key is a high-level notion such as "the action key".
 * The corresponding low-level event can be a keyboard event or a joypad event.
 */
class GameControls {

  public:

    /**
     * @brief The game keys that can be customized.
     *
     * These high-level keys can be mapped onto the keyboard and the joypad.
     */
    enum GameKey {
      NONE = 0,
      ACTION,
      SWORD,
      ITEM_1,
      ITEM_2,
      PAUSE,
      RIGHT,
      UP,
      LEFT,
      DOWN
    };

  private:

    Game& game;						/**< the game */
    Savegame& savegame;					/**< the savegame, which stores the keyboard and joypad mappings of the game keys */

    std::string game_key_names[9];			/**< human name of each game key, in the current language */
    std::map<InputEvent::KeyboardKey, GameKey> keyboard_mapping;	/**< associates each game key to the keyboard key that triggers it */
    std::map<std::string, GameKey> joypad_mapping;	/**< associates each game key to the joypad action that triggers it */
    bool keys_pressed[9];				/**< memorizes the state of each game key */
    static const uint16_t direction_masks[4];		/**< bit mask associated to each directional key: this allows to
							 * store any combination of the four main directions into a simple integer */
    static const int masks_to_directions8[16];		/**< associates to each possible combination of directional_keys a direction
							  * between 0 and 7, or -1 to indicate that no direction is set */

    bool customizing;					/**< indicates that the next keyboard event will be considered as the new mapping for a game key */
    GameKey key_to_customize;				/**< the game key to customize when customizing is true */

    static const std::string direction_names[4];	/**< English name of each arrow direction, used to represent a joypad action by a string */

    void game_key_pressed(GameKey key);
    void game_key_released(GameKey key);

    // keyboard mapping
    void key_pressed(InputEvent::KeyboardKey keyboard_key_pressed);
    void key_released(InputEvent::KeyboardKey keyboard_key_released);
    const std::string& get_keyboard_key_savegame_variable(GameKey game_key);
    const std::string& get_joypad_string_savegame_variable(GameKey game_key);
    InputEvent::KeyboardKey get_saved_keyboard_key(GameKey game_key);
    void set_saved_keyboard_key(GameKey game_key, InputEvent::KeyboardKey key);
    const std::string& get_saved_joypad_string(GameKey game_key);
    void set_saved_joypad_string(GameKey game_key, const std::string& joypad_string);

    // joypad mapping
    void joypad_button_pressed(int button);
    void joypad_button_released(int button);
    void joypad_axis_moved(int axis, int direction);
    void joypad_hat_moved(int hat, int direction);

  public:

    // creation and destruction
    GameControls(Game& game);
    ~GameControls();

    // controls
    InputEvent::KeyboardKey get_keyboard_key(GameKey game_key);
    const std::string& get_key_name(GameKey game_key);
    const std::string get_keyboard_string(GameKey game_key);  // TODO replace by the Solarus API names
    const std::string get_joypad_string(GameKey game_key);

    void notify_input(InputEvent& event);
    bool is_key_pressed(GameKey game_key);
    int get_wanted_direction8();

    // customization
    void customize(GameKey key);
    bool is_customizing();
    GameKey get_key_to_customize();
    bool is_customization_done();
};

#endif

