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
#ifndef SOLARUS_TEMPORAL_MOVEMENT_H
#define SOLARUS_TEMPORAL_MOVEMENT_H

#include "Common.h"
#include "movements/SmoothMovement.h"

/**
 * @brief Movement of an entity that follows a straight trajectory
 * for an amount of time.
 *
 * If an obstacle is reached, the movement tries to fix the trajectory
 * so that the movement can continue (assuming the option <tt>smooth</tt>
 * is set).
 */
class TemporalMovement: public SmoothMovement {

  private:

    uint32_t end_movement_date;
    bool finished;

  public:

    TemporalMovement(int speed, double angle, uint32_t time, bool smooth = true);
    TemporalMovement(int speed, const Rectangle &source_xy, const Rectangle &target_xy, uint32_t time, bool smooth = true);
    ~TemporalMovement();

    void update();
    void set_suspended(bool suspended);
    bool is_finished();
    void set_finished();
    void start(int speed, double direction, uint32_t time);
};

#endif

