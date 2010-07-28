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
#include "entities/CarriedItem.h"
#include "entities/DestructibleItem.h"
#include "entities/Hero.h"
#include "entities/Enemy.h"
#include "entities/Explosion.h"
#include "entities/Sensor.h"
#include "entities/Stairs.h"
#include "entities/MapEntities.h"
#include "movements/PixelMovement.h"
#include "movements/FollowMovement.h"
#include "movements/CollisionMovement.h"
#include "Sprite.h"
#include "Game.h"
#include "Map.h"
#include "lowlevel/System.h"

/**
 * @brief Movement of the item when the hero is lifting it.
 */
static const Rectangle lifting_translations[4][6] = {
  { Rectangle(0, 0), Rectangle(0, 0), Rectangle(-3, -3), Rectangle(-5, -3), Rectangle(-5,-2) },
  { Rectangle(0, 0), Rectangle(0, 0), Rectangle( 0, -1), Rectangle( 0, -1), Rectangle( 0, 0) },
  { Rectangle(0, 0), Rectangle(0, 0), Rectangle( 3, -3), Rectangle( 5, -3), Rectangle( 5,-2) },
  { Rectangle(0, 0), Rectangle(0, 0), Rectangle( 0,-10), Rectangle( 0,-12), Rectangle( 0, 0) },
};

/**
 * @brief Constructor.
 *
 * Creates a carried item (i.e. an item carried by the hero)
 * based on a destructible item (i.e. the item placed on the map
 * before the hero lifts it).
 *
 * @param hero the hero
 * @param destructible_item a destructible item
 */
CarriedItem::CarriedItem(Hero *hero, DestructibleItem *destructible_item):
  MapEntity(), hero(hero), is_lifting(true), is_throwing(false), is_breaking(false),
  break_on_intermediate_layer(false), explosion_date(0) {

  // put the item on the hero's layer
  set_layer(hero->get_layer());

  // align correctly the item with the hero
  int direction = hero->get_animation_direction();
  if (direction % 2 == 0) {
    set_xy(destructible_item->get_x(), hero->get_y());
  }
  else {
    set_xy(hero->get_x(), destructible_item->get_y());
  }
  set_origin(destructible_item->get_origin());
  set_size(destructible_item->get_width(), destructible_item->get_height());

  // create the lift movement and the sprite
  PixelMovement *movement = new PixelMovement(lifting_translations[direction], 6, 100, false, false);
  create_sprite(destructible_item->get_animation_set_id());
  get_sprite()->set_current_animation("stopped");
  set_movement(movement);

  // create the breaking sound
  destruction_sound_id = destructible_item->get_destruction_sound_id();

  // create the shadow (not visible yet)
  shadow_sprite = new Sprite("entities/shadow");
  shadow_sprite->set_current_animation("big");

  // damage on enemies
  damage_on_enemies = destructible_item->get_damage_on_enemies();

  // explosion
  if (destructible_item->can_explode()) {
    explosion_date = System::now() + 6000;
  }
}

/**
 * @brief Destructor.
 */
CarriedItem::~CarriedItem(void) {
  delete shadow_sprite;
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType CarriedItem::get_type() {
  return CARRIED_ITEM;
}

/**
 * @brief Returns whether entities of this type can be obstacles for other entities.
 *
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 *
 * @return true if this type of entity can be obstacle for other entities
 */
bool CarriedItem::can_be_obstacle(void) {
  return false; 
}

/**
 * @brief Returns whether entities of this type have detection capabilities.
 *
 * This function returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * notify_collision() will be called when a collision is detected.
 *
 * @return true if this type of entity can detect other entities
 */
bool CarriedItem::can_detect_entities(void) {
  return false;
}

/**
 * @brief Returns whether entities of this type can be displayed.
 *
 * If yes, the sprites added by the add_sprite() calls will be 
 * displayed (if any).
 *
 * @return true if this type of entity can be displayed
 */
bool CarriedItem::can_be_displayed(void) {
  return true; 
}

/**
 * @brief Returns whether this entity has to be displayed in y order.
 *
 * This function returns whether an entity of this type should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are displayed in the normal order (i.e. as specified by the map file), 
 * and before the entities with the feature.
 *
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool CarriedItem::is_displayed_in_y_order(void) {
  return true;
}

/**
 * @brief Returns the damage this item can cause to ennemies.
 * @return the damage on enemies
 */
int CarriedItem::get_damage_on_enemies(void) {
  return damage_on_enemies;
}

/**
 * @brief Makes the item sprite stop moving.
 *
 * This function is called when the hero stops walking while carrying the item.
 * The item also stops moving.
 */
void CarriedItem::set_animation_stopped(void) {

  if (!is_lifting && !is_throwing) {
    std::string animation = will_explode_soon() ? "stopped_explosion_soon" : "stopped";
    get_sprite()->set_current_animation(animation);
  }
}

/**
 * @brief Makes the item sprite move.
 *
 * This function is called when the hero starts walking while carrying the item.
 * The item moves like him.
 */
void CarriedItem::set_animation_walking(void) {
  if (!is_lifting && !is_throwing) {
    std::string animation = will_explode_soon() ? "walking_explosion_soon" : "walking";
    get_sprite()->set_current_animation(animation);
  }
}

/**
 * @brief Throws the item.
 * @param direction direction where the hero throws the item (0 to 3)
 */
void CarriedItem::throw_item(int direction) {

  this->throwing_direction = direction;
  this->is_lifting = false;
  this->is_throwing = true;

  // play the sound
  game->play_sound("throw");

  // stop the sprite animation
  Sprite *sprite = get_sprite();
  sprite->set_current_animation("stopped");

  // set the movement of the item sprite
  set_y(hero->get_y());
  CollisionMovement *movement = new CollisionMovement();
  movement->set_speed(20);
  movement->set_direction(direction * 90);
  clear_movement();
  set_movement(movement);

  this->y_increment = -2;
  this->next_down_date = System::now() + 40;
  this->item_height = 18;
}

/**
 * @brief Returns whether the item is being thrown.
 * @return true if the item is being thrown
 */
bool CarriedItem::is_being_thrown(void) {
  return is_throwing;
}
 
/**
 * @brief Returns whether the item is about to explode.
 * @return true if the item is about to explode 
 */
bool CarriedItem::will_explode_soon(void) {
  return can_explode() && System::now() >= explosion_date - 1500;
}

/**
 * @brief Destroys the item when it is being thrown.
 */
void CarriedItem::break_item(void) {

  if (is_throwing && throwing_direction != 3) {
    // destroy the item where it is actually displayed
    set_y(get_y() - item_height);
  }

  movement->stop();

  if (!can_explode()) {
    game->play_sound(destruction_sound_id);
    get_sprite()->set_current_animation("destroy");
  }
  else {
    map->get_entities()->add_entity(new Explosion(get_layer(), get_xy(), true));
    if (is_throwing) {
      remove_from_map(); // because if the item was still carried by the hero, then the hero class will destroy it
    }
  }
  is_throwing = false;
  is_breaking = true;
}

/**
 * @brief Returns whether the item is broken.
 *
 * When this function returns true, the item can be deleted from memory.
 *
 * @return true if the item is broken
 */
bool CarriedItem::is_broken(void) {
  return is_breaking && (get_sprite()->is_animation_finished() || can_explode());
}

/**
 * @brief Returns whether the item can explode.
 * @return true if the item will explode
 */
bool CarriedItem::can_explode(void) {
  return explosion_date != 0;
}

/**
 * @brief This function is called by the map when the game is suspended or resumed.
 * @param suspended true to suspend the entity, false to resume it
 */
void CarriedItem::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended); // suspend the animation and the movement

  if (is_throwing) {
    // suspend the shadow
    shadow_sprite->set_suspended(suspended);
  }

  if (!suspended && when_suspended != 0) {
    // recalculate the timers
    if (is_throwing) {
      next_down_date += System::now() - when_suspended;
    }
    if (can_explode()) {
      explosion_date += System::now() - when_suspended;
    }
  }
}

/**
 * @brief This function is called repeatedly.
 */
void CarriedItem::update(void) {

  // update the sprite and the position
  MapEntity::update();

  if (suspended) {
    return;
  }

  // when the hero finishes lifting the item, start carrying it
  if (is_lifting && get_movement()->is_finished()) {
    is_lifting = false;

    // make the hero carry the item
    hero->start_carrying();

    // make the item follow the hero
    clear_movement();
    set_movement(new FollowMovement(hero, 0, -18, false));
  }

  // when the item has finished flying, destroy it
  else if (can_explode() && !is_breaking) {
    
    uint32_t now = System::now();
    
    if (now >= explosion_date) {
      break_item();
    }
    else if (will_explode_soon()) {

      std::string animation = get_sprite()->get_current_animation();
      if (animation == "stopped") {
	get_sprite()->set_current_animation("stopped_explosion_soon");
      }
      else if (animation == "walking") {
	get_sprite()->set_current_animation("walking_explosion_soon");
      }
    }
  }

  if (is_throwing) {
    shadow_sprite->update();

    if (is_broken()) {
      remove_from_map();
    }
    else if (break_on_intermediate_layer) {
      break_item();
      map->get_entities()->set_entity_layer(this, LAYER_INTERMEDIATE);
      break_on_intermediate_layer = false;
    }
    else if (movement->is_stopped() || y_increment >= 7) {
      break_item();
    }
    else {
      uint32_t now = System::now();
      while (now >= next_down_date) {
	next_down_date += 40;
	item_height -= y_increment;
	y_increment++;
      }
    }
  }
}

/**
 * @brief Displays the carried item on the map.
 *
 * This is a redefinition of MapEntity::display_on_map()
 * to display the shadow independently of the item movement.
 */
void CarriedItem::display_on_map(void) {

  if (!is_throwing) {
    // display the sprite normally
    MapEntity::display_on_map();
  }
  else {
    // when the item is being thrown, display the shadow and the item separately
    map->display_sprite(shadow_sprite, get_x(), get_y());
    map->display_sprite(get_sprite(), get_x(), get_y() - item_height);
  }
}

/**
 * @brief This function is called when this carried item collides an enemy.
 * @param enemy the enemy
 */
void CarriedItem::notify_collision_with_enemy(Enemy *enemy) {

  if (is_throwing && !can_explode()) {
    enemy->try_hurt(ATTACK_THROWN_ITEM, this, NULL);
  }
}

/**
 * @brief Notifies this entity that it has just attacked an enemy.
 *
 * This function is called even if this attack was not successful.
 *
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack:
 * - a number greater than 0 represents the number of health points the enemy has just lost
 * - a value of 0 means that the attack was just ignored 
 * - a value of -1 means that the enemy was protected against the attack
 * - a value of -2 means that the attack immobilized the enemy
 * @param killed indicates that the attack has just killed the enemy
 */
void CarriedItem::just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed) {

  if (result != 0) {
    break_item();
  }
}

/**
 * @brief Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool CarriedItem::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return false;
}

/**
 * @brief Returns whether a conveyor belt is currently considered as an obstacle for this entity.
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is currently an obstacle for this entity
 */
bool CarriedItem::is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt) {
  return false;
}

/**
 * @brief Returns whether some stairs are currently considered as an obstacle for this entity.
 * @param stairs an stairs entity
 * @return true if the stairs are currently an obstacle for this entity
 */
bool CarriedItem::is_stairs_obstacle(Stairs *stairs) {
  return false;
}

/**
 * @brief Returns whether a water tile is currently considered as an obstacle for this entity.
 * @return true if the water tiles are currently an obstacle for this entity
 */
bool CarriedItem::is_water_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a hole is currently considered as an obstacle for this entity.
 * @return true if the holes are currently an obstacle for this entity
 */
bool CarriedItem::is_hole_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a ladder is currently considered as an obstacle for this entity.
 * @return true if the ladders are currently an obstacle for this entity
 */
bool CarriedItem::is_ladder_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a raised crystal switch block is currently considered as an obstacle for this entity.
 * @param raised_block a crystal switch block raised
 * @return false 
 */
bool CarriedItem::is_raised_block_obstacle(CrystalSwitchBlock *raised_block) {
  // the thrown items can traverse the crystal switch blocks
  return false;
}

/**
 * @brief Returns whether a crystal switch is currently considered as an obstacle for this entity.
 * @param crystal_switch a crystal switch
 * @return true if the crystal switch is currently an obstacle for this entity
 */
bool CarriedItem::is_crystal_switch_obstacle(CrystalSwitch *crystal_switch) {
  return !is_being_thrown();
}

/**
 * @brief Returns whether a non-playing character is currently considered as an obstacle for this entity.
 * @param npc a non-playing character
 * @return true if the NPC is currently an obstacle for this entity
 */
bool CarriedItem::is_npc_obstacle(InteractiveEntity *npc) {
  return false;
}

/**
 * @brief Returns whether a jump sensor is currently considered as an obstacle for this entity.
 * @param jump_sensor a jump sensor
 * @return true if the jump sensor is currently an obstacle for this entity
 */
bool CarriedItem::is_jump_sensor_obstacle(JumpSensor *jump_sensor) {
  return false;
}

/**
 * @brief Returns whether a sensor is currently considered as an obstacle for this entity.
 * @param sensor a sensor
 * @return true if this sensor is currently an obstacle for this entity.
 */
bool CarriedItem::is_sensor_obstacle(Sensor *sensor) {
  return false;
}

/**
 * @brief Returns whether an enemy character is considered as an obstacle for this entity.
 * @param enemy an enemy
 * @return true if this enemy is considered as an obstacle for this entity.
 */
bool CarriedItem::is_enemy_obstacle(Enemy *enemy) {
  // if this item explodes when reaching an obstacle, then we consider enemies as obstacles
  return can_explode();
}

/**
 * @brief This function is called when some stairs detect a collision with this entity.
 * @param stairs the stairs entity
 * @param collision_mode the collision mode that detected the event
 */
void CarriedItem::notify_collision_with_stairs(Stairs *stairs, int collision_mode) {

  if (is_throwing && !is_breaking
      && stairs->is_inside_floor() && get_layer() == LAYER_LOW) {
    break_on_intermediate_layer = true; // show the destruction animation above the stairs
  }
}

