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
#ifndef ZSDX_CHEST_H
#define ZSDX_CHEST_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * Represents a chest on a map.
 * It can be a normal chest or a big chest.
 */
class Chest: public Detector {

  private:

    bool big_chest;       /**< true for a big chest, false for normal chests */
    Treasure *treasure;   /**< the treasure placed in this chest, or NULL if the chest contains nothing */
    bool hidden;          /**< indicates that the chest is hidden (the script has to unhide it) */
    bool open;            /**< true if the chest is open (but the treasure may not have
			   * been given yet because there is a delay of 500 ms) */
    bool treasure_given;  /**< true if the chest is open and the treasure has been given to the player */
    uint32_t treasure_date; /**< date when the treasure will be given to the player */

    void initialize_sprite(void);

  public:

    Chest(const std::string &name, Layer layer, int x, int y, bool big_chest, Treasure *treasure);
    ~Chest(void);
    static CreationFunction parse;

    EntityType get_type(void);
    bool is_displayed_in_y_order(void);
    void set_map(Map *map);

    bool is_open(void);
    void set_open(bool open);

    bool is_hidden(void);
    void set_hidden(bool hidden);

    bool is_obstacle_for(MapEntity *other);
    void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
    void update(void);
    void display_on_map(void);
    void action_key_pressed(void);
    void set_suspended(bool suspended);
};

#endif

