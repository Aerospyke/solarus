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
#ifndef SOLARUS_HERO_STATE_SWORD_LOADING_H
#define SOLARUS_HERO_STATE_SWORD_LOADING_H

#include "hero/StatePlayerMovement.h"

/**
 * @brief The state "sword loading" of the hero.
 */
class Hero::StateSwordLoading: public Hero::StatePlayerMovement {

  private:

    uint32_t sword_loaded_date;            /**< date when the sword is loaded */
    bool sword_loaded;                     /**< becomes true when the spin attack is possible */

  public:

    StateSwordLoading(Hero *hero);
    ~StateSwordLoading(void);

    void start(State *previous_state);
    void update(void);
    void set_suspended(bool suspended);
    void set_animation_stopped(void);
    void set_animation_walking(void);
    bool is_direction_locked(void);

};

#endif

