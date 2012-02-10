/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_ARROW_H
#define SOLARUS_ARROW_H

#include "Common.h"
#include "entities/MapEntity.h"

/**
 * An arrow thrown by the bow on the map.
 */
class Arrow: public MapEntity {

  private:

    Hero *hero;                /**< the hero */
    uint32_t disappear_date;   /**< date when the arrow disappears */
    bool stop_now;             /**< true to make the arrow stop now */
    MapEntity *entity_reached; /**< an entity reached by the arrow */

  public:

    Arrow(Hero *hero);
    ~Arrow(void);

    EntityType get_type(void);

    // features
    bool can_be_obstacle(void);
    bool can_detect_entities(void);
    bool can_be_displayed(void);
    bool is_displayed_in_y_order(void);

    bool is_teletransporter_obstacle(Teletransporter *teletransporter);
    bool is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt);
    bool is_internal_stairs_obstacle(InternalStairs *internal_stairs);
    bool is_water_obstacle(void);
    bool is_hole_obstacle(void);
    bool is_ladder_obstacle(void);
    bool is_raised_block_obstacle(CrystalSwitchBlock *raised_block);
    bool is_crystal_switch_obstacle(CrystalSwitch *crystal_switch);
    bool is_npc_obstacle(InteractiveEntity *npc);
    bool is_jump_sensor_obstacle(JumpSensor *jump_sensor);

    // state
    void update(void);
    void set_suspended(bool suspended);
    void stop(void);
    bool is_stopped(void);
    bool is_flying(void);
    void attach_to(MapEntity *entity_reached);

    // collisions
    void notify_collision_with_enemy(Enemy *enemy, Sprite *enemy_sprite, Sprite *this_sprite);
    void just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed);
    bool has_reached_map_border(void);
};

#endif

