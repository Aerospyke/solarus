/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_INVENTORY_ITEM_H
#define SOLARUS_INVENTORY_ITEM_H

#include "Common.h"
#include "Savegame.h"

/**
 * @brief Represents an item from the inventory that the player can use.
 *
 * This class handles the behavior of the item when the player uses it.
 * It only includes the items displayed on the inventory submenu
 * (accessible when the game is paused) and that can be assigned to an item key.
 */
class InventoryItem {

  private:

    Game &game;			/**< the game this item is used in */
    std::string item_name;	/**< name of the item being used */
    int variant;		/**< the possession state of this item when it is used */

    // state
    bool finished;		/**< true if we have finished using this item or not using it */

  public:

    // creation and destruction
    InventoryItem(Game &game, const std::string &item_name);
    ~InventoryItem();

    // item information
    const std::string& get_name();
    int get_variant();

    // state
    void start();
    void update();
    void set_map(Map &map);
    bool is_finished();
    void set_finished();
};

#endif

