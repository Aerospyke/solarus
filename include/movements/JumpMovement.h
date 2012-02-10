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
#ifndef SOLARUS_JUMP_MOVEMENT_H
#define SOLARUS_JUMP_MOVEMENT_H

#include "Common.h"
#include "movements/PixelMovement.h"

/**
 * Movement for an entity that jumps in a direction.
 */
class JumpMovement: public PixelMovement {

  private:

    static const Rectangle basic_translations[8];
    Rectangle *translation_vectors;
    int jump_height;

  protected:

    void make_next_move(void);

  public:

    JumpMovement(int direction, int length, bool with_collisions);
    ~JumpMovement(void);

    int get_jump_height(void);
};

#endif

