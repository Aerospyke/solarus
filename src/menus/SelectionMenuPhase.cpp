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
#include "menus/SelectionMenuPhase.h"
#include "menus/SelectionMenu.h"
#include "Transition.h"
#include "Sprite.h"
#include "StringResource.h"
#include "hud/HeartsView.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Rectangle.h"

/**
 * Creates a selection menu phase.
 * @param selection_menu the selection menu this phase will belong to
 * @param title_string_key key of the title string to display in this phase
 */
SelectionMenuPhase::SelectionMenuPhase(SelectionMenu *menu, const std::string &title_string_key):
  menu(menu) {

  menu->set_title_text(title_string_key);
}

/**
 * Destroys the selection menu phase.
 */
SelectionMenuPhase::~SelectionMenuPhase(void) {

}

/**
 * Updates the selection menu in this phase.
 */
void SelectionMenuPhase::update(void) {
  // nothing to do by default (subclasses can redefine this method)
}

/**
 * Displays this phase of the selection menu.
 * @param destination_surface surface where this phase has to be drawn
 */
void SelectionMenuPhase::display(Surface *destination_surface) {
  // nothing to do by default (subclasses can redefine this method)
}

/**
 * This function is called when an event occurs.
 * @param event the event
 */
void SelectionMenuPhase::handle_event(const SDL_Event &event) {
  // nothing to do by default (subclasses can redefine this method)
}

