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
#include "entities/Teletransporter.h"
#include "ZSDX.h"
#include "Game.h"
#include "Sprite.h"
#include "Map.h"
#include "entities/Hero.h"

/**
 * Constructor.
 * @param name name of the teletransporter
 * @param layer layer of the teletransporter
 * @param x x position of the teletransporter's rectangle
 * @param y y position of the teletransporter's rectangle
 * @param width width of the teletransporter's rectangle
 * @param height height of the teletransporter's rectangle
 * @param subtype the subtype of teletransporter
 * @param transition_style style of transition between the two maps
 * @param destination_map_id id of the destination map
 * @param destination_point_name location on the destination map, or "_same" to keep the hero's coordinates,
 * or "_side" to place the hero on the appropriate side of the map
 */
Teletransporter::Teletransporter(const std::string &name, Layer layer, int x, int y, int width, int height,
				 Subtype subtype, Transition::Style transition_style,
				 MapId destination_map_id, std::string destination_point_name):
  Detector(COLLISION_CUSTOM, name, layer, x, y, width, height),
  subtype(subtype), transition_style(transition_style),
  destination_map_id(destination_map_id), destination_point_name(destination_point_name) {
  
  if (subtype == YELLOW) {
    create_sprite("entities/teletransporter");
    get_sprite()->set_current_animation("yellow");
  }
  else {
    // TODO
  }
}

/**
 * Destructor.
 */
Teletransporter::~Teletransporter(void) {

}

/**
 * Sets the map.
 * @param map the map
 */
void Teletransporter::set_map(Map *map) {

  MapEntity::set_map(map);

  if (destination_point_name == "_side") {

    int x = get_x();
    int y = get_y();

    if (get_width() == 16 && x == -16) {
      destination_side = 0;
    }
    else if (get_width() == 16 && x == map->get_width()) {
      destination_side = 2;
    }
    else if (get_height() == 16 && y == -16) {
      destination_side = 3;
    }
    else if (get_height() == 16 && y == map->get_height()) {
      destination_side = 1;
    }
    else {
      DIE("Bad position of teletransporter '" << get_name() << "'"); 
    }
  }

  transition_direction = (destination_side + 2) % 4;
}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Teletransporter::get_type() {
  return TELETRANSPORTER;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Teletransporter::is_obstacle_for(MapEntity *other) {
  return other->is_teletransporter_obstacle(this);
}

/**
 * Checks whether an entity's collides with this entity.
 * @param entity an entity
 * @return true if the entity's collides with this entity
 */
bool Teletransporter::check_collision_custom(MapEntity *entity) {

  if (destination_point_name == "_side") {

    if (!entity->is_hero()) {
      return false;
    }

    Hero *hero = (Hero*) entity;
    SDL_Rect facing_point = hero->get_facing_point(transition_direction);
    return hero->is_moving_towards(transition_direction)
      && is_point_in(get_position_in_map(), facing_point.x, facing_point.y);
  }

  else if (destination_point_name == "_same") {
  
    if (!entity->is_hero()) {
      return false;
    }

    Hero *hero = (Hero*) entity;
    return check_collision_origin_point(hero) && (!hero->is_on_hole() || hero->get_state() == Hero::RETURNING_TO_SOLID_GROUND);
  }

  const SDL_Rect &entity_position = entity->get_position_in_map();
  int x1 = entity_position.x + 4;
  int x2 = x1 + entity_position.w - 5;
  int y1 = entity_position.y + 4;
  int y2 = y1 + entity_position.h - 5;

  return is_point_in(get_position_in_map(), x1, y1) &&
    is_point_in(get_position_in_map(), x2, y1) &&
    is_point_in(get_position_in_map(), x1, y2) &&
    is_point_in(get_position_in_map(), x2, y2);
}

/**
 * This function is called by the engine when an entity overlaps the teletransporter.
 * This is a redefinition of Detector::collision().
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Teletransporter::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  entity_overlapping->collision_with_teletransporter(this, collision_mode);
}

/**
 * Makes the teletransporter move the hero to the destination.
 * @param hero the hero
 */
void Teletransporter::transport_hero(Hero *hero) {

  std::string name = destination_point_name;

  if (destination_point_name == "_side") {

    // special desination point: side of the map
    // we determine the appropriate side based on the teletransporter's position;
    // we also place the hero on the old map so that its position corresponds to the new map

    switch (destination_side) {

    case 0:
      name += '0'; // scroll to the west
      hero->set_x(0);
      break;

    case 1:
      name += '1'; // scroll to the south
      hero->set_y(map->get_height() + 5);
      break;

    case 2:
      name += '2'; // scroll to the east
      hero->set_x(map->get_width());
      break;

    case 3:
      name += '3'; // scroll to the north
      hero->set_y(5);
      break;

    default:
      DIE("Bad destination side for teletransporter '" << get_name() << "'"); 
    }
  }

  zsdx->game->set_current_map(destination_map_id, name, transition_style);
}
