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
#ifndef SOLARUS_HERO_JUMPING_STATE_H
#define SOLARUS_HERO_JUMPING_STATE_H

#include "hero/State.h"

/**
 * @brief The state "Jumping" of the hero.
 */
class Hero::JumpingState: public Hero::State {

  private:

    JumpMovement *movement;			/**< the movement applied to the hero */
    int direction8;				/**< direction of the jump (0 to 7) */
    bool with_sound;				/**< indicates that a jump sound is played */
    Layer layer_after_jump;			/**< layer where the hero arrives (LAYER_NB means unchanged) */

  public:

    JumpingState(Hero &hero, int direction8, int length, bool ignore_obstacles, bool with_sound,
	uint32_t movement_delay = 0, Layer layer_after_jump = LAYER_NB);
    ~JumpingState();

    void start(State *previous_state);
    void stop(State *next_state);
    void update();
    int get_wanted_movement_direction8();
    bool can_start_gameover_sequence();
    int get_height_above_shadow();
    bool is_touching_ground();
    bool can_avoid_deep_water();
    bool can_avoid_hole();
    bool can_avoid_teletransporter();
    bool can_avoid_conveyor_belt();
    bool is_sensor_obstacle(Sensor *sensor);
    bool can_avoid_sensor();
    bool can_be_hurt();
};

#endif

