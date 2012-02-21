/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_HUD_H
#define SOLARUS_HUD_H

#include "Common.h"

/**
 * @brief Displays on-screen information such as the rupees, the icons, etc.
 */
class HUD {

  private:

    Game &game;

    int nb_elements;
    HudElement *elements[16];

    bool showing_dialog;

    void update_blinking();

  public:

    HUD(Game &game);
    ~HUD();

    void update();
    void display(Surface& dst_surface);

};

#endif

