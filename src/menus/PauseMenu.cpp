#include "menus/PauseMenu.h"
#include "menus/PauseSubmenuInventory.h"
#include "menus/PauseSubmenuMap.h"
#include "menus/PauseSubmenuQuestStatus.h"
#include "menus/PauseSubmenuOptions.h"
#include "Game.h"
#include "Savegame.h"
#include "ResourceManager.h"
#include "Sprite.h"
#include "Sound.h"
#include "Color.h"
#include "TextSurface.h"

// TODO: load this from some external file (for future translation)
static const string texts[] = {
  "Oui", // 0
  "Non", // 1
  "Voulez-vous sauvegarder", // 2
  "votre progression ?",     // 3
  "Voulez-vous continuer",   // 4
  "la partie ?",             // 5
};

/**
 * Opens a pause menu.
 * @param game the game
 */
PauseMenu::PauseMenu(Game *game):
  game(game), savegame(game->get_savegame()), keys_effect(game->get_keys_effect()) {

  this->current_submenu = NULL;
  this->backgrounds_surface = ResourceManager::load_image("menus/pause_submenus.png");
  SDL_SetAlpha(backgrounds_surface, SDL_SRCALPHA, 216);

  this->save_dialog_sprite = new Sprite("menus/pause_save_dialog");
  this->save_dialog_state = 0;

  ResourceManager::get_sound("pause_open")->play();
  keys_effect->set_pause_key_effect(KeysEffect::PAUSE_KEY_RETURN);
  keys_effect->save_action_key_effect();
  keys_effect->save_sword_key_effect();

  question_text[0] = new TextSurface(160, 112, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  question_text[0]->set_text_color(8, 8, 8);
  question_text[1] = new TextSurface(160, 128, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  question_text[1]->set_text_color(8, 8, 8);

  answer_text[0] = new TextSurface(100, 148, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  answer_text[0]->set_text_color(8, 8, 8);
  answer_text[0]->set_text(texts[0]);
  answer_text[1] = new TextSurface(219, 148, TextSurface::ALIGN_CENTER, TextSurface::ALIGN_MIDDLE);
  answer_text[1]->set_text_color(8, 8, 8);
  answer_text[1]->set_text(texts[1]);

  set_current_submenu(savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU));
}

/**
 * Destructor.
 */
PauseMenu::~PauseMenu(void) {
  delete current_submenu;
  SDL_FreeSurface(backgrounds_surface);
  delete save_dialog_sprite;

  delete question_text[0];
  delete question_text[1];
  delete answer_text[0];
  delete answer_text[1];
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
 * @param key the key pressed
 */
void PauseMenu::key_pressed(Controls::GameKey key) {

  // the user is in one of the submenus
  if (save_dialog_state == 0) {

    if (key == Controls::PAUSE) {
      quit();
      game->set_paused(false);
    }
    else if (key == Controls::SWORD && save_dialog_state == 0) {

      ResourceManager::get_sound("pause_open")->play();

      save_dialog_state = 1;
      save_dialog_choice = 0;
      save_dialog_sprite->set_current_animation("left");

      question_text[0]->set_text(texts[2]);
      question_text[1]->set_text(texts[3]);

      action_key_effect_saved = keys_effect->get_action_key_effect();
      sword_key_effect_saved = keys_effect->get_sword_key_effect();
      keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_VALIDATE);
      keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_NONE);
    }
    else {
      current_submenu->key_pressed(key);
    }
  }

  // the dialog box to save is visible
  else if (key == Controls::LEFT || key == Controls::RIGHT) {
    // move the cursor

    ResourceManager::get_sound("cursor")->play();
    save_dialog_choice = 1 - save_dialog_choice;
    save_dialog_sprite->set_current_animation(save_dialog_choice == 0 ? "left" : "right");
  }
  else if (key == Controls::ACTION) {
    // validate a choice

    if (save_dialog_state == 1) {
      save_dialog_state = 2;
 
      if (save_dialog_choice == 0) {
	savegame->save();
	ResourceManager::get_sound("piece_of_heart")->play();
      }
      else {
	ResourceManager::get_sound("danger")->play();
      }

      question_text[0]->set_text(texts[4]);
      question_text[1]->set_text(texts[5]);

      save_dialog_choice = 0;
      save_dialog_sprite->set_current_animation("left");
    }
    else {
      ResourceManager::get_sound("danger")->play();

      save_dialog_state = 0;
      keys_effect->set_action_key_effect(action_key_effect_saved);
      keys_effect->set_sword_key_effect(sword_key_effect_saved);

      if (save_dialog_choice == 1) {
	game->reset();
	quit();
	game->set_paused(false);
      }
    }
  }
}

/**
 * Updates the pause menu.
 */
void PauseMenu::update(void) {
  current_submenu->update();
}

/**
 * Displays the pause menu.
 */
void PauseMenu::display(SDL_Surface *destination) {

  // display the background for the current submenu
  int submenu_index = savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU);
  SDL_Rect src_position = {320 * submenu_index, 0, 320, 240};
  SDL_BlitSurface(backgrounds_surface, &src_position, destination, NULL);

  // display the current submenu content
  current_submenu->display(destination);

  // display the save dialog
  if (save_dialog_state > 0) {
    save_dialog_sprite->display(destination, 50, 87);
    question_text[0]->display(destination);
    question_text[1]->display(destination);
    answer_text[0]->display(destination);
    answer_text[1]->display(destination);
  }
}

/**
 * Moves to the specified submenu.
 * @param submenu_index the submenu to show
 */
void PauseMenu::set_current_submenu(int submenu_index) {

  // show the default action and sword keys
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_SAVE);

  // go to the specified submenu
  savegame->set_integer(Savegame::PAUSE_LAST_SUBMENU, submenu_index);

  if (current_submenu != NULL) {
    delete current_submenu;
  }

  switch (submenu_index) {

  case 0:
    current_submenu = new PauseSubmenuInventory(this, game);
    break;

  case 1:
    current_submenu = new PauseSubmenuMap(this, game);
    break;

  case 2:
    current_submenu = new PauseSubmenuQuestStatus(this, game);
    break;

  case 3:
    current_submenu = new PauseSubmenuOptions(this, game);
    break;
  }
}

/**
 * Shows the submenu located at the left side from the current one.
 */
void PauseMenu::show_left_submenu(void) {

  ResourceManager::get_sound("pause_closed")->play();
  int submenu_index = savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU);
  set_current_submenu((submenu_index + 3) % 4);
}

/**
 * Shows the submenu located at the right side from the current one.
 */
void PauseMenu::show_right_submenu(void) {

  ResourceManager::get_sound("pause_closed")->play();
  int submenu_index = savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU);
  set_current_submenu((submenu_index + 1) % 4);
}
