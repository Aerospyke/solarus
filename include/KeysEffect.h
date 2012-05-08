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
#ifndef SOLARUS_KEYS_EFFECT_H
#define SOLARUS_KEYS_EFFECT_H

#include "Common.h"

/**
 * @brief Associate to each game key its current effect on the game.
 *
 * This class associates to each game key (the action key,
 * the sword key, the pause key, and the two inventory item keys)
 * the current effect of pressing that key.
 *
 * When the user presses one of those keys, the engine calls
 * this class to determine what should happen (e.g. swing the sword,
 * open a chest, save the game, use an item, etc.).
 * This class is also used by the HUD to display the five icons
 * representing those keys.
 */
class KeysEffect {

  public:

    /**
     * @brief What happens when the user presses the Action key.
     */
    enum ActionKeyEffect {
      ACTION_KEY_NONE,     /**< nothing happens when the user presses the Action key and the action icon is not shown */

      ACTION_KEY_VALIDATE, /**< validate a choice */
      ACTION_KEY_NEXT,     /**< display the next message in a dialog */
      ACTION_KEY_INFO,     /**< display some information about the selected item in the menu */
      ACTION_KEY_RETURN,   /**< exit a dialog or a submenu */

      ACTION_KEY_LOOK,     /**< look at something */
      ACTION_KEY_OPEN,     /**< open a door or a chest */
      ACTION_KEY_ACTION,   /**< perform an action */
      ACTION_KEY_LIFT,     /**< lift an objet */
      ACTION_KEY_THROW,    /**< throw an object */
      ACTION_KEY_GRAB,     /**< grab an object */
      ACTION_KEY_STOP,     /**< stop an action (for example: using an object which consumes magic) */
      ACTION_KEY_SPEAK,    /**< speak with an NPC */
      ACTION_KEY_CHANGE,   /**< modify an option */
      ACTION_KEY_SWIM      /**< swim faster */
    };

    /**
     * @brief What happens when the user presses the Sword key.
     */
    enum SwordKeyEffect {
      SWORD_KEY_HIDDEN   = -1,   /**< nothing happens when the user presses the Sword key and the sword icon is not shown */
      SWORD_KEY_NONE     = 0,    /**< nothing happens when the user presses the Sword key */
      SWORD_KEY_SAVE     = 1,    /**< save the game */
      SWORD_KEY_RETURN   = 2,    /**< exit a submenu */
      SWORD_KEY_VALIDATE = 3,    /**< validate a choice in the save game dialog */
      SWORD_KEY_SKIP     = 4,    /**< skip a message in a dialog */
      SWORD_KEY_SWORD    = 5     /**< swing the sword (this is the case most of the time) */
    };

    /**
     * @brief What happens when the user presses the Pause key.
     */
    enum PauseKeyEffect {
      PAUSE_KEY_NONE,      /**< nothing happens when the user presses the Pause key */
      PAUSE_KEY_PAUSE,     /**< open the pause menu */
      PAUSE_KEY_RETURN     /**< close the pause menu */
    };

  private:

    ActionKeyEffect action_key_effect;       /**< current effect of the action key */
    ActionKeyEffect action_key_effect_saved; /**< a previous effect of the action key */
    bool action_key_enabled;                 /**< indicates that the action key is currently enabled */

    SwordKeyEffect sword_key_effect;         /**< current effect of the sword key */
    SwordKeyEffect sword_key_effect_saved;   /**< a previous effect of the sword key */
    bool sword_key_enabled;                  /**< indicates that the sword key is currently enabled */

    PauseKeyEffect pause_key_effect;         /**< current effect of the pause key */
    bool pause_key_enabled;                  /**< indicates that the pause key is currently enabled */

    bool item_keys_enabled;

  public:

    KeysEffect();
    ~KeysEffect();

    // action key

    ActionKeyEffect get_action_key_effect();
    void set_action_key_effect(ActionKeyEffect action_key_effect);
    bool is_action_key_enabled();
    void set_action_key_enabled(bool enable);
    void save_action_key_effect();
    void restore_action_key_effect();
    bool is_action_key_acting_on_facing_entity();

    // sword key

    SwordKeyEffect get_sword_key_effect();
    void set_sword_key_effect(SwordKeyEffect sword_key_effect);
    bool is_sword_key_enabled();
    void set_sword_key_enabled(bool enable);
    void save_sword_key_effect();
    void restore_sword_key_effect();

    // pause key

    PauseKeyEffect get_pause_key_effect();
    void set_pause_key_effect(PauseKeyEffect pause_key_effect);
    bool is_pause_key_enabled();
    void set_pause_key_enabled(bool enable);

    // item keys
    bool are_item_keys_enabled();
    void set_item_keys_enabled(bool enable);

    // all keys
    void set_all_keys_enabled(bool enable);

};

#endif

