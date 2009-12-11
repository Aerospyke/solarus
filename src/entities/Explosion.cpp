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
#include "entities/Explosion.h"
#include "entities/Enemy.h"
#include "entities/MapEntities.h"
#include "Map.h"
#include "ResourceManager.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "lowlevel/Sound.h"

/**
 * Creates an explosion.
 * @param xy coordinates of the center of the explosion
 * @param with_damages true to hurt the hero and the enemies
 */
Explosion::Explosion(Layer layer, const Rectangle &xy, bool with_damages):
  Detector(COLLISION_SPRITE, "", layer, xy.get_x(), xy.get_y(), 48, 48) { 

  // initialize the entity
  create_sprite("entities/explosion");
  ResourceManager::get_sound("explosion")->play();

  if (with_damages) {
    get_sprite()->get_animation_set()->enable_pixel_collisions();
    set_bounding_box_from_sprite();
  }
}

/**
 * Destructor.
 */
Explosion::~Explosion(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Explosion::get_type() {
  return BOOMERANG;
}

/**
 * Returns whether entities of this type can be obstacles for other entities.
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 * @return true if this type of entity can be obstacle for other entities
 */
bool Explosion::can_be_obstacle(void) {
  return false; 
}

/**
 * Returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * notify_collision() will be called when a collision is detected.
 * @return true if this type of entity can detect other entities
 */
bool Explosion::can_detect_entities(void) {
  return true;
}

/**
 * Returns whether entities of this type can be displayed.
 * If enabled, the sprites added by the add_sprite() calls will be 
 * displayed (if any).
 * @return true if this type of entity can be displayed
 */
bool Explosion::can_be_displayed(void) {
  return true; 
}

/**
 * Returns whether an entity of this type should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are displayed in the normal order (i.e. as specified by the map file), 
 * and before the entities with the feature.
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool Explosion::is_displayed_in_y_order(void) {
  return false;
}

/**
 * Updates this entity.
 */
void Explosion::update(void) {
  Detector::update();
  if (get_sprite()->is_animation_finished()) {
    map->get_entities()->remove_entity(this);
  }
}

/**
 * This function is called by check_collision(MapEntity*, Sprite*) when another entity's
 * sprite overlaps a sprite of this detector.
 * @param other_entity the entity overlapping this detector
 * @param other_sprite the sprite of other_entity that is overlapping this detector
 * @param this_sprite the sprite of this detector that is overlapping the other entity's sprite
 */
void Explosion::notify_collision(MapEntity *other_entity, Sprite *other_sprite, Sprite *this_sprite) {
  other_entity->notify_collision_with_explosion(this, other_sprite);
}

/**
 * This function is called when an enemy's sprite collides with a sprite of this entity
 * @param enemy the enemy
 * @param enemy_sprite the enemy's sprite that overlaps a sprite of this entity
 * @param this_sprite this entity's sprite that overlaps the enemy's sprite
 */
void Explosion::notify_collision_with_enemy(Enemy *enemy, Sprite *enemy_sprite, Sprite *this_sprite) {
  try_attack_enemy(enemy, enemy_sprite);
}

/**
 * Attacks the specified enemy if possible.
 * This function is called by this explosion when it detects an enemy, or by an enemy who detects this explosion.
 * @param enemy the enemy to attack
 * @param enemy_sprite the enemy's sprite detected by the explosion
 */
void Explosion::try_attack_enemy(Enemy *enemy, Sprite *enemy_sprite) {
  
  // see if the enemy was already hurt by this explosion
  bool found = false;
  std::list<Enemy*>::iterator it;
  for (it = victims.begin(); it != victims.end() && !found; it++) {
    found = ((*it) == enemy);
  }

  if (!found) {
    enemy->try_hurt(ATTACK_EXPLOSION, this, enemy_sprite);
  }
}

/**
 * Notifies this entity that it has just attacked an enemy
 * (even if this attack was not successful).
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack:
 * - a number greater than 0 represents the number of health points the enemy has just lost
 * - a value of 0 means that the attack was just ignored 
 * - a value of -1 means that the enemy was protected against the attack
 * - a value of -2 means that the attack immobilized the enemy
 */
void Explosion::just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result) {

  if (result > 0) {
    victims.push_back(victim);
  }
}

