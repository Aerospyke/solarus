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
#ifndef SOLARUS_TILE_PATTERN_H
#define SOLARUS_TILE_PATTERN_H

#include "Common.h"
#include "entities/Obstacle.h"

/**
 * @brief Abstract class for a tile pattern.
 *
 * A tile pattern defines a rectangle image in a tileset and has an obstacle property.
 * It may have a special animation.
 * The width and the height of a tile pattern are always multiples or 8.
 */
class TilePattern {

  protected:

    const Obstacle obstacle; /**< is the tile an obstacle? */

    const int width;         /**< tile width (multiple of 8) */
    const int height;        /**< tile height (multiple of 8) */

    TilePattern(Obstacle obstacle, int width, int height);

  public:

    virtual ~TilePattern();

    int get_width() const;
    int get_height() const;
    Obstacle get_obstacle() const;

    static void update();
    void display_on_map(Map &map, const Rectangle &position_in_map);

    /**
     * Displays the tile image on a surface.
     * @param destination the destination surface
     * @param dst_position position of the tile pattern on the destination surface
     * @param tileset tileset of this tile
     */
    virtual void display(Surface *destination, const Rectangle &dst_position, Tileset &tileset) = 0;

    virtual bool is_static();
};

#endif

