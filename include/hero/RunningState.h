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
#ifndef SOLARUS_HERO_RUNNING_STATE_H
#define SOLARUS_HERO_RUNNING_STATE_H

#include "hero/State.h"

/**
 * @brief The state "Running" of the hero.
 */
class Hero::RunningState: public Hero::State {

  private:

    int phase;				/**< current phase of the run */
    uint32_t next_phase_date;		/**< date of the next phase */

  public:

    RunningState(Hero *hero);
    ~RunningState(void);

    void start(State *previous_state);
    void stop(State *next_state);
    void update(void);
    void set_suspended(bool suspended);
    void notify_movement_tried(bool success);
    int get_wanted_movement_direction8(void);
    bool can_take_jump_sensor(void);
    bool can_be_hurt(void);
};

#endif

