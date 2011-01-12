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
#include "movements/TemporalMovement.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/System.h"

/**
 * @brief Constructor.
 * @param speed the speed
 * @param angle angle of the movement in radians
 * @param duration duration of the movement in milliseconds
 * @param smooth true to make the movement smooth
 */
TemporalMovement::TemporalMovement(int speed, double angle, uint32_t duration, bool smooth):
  SmoothMovement(smooth) {
  start(speed, angle, duration);
}

/**
 * @brief Constructor.
 * @param speed the speed
 * @param source_xy the movement will start from this point
 * @param target_xy the movement will go into this point's direction
 * @param duration duration of the movement in milliseconds
 * @param smooth true to make the movement smooth
 */
TemporalMovement::TemporalMovement(int speed, const Rectangle &source_xy, const Rectangle &target_xy, uint32_t duration, bool smooth):
  SmoothMovement(smooth) {

  double angle = Geometry::get_angle(source_xy.get_x(), source_xy.get_y(), target_xy.get_x(), target_xy.get_y());
  start(speed, angle, duration);
}

/**
 * @brief Destructor.
 */
TemporalMovement::~TemporalMovement() {

}

/**
 * @brief Starts the straight movement into a direction.
 * @param speed the speed
 * @param direction angle of the movement in radians
 * @param duration duration of the movement in milliseconds
 */
void TemporalMovement::start(int speed, double direction, uint32_t duration) {

  finished = false;
  end_movement_date = System::now() + duration;
  set_speed(speed);
  if (speed != 0) {
    set_angle(direction);
  }
}

/**
 * @brief Updates the movement.
 */
void TemporalMovement::update() {

  SmoothMovement::update();

  uint32_t now = System::now();
  if (now >= end_movement_date) {
    stop();
    finished = true;
  }
}

/**
 * @brief Suspends or resumes the movement
 * @param suspended true to suspend the movement, false to resume it
 */
void TemporalMovement::set_suspended(bool suspended) {

  SmoothMovement::set_suspended(suspended);

  if (!suspended) {
    end_movement_date += System::now() - get_when_suspended();
  }
}

/**
 * @brief Returns whether the movement is finished.
 *
 * This functions returns false while the delay is not finished,
 * even if the movement was stopped by an obstacle.
 *
 * @return true if the movement is finished
 */
bool TemporalMovement::is_finished() {
  return finished;
}

/**
 * @brief Stops the movement finished even if the delay planned is not finished yet.
 */
void TemporalMovement::set_finished() {
  stop();
  this->finished = true;
}

