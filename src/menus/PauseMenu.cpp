#include "menus/PauseMenu.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "DungeonEquipment.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "menus/PauseSubmenuInventory.h"
#include "menus/PauseSubmenuMap.h"
#include "menus/PauseSubmenuQuestStatus.h"
#include "menus/PauseSubmenuOptions.h"

/**
 * Opens a pause menu.
 * @param game the game
 */
PauseMenu::PauseMenu(Game *game):
  game(game), savegame(game->get_savegame()), equipment(savegame->get_equipment()),
  dungeon_equipment(savegame->get_dungeon_equipment()), keys_effect(game->get_keys_effect()) {

  submenus[0] = new PauseSubmenuInventory(game);
  submenus[1] = new PauseSubmenuMap(game);
  submenus[2] = new PauseSubmenuQuestStatus(game);
  submenus[3] = new PauseSubmenuOptions(game);

  current_submenu_index = savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU);

  backgrounds_surface = ResourceManager::load_image("menus/pause_submenus.png");

  ResourceManager::get_sound("pause_open")->play();

  keys_effect->set_pause_key_effect(KeysEffect::PAUSE_KEY_RETURN);
  keys_effect->save_action_key_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  keys_effect->save_sword_key_effect();
  keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_SAVE);
}

/**
 * Destructor.
 */
PauseMenu::~PauseMenu(void) {
  for (int i = 0; i < 4; i++) {
    delete submenus[i];
  }
}

/**
 * Returns the submenu currently displayed.
 * @return the current submenu
 */
PauseSubmenu * PauseMenu::get_current_submenu(void) {
  return submenus[current_submenu_index];
}

/**
 * Exits the pause menu.
 */
void PauseMenu::quit(void) {

  ResourceManager::get_sound("pause_closed")->play();
  keys_effect->set_pause_key_effect(KeysEffect::PAUSE_KEY_PAUSE);
  keys_effect->restore_action_key_effect();
  keys_effect->restore_sword_key_effect();
}

/**
 * This function is called when a key is pressed.
 * @param keysym the key pressed
 */
void PauseMenu::key_pressed(const SDL_keysym &keysym) {

  if (keysym.sym == SDLK_d) {
    quit();
    game->set_paused(false);
  }
  else {
    get_current_submenu()->key_pressed(keysym);
  }
}

/**
 * Updates the pause menu.
 */
void PauseMenu::update(void) {
  get_current_submenu()->update();
}

/**
 * Displays the pause menu.
 */
void PauseMenu::display(SDL_Surface *destination) {

  // display the background for the current submenu
  SDL_Rect src_position = {320 * current_submenu_index, 0, 320, 240};
  SDL_BlitSurface(backgrounds_surface, &src_position, destination, NULL);

  // display the current submenu content
  get_current_submenu()->display(destination);
}
