/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef MAIN_LOOP_H
#define MAIN_LOOP_H

#include "Common.h"
#include <string>

/**
 * @brief Main class of the game engine.
 *
 * It starts the program and handles the succession of its screens.
 */
class MainLoop {

  public:

    MainLoop(int argc, char** argv);
    ~MainLoop();

    void run();
    void set_exiting();
    bool is_exiting();
    void set_resetting();
    Game* get_game();
    void set_game(Game* game);

    DebugKeys& get_debug_keys();
    LuaContext& get_lua_context();

  private:

    Surface* root_surface;      /**< the surface where everything is drawn (always SOLARUS_GAME_WIDTH * SOLARUS_GAME_HEIGHT) */
    DebugKeys* debug_keys;      /**< special keys to debug the game, e.g. to traverse walls (disabled in release mode) */
    LuaContext* lua_context;    /**< the Lua world where scripts are run */
    bool exiting;               /**< indicates that the program is about to stop */
    Game* game;                 /**< The current game if any, NULL otherwise. */
    Game* next_game;            /**< The game to start at next cycle (NULL means resetting the game). */

    void notify_input(InputEvent& event);
    void draw();
    void update();
};

#endif

