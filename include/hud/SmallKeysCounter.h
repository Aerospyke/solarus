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
#ifndef SOLARUS_SMALL_KEYS_COUNTER_H
#define SOLARUS_SMALL_KEYS_COUNTER_H

#include "Common.h"
#include "hud/HudElement.h"

/**
 * This class displays the small key counter and its icon
 * when the small keys are enabled in the current map.
 */
class SmallKeysCounter: public HudElement {

  private:

    Surface *img_icon;
    Counter *counter;

  public:

    SmallKeysCounter(Game *game, int x, int y);
    ~SmallKeysCounter(void);

    void update(void);
    void rebuild(void);
};

#endif

