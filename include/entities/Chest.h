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
#ifndef SOLARUS_CHEST_H
#define SOLARUS_CHEST_H

#include "Common.h"
#include "Treasure.h"
#include "entities/Detector.h"

/**
 * @brief Represents a chest on a map.
 *
 * It can be a normal chest or a big chest.
 */
class Chest: public Detector {

  private:

    Treasure treasure;			/**< the treasure placed in this chest (possibly "_none") */
    bool big_chest;			/**< true for a big chest, false for normal chests */
    bool open;				/**< true if the chest is open (but the treasure may not have
					 * been given yet because there is a delay of 500 ms) */
    bool treasure_given;		/**< true if the chest is open and the treasure has been given to the player */
    uint32_t treasure_date;		/**< date when the treasure will be given to the player */

    void initialize_sprite();

  public:

    Chest(const std::string &name, Layer layer, int x, int y, bool big_chest, const Treasure &treasure);
    ~Chest();
    static CreationFunction parse;

    EntityType get_type();
    bool is_displayed_in_y_order();

    bool is_open();
    void set_open(bool open);
    void set_visible(bool visible);

    bool is_obstacle_for(MapEntity &other);
    void notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode);
    void update();
    void action_key_pressed();
    void set_suspended(bool suspended);
};

#endif

