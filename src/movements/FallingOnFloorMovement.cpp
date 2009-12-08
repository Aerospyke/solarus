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
#include "movements/FallingOnFloorMovement.h"

/**
 * Y move at each time frame.
 */
const Rectangle FallingOnFloorMovement::moves[3][26] = {

  // low
  {{0, -1},  {0, 1}},

  // medium
  {{0, -1},  {0, -1},  {0, -1},  {0, -1},  {0,  1},  {0,  1},  {0,  1}, 
   {0,  1},  {0, -1},  {0, -1},  {0,  1},  {0,  1}},

  // high
  {{0, -4},  {0, -3},  {0, -3},  {0, -2},  {0, -2},  {0, -1},  {0,  1}, 
   {0,  2},  {0,  2},  {0,  3},  {0,  3},  {0,  4},  {0, -2},  {0, -1}, 
   {0, -1},  {0, -1},  {0, -1},  {0,  1},  {0,  1},  {0,  1},  {0,  1}, 
   {0,  2},  {0, -1},  {0, -1},  {0,  1},  {0,  1}},
};

/**
 * Number of moves for each height.
 */
const int FallingOnFloorMovement::nb_moves[3] = {2, 12, 26};

/**
 * Creates a movement falling object.
 * @param height height the entity will fall from
 */
FallingOnFloorMovement::FallingOnFloorMovement(FallingHeight height):
  PixelMovement(moves[height], nb_moves[height], 30, false, false) {

}

/**
 * Destructor.
 */
FallingOnFloorMovement::~FallingOnFloorMovement(void) {

}

