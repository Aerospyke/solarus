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
#ifndef SOLARUS_SIMPLE_GREEN_SOLDIER_H
#define SOLARUS_SIMPLE_GREEN_SOLDIER_H

#include "Common.h"
#include "entities/Enemy.h"

/**
 * @brief A green soldier with a small sword.
 *
 * Unlike the normal green soldier, it cannot see or follow the hero.
 */
class SimpleGreenSoldier: public Enemy {

  private:

    void walk(int direction);

  protected:

    void initialize(void);
    void restart(void);

  public:

    SimpleGreenSoldier(const ConstructionParameters &params);
    ~SimpleGreenSoldier(void);

    void update(void);
    void notify_movement_tried(bool success);
};

#endif

