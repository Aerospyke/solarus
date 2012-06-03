/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "menus/PauseSubmenu.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Sound.h"
#include "Game.h"

/**
 * @brief Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenu::PauseSubmenu(PauseMenu &pause_menu, Game &game):
  pause_menu(pause_menu), game(game), savegame(game.get_savegame()),
  equipment(game.get_equipment()) {

  caption_text_1 = new TextSurface(SOLARUS_GAME_WIDTH_MIDDLE, 0,
				   TextSurface::ALIGN_CENTER,
				   TextSurface::ALIGN_MIDDLE);
  caption_text_1->set_font("fixed");

  caption_text_2 = new TextSurface(SOLARUS_GAME_WIDTH_MIDDLE, 0,
				   TextSurface::ALIGN_CENTER,
				   TextSurface::ALIGN_MIDDLE);
  caption_text_2->set_font("fixed");
}

/**
 * @brief Destructor.
 */
PauseSubmenu::~PauseSubmenu() {
  delete caption_text_1;
  delete caption_text_2;
}

/**
 * @brief Displays the common part of a submenu.
 *
 * The subclasses should call this function.
 *
 * @param destination the surface to draw
 */
void PauseSubmenu::display(Surface *destination) {
  caption_text_1->display(destination);
  caption_text_2->display(destination);
}

/**
 * @brief Sets the caption text.
 *
 * The caption text can have one or two lines, with 20 characters maximum for each line.
 * If the text you want to display has two lines, use the '$' character to separate them.
 *
 * @param text the caption text to set
 */
void PauseSubmenu::set_caption_text(const std::string &text) {

  // check the presence of the '$' character
  size_t index = text.find("$", 0);

  if (index == std::string::npos) {
    // only one line
    caption_text_1->set_y(209);
    caption_text_1->set_text(text);
    caption_text_2->set_text("");
  }
  else {
    // two lines

    const std::string line1 = text.substr(0, index);
    const std::string line2 = text.substr(index + 1);

    caption_text_1->set_y(203);
    caption_text_2->set_y(215);
    caption_text_1->set_text(line1);
    caption_text_2->set_text(line2);
  }
}

/**
 * @brief Plays the cursor sound.
 */
void PauseSubmenu::play_cursor_sound() {
  Sound::play("cursor");
}

