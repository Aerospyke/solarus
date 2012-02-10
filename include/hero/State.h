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
#ifndef SOLARUS_HERO_STATE_H
#define SOLARUS_HERO_STATE_H

#include "Common.h"
#include "entities/Hero.h"

/**
 * @brief Abstract base class for a state of the hero.
 *
 * This class contains all hero-related functions that depend on his state
 * and provide a default implementation for them.
 * Most of them are almost empty here because they depend on the state.
 * Redefine for each state the functions that you need to implement or change.
 */
class Hero::State {

  protected:

    Game *game;				/**< the game */
    Map *map;				/**< the current map */
    Hero *hero;				/**< the hero controlled by this state */
    uint32_t when_suspended;		/**< indicates when this state was suspended */

    State(Hero *hero);

  public:

    // creation and destruction
    virtual ~State(void);
    virtual void start(State *previous_state);
    virtual void stop(State *next_state);

    // game loop
    virtual void update(void);
    virtual void display_on_map(void);
    virtual void set_suspended(bool suspended);
    virtual void action_key_pressed(void);
    virtual void sword_key_pressed(void);
    virtual void directional_key_pressed(int direction4);
    virtual void directional_key_released(int direction4);
    virtual void item_key_pressed(int slot);
    virtual void item_key_released(int slot);

    // game
    virtual void set_map(Map *map);
    virtual bool can_start_gameover_sequence(void);

    // sprites
    virtual bool is_hero_visible(void);
    virtual bool is_direction_locked(void);
    virtual int get_height_above_shadow(void);

    virtual bool can_control_movement(void);
    virtual bool can_control_direction(void);
    virtual int get_wanted_movement_direction(void);
    virtual void notify_walking_speed_changed(void);
    virtual void notify_layer_changed(void);
    virtual void notify_movement_changed(void);
    virtual void notify_movement_result(bool tried_to_move, bool success);
    virtual void reset_movement(void);

    // ground
    virtual bool can_avoid_deep_water(void);
    virtual bool can_avoid_hole(void);
    virtual bool is_touching_ground(void);
    virtual void notify_ground_changed(void);

    // obstacles and collisions
    virtual void set_stop_on_obstacles(bool stop_on_obstacles);
    virtual bool are_collisions_ignored(void);
    virtual bool is_water_obstacle(void);
    virtual bool is_hole_obstacle(void);
    virtual bool is_ladder_obstacle(void);
    virtual bool is_teletransporter_obstacle(Teletransporter *teletransporter);
    virtual bool can_avoid_teletransporter(void);
    virtual bool is_teletransporter_delayed(void);
    virtual bool is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt);
    virtual bool can_avoid_conveyor_belt(void);
    virtual bool can_avoid_stairs(void);
    virtual bool is_sensor_obstacle(Sensor *sensor);
    virtual bool can_avoid_sensor(void);
    virtual bool is_jump_sensor_obstacle(JumpSensor *jump_sensor);
    virtual bool can_avoid_explosion(void);

    // enemies
    virtual void notify_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed);
    virtual int get_sword_damage_factor(void);
    virtual bool can_be_hurt(void);

    // state specific
    virtual bool is_free(void);
    virtual bool is_grabbing_or_pulling(void);
    virtual bool is_moving_grabbed_entity(void);
    virtual void notify_grabbed_entity_collision(void);
    virtual bool is_striking_with_sword(Detector *detector);
    virtual bool can_start_sword(void);
    virtual bool can_sword_hit_crystal_switch(void);
};

#endif

