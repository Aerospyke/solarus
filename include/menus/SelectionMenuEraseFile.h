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
#ifndef ZSDX_SELECTION_MENU_ERASE_FILE_H
#define ZSDX_SELECTION_MENU_ERASE_FILE_H

#include "Common.h"
#include "menus/SelectionMenuPhase.h"

/**
 * Phase of the selection menu, where the user
 * chooses a file to erase.
 */
class SelectionMenuEraseFile: public SelectionMenuPhase {

  public:

    // creation and destruction
    SelectionMenuEraseFile(SelectionMenuPhase *previous);
    ~SelectionMenuEraseFile(void);

    // update and display
    void handle_event(const SDL_Event &event);
    void display(Surface *screen_surface);
};

#endif

