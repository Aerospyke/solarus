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
#include "entities/InternalStairs.h"
#include "lowlevel/FileTools.h"

/**
 * Creates a new internal stairs entity.
 * @param layer layer of the entity to create on the map
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param direction direction of the stairs (0 to 3)
 */
InternalStairs::InternalStairs(Layer layer, int x, int y, int direction):
  Detector(COLLISION_FACING_POINT | COLLISION_RECTANGLE, "", layer, x, y, 16, 16) {

  if (layer == LAYER_HIGH) {
    DIE("Cannot put internal stairs on the high layer");
  }

  set_direction(direction);
  set_origin(0, 0);
}

/**
 * Destructor.
 */
InternalStairs::~InternalStairs(void) {

}

/**
 * Creates an instance from an input stream.
 * The input stream must respect the syntax of this entity type.
 * @param game the game that will contain the entity created
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity * InternalStairs::parse(Game *game, std::istream &is, Layer layer, int x, int y) {

  int direction;
  FileTools::read(is, direction);
  return new InternalStairs(Layer(layer), x, y, direction);
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType InternalStairs::get_type() {
  return INTERNAL_STAIRS;
}

/**
 * Returns whether this entity can have collisions with entities even if
 * they are not on the same layer.
 * @return true if this entity can collide with entities that are on another layer
 */
bool InternalStairs::has_layer_independent_collisions(void) {
  return true;
}

/**
 * Returns true if this entity does not react to the sword.
 * If true is returned, nothing will happen when the hero hits this entity with the sword.
 * @return true if the sword is ignored
 */
bool InternalStairs::is_sword_ignored(void) {
  return true;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this block is raised
 */
bool InternalStairs::is_obstacle_for(MapEntity *other) {
  return other->is_internal_stairs_obstacle(this);
}

/**
 * This function is called when another entity overlaps this entity.
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void InternalStairs::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {
  entity_overlapping->notify_collision_with_internal_stairs(this);
}

