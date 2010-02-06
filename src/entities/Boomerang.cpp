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
#include "entities/Boomerang.h"
#include "entities/Hero.h"
#include "entities/Enemy.h"
#include "entities/MapEntities.h"
#include "movements/CollisionMovement.h"
#include "movements/PlayerMovement.h"
#include "movements/TargetMovement.h"
#include "Game.h"
#include "Map.h"
#include "lowlevel/System.h"

/**
 * Creates a boomerang.
 * @param hero the hero
 * @param boomerang_direction the boomerang direction (0 to 360)
 */
Boomerang::Boomerang(Hero *hero, int boomerang_direction):
  MapEntity(), hero(hero), has_to_go_back(false), going_back(false) {

  // initialize the entity
  set_layer(hero->get_layer());
  create_sprite("entities/boomerang");
  set_bounding_box_from_sprite();

  int hero_x = hero->get_top_left_x();
  int hero_y = hero->get_top_left_y();
  switch (hero->get_animation_direction()) {

    case 0:
      set_xy(hero_x + 24, hero_y + 8);
      break;

    case 1:
      set_xy(hero_x + 8, hero_y - 8);
      break;

    case 2:
      set_xy(hero_x - 8, hero_y + 8);
      break;

    case 3:
      set_xy(hero_x + 8, hero_y + 24);
      break;

  }

  initial_coords.set_xy(get_xy());

  CollisionMovement *movement = new CollisionMovement();
  movement->set_speed(16);
  movement->set_direction(boomerang_direction);
  set_movement(movement);

  next_sound_date = System::now();
}

/**
 * Destructor.
 */
Boomerang::~Boomerang(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Boomerang::get_type() {
  return BOOMERANG;
}

/**
 * Returns whether entities of this type can be obstacles for other entities.
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 * @return true if this type of entity can be obstacle for other entities
 */
bool Boomerang::can_be_obstacle(void) {
  return false; 
}

/**
 * Returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * notify_collision() will be called when a collision is detected.
 * @return true if this type of entity can detect other entities
 */
bool Boomerang::can_detect_entities(void) {
  return false;
}

/**
 * Returns whether entities of this type can be displayed.
 * If enabled, the sprites added by the add_sprite() calls will be 
 * displayed (if any).
 * @return true if this type of entity can be displayed
 */
bool Boomerang::can_be_displayed(void) {
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
bool Boomerang::is_displayed_in_y_order(void) {
  return false;
}

/**
 * Returns whether a teletransporter is currently considered as an obstacle for this entity.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is currently an obstacle for this entity
 */
bool Boomerang::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return false;
}

/**
 * Returns whether a conveyor belt is currently considered as an obstacle for this entity.
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is currently an obstacle for this entity
 */
bool Boomerang::is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt) {
  return false;
}

/**
 * Returns whether a water tile is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @return true if the water tiles are currently an obstacle for this entity
 */
bool Boomerang::is_water_obstacle(void) {
  return false;
}

/**
 * Returns whether a hole is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @return true if the holes are currently an obstacle for this entity
 */
bool Boomerang::is_hole_obstacle(void) {
  return false;
}

/**
 * Returns whether a ladder is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @return true if the ladders are currently an obstacle for this entity
 */
bool Boomerang::is_ladder_obstacle(void) {
  return false;
}

/**
 * Returns whether a raised crystal switch block is currently considered as an obstacle for this entity.
 * @param raised_block a crystal switch block raised
 * @return false 
 */
bool Boomerang::is_raised_block_obstacle(CrystalSwitchBlock *raised_block) {
  // the boomerang can traverse the crystal switch blocks
  return false;
}

/**
 * Returns whether a crystal switch is currently considered as an obstacle for this entity.
 * @param crystal_switch a crystal switch
 * @return true if the crystal switch is currently an obstacle for this entity
 */
bool Boomerang::is_crystal_switch_obstacle(CrystalSwitch *crystal_switch) {
  return false;
}

/**
 * Returns whether a non-playing character is currently considered as an obstacle for this entity.
 * @param npc a non-playing character
 * @return true if the NPC is currently an obstacle for this entity
 */
bool Boomerang::is_npc_obstacle(InteractiveEntity *npc) {
  return false;
}

/**
 * Returns whether a jump sensor is currently considered as an obstacle for this entity.
 * This function returns true by default.
 * @param jump_sensor a jump sensor
 * @return true if the jump sensor is currently an obstacle for this entity
 */
bool Boomerang::is_jump_sensor_obstacle(JumpSensor *jump_sensor) {
  return false;
}

/**
 * Returns whether the boomerang is going back towards the hero, i.e. if go_back() has been called.
 * @return true if the boomerang is going back
 */
bool Boomerang::is_going_back(void) {
  return has_to_go_back || going_back;
}

/**
 * Makes the boomerang go back towards the hero.
 */
void Boomerang::go_back(void) {

  if (is_going_back()) {
    DIE("The boomerang is already going back");
  }

  has_to_go_back = true;
}

/**
 * Updates the boomerang.
 */
void Boomerang::update(void) {

  MapEntity::update();

  if (suspended) {
    return;
  }

  uint32_t now = System::now();
  if (now >= next_sound_date) {
    game->play_sound("boomerang");
    next_sound_date = now + 150;
  }

  if (!going_back) {

    if (has_to_go_back) {
      going_back = true;
      clear_movement();
      set_movement(new TargetMovement(hero, 16));
      map->get_entities()->set_entity_layer(this, hero->get_layer()); // because the hero's layer may have changed
    }
    else if (get_movement()->is_stopped()) {
      // collision with an obstacle
      
      if (!map->test_collision_with_border(get_movement()->get_last_collision_box_on_obstacle())) {
        // play a sound unless we are on the map border
	game->play_sound("sword_tapping");
      }
      go_back();
    }
    else if (get_distance(initial_coords.get_x(), initial_coords.get_y()) >= 144) {
      go_back();
    }
  }
  else if (get_movement()->is_finished()) {
    remove_from_map();
  }
}

/**
 * This function is called when an enemy collides with the entity.
 * @param enemy the enemy
 */
void Boomerang::notify_collision_with_enemy(Enemy *enemy) {

  if (!overlaps(hero)) {
    enemy->try_hurt(ATTACK_BOOMERANG, this, NULL);
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
 * @param killed indicates that the attack has just killed the enemy
 */
void Boomerang::just_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool dead) {

  if (result != 0 && !is_going_back()) {
    go_back();
  }
}

