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
#ifndef SOLARUS_SELECTION_MENU_CONFIRM_ERASE_H
#define SOLARUS_SELECTION_MENU_CONFIRM_ERASE_H

#include "Common.h"
#include "menus/SelectionMenuPhase.h"

/**
 * @brief Phase of the selection menu where the user
 * has to confirm that he really wants to erase a savegame file.
 */
class SelectionMenuConfirmErase: public SelectionMenuPhase {

  private:

    int save_number_to_erase;
    void delete_save_file(int save_number);

  public:

    // creation and destruction
    SelectionMenuConfirmErase(SelectionMenu *menu, int save_number_to_erase);
    ~SelectionMenuConfirmErase();

    // update and display
    void display(Surface& dst_surface);
    void notify_input(InputEvent &event);
};

#endif

