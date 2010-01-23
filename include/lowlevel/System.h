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
#ifndef SOLARUS_SYSTEM_H
#define SOLARUS_SYSTEM_H

#include "Common.h"

/**
 * This class provides lowlevel functions and initialization.
 */
class System {

  public:

    static void initialize(int argc, char **argv);
    static void quit(void);
    static void update(void);

    static uint32_t now(void);
    static void sleep(uint32_t duration);
};

#endif

