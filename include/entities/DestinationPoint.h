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
#ifndef ZSDX_DESTINATION_POINT_H
#define ZSDX_DESTINATION_POINT_H

#include "Common.h"
#include "MapEntity.h"

/**
 * A destination point on the map.
 * A destination point is a location where the hero can arrive when using a teletransporter.
 */
class DestinationPoint: public MapEntity {

 private:

  bool is_visible;
  bool change_direction;

 public:

  DestinationPoint(std::string name, Layer layer, int x, int y, int hero_direction, bool is_visible);
  ~DestinationPoint(void);

  EntityType get_type(void);
};

#endif
