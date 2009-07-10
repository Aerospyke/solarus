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
#ifndef ZSDX_ENEMY_H
#define ZSDX_ENEMY_H

#include "Common.h"
#include "entities/Detector.h"
#include "entities/PickableItem.h"
#include "entities/EnemyAttack.h"

/**
 * Abstract class representing an enemy.
 * This class stores the attack and defense properties of the enemy.
 * The subclasses have to set these properties and create the enemy's sprites.
 * Every enemy sprite must have at least the following animations:
 * "walking", "hurt", "immobilized" and "shaking" with the four main directions.
 * The animation of its sprites
 * automically switches depending on its current movement and the attacks it is subject to.
 */
class Enemy: public Detector {

 public:

  /**
   * Types of enemies.
   */
  enum EnemyType {
    SIMPLE_GREEN_SOLDIER,
    BUBBLE,
    TENTACLE,
  };

  /**
   * Enemy ranks.
   */
  enum Rank {
    RANK_NORMAL,
    RANK_MINIBOSS,
    RANK_BOSS
  };

 protected:

  /**
   * Defines the sounds that can be played when an enemy is hurt.
   */
  enum HurtSoundStyle {
    HURT_SOUND_NORMAL,  /**< "enemy_hurt" is played */
    HURT_SOUND_MONSTER, /**< "monster_hurt" is played */
    HURT_SOUND_BOSS,    /**< "boss_hurt" is played */
  };

  /**
   * This structure contains the parameters needed by the subclasses constructors.
   */
  struct ConstructionParameters {
    std::string name;
    Layer layer;
    int x, y;
  };

  // attack/defense features of this type of enemy
  int damage_on_hero;                 /**< number of heart quarters the player loses when he gets hurt by this enemy;
				       * this number is divided depending on the hero's tunic number (default: 1) */
  int magic_damage_on_hero;           /**< number of magic points the player loses when he gets hurt by this enemy (default: 0) */
  int life;                           /**< number of health points of the enemy (default: 1) */
  HurtSoundStyle hurt_sound_style;    /**< the sound played when this kind of enemy gets hurt by the hero
				       * (default: HURT_SOUND_NORMAL) */
  bool pushed_back_when_hurt;         /**< indicates whether the enemy is pushed back when it gets hurt by the hero
				       * (default: true) */
  bool push_back_hero_on_sword;       /**< indicates whether the hero is pushed back when he hurts the enemy with his
				       * sword (default: false) */
  int minimum_shield_needed;          /**< shield number needed by the hero to avoid the attack of this enemy,
				       * or 0 to make the attack unavoidable (default: 0) */

  int vulnerabilities[ATTACK_NUMBER]; /**< indicates how the enemy reacts to each attack (default: depends on the attacks):
				       * - a number greater than 0 represents the number of health points lost when
				       * he is subject to this attack (for a sword attack, this number will be multiplied
				       * depending on the sword and the presence of a spin attack),
				       * - a value of 0 means that the attack is just ignored (this is the case
				       * for some special enemies like Octorok's stones),
				       * - a value of -1 means that the enemy is protected against this attack (the shield
				       * sound is played),
				       * - a value of -2 means that this attack immobilizes the enemy */

  // enemy characteristics
  Rank rank;                          /**< is this enemy a normal enemy, a miniboss or a boss? */
  int savegame_variable;              /**< index of the boolean variable indicating whether this enemy is killed,
				       * or -1 if it is not saved */

  // enemy state
  bool being_hurt;                    /**< indicates that the enemy is being hurt */
  Movement *normal_movement;          /**< backup of the enemy's movement, which is replaced by
				       * a straight movement while it is hurt */
  bool invulnerable;                  /**< indicates that the enemy cannot be hurt for now */
  Uint32 vulnerable_again_date;       /**< date when the enemy can be hurt again */
  bool can_attack;                    /**< indicates that the enemy can currently attack the hero */
  Uint32 can_attack_again_date;       /**< date when the enemy can attack again */
  bool immobilized;                   /**< indicates that the enemy is currently immobilized */
  Uint32 start_shaking_date;          /**< date when the enemy shakes */ 
  Uint32 end_shaking_date;            /**< date when the enemy stops shaking and walks again */ 

  // pickable item
  PickableItem::Subtype pickable_item_subtype;  /**< subtype of pickable item that appears when this enemy gets killed */
  int pickable_item_savegame_variable;          /**< savegame variable of the pickable item (if any) */

  // creation
  Enemy(const ConstructionParameters &params);
  virtual void initialize(void) = 0; // to initialize the features, the sprites and the movement
  virtual void restart(void);

  // functions available to the subclasses to define the enemy type properties (they can also change directly the fields)
  void set_damage(int damage_on_hero);
  void set_damage(int damage_on_hero, int magic_damage_on_hero);
  void set_life(int life);
  void set_features(int damage_on_hero, int life);
  void set_features(int damage_on_hero, int life, HurtSoundStyle hurt_sound_style);
  void set_features(int damage_on_hero, int life, HurtSoundStyle hurt_sound_style,
		    bool pushed_back_when_hurt, bool push_back_hero_on_sword, int minimum_shield_needed);
  void set_vulnerability(EnemyAttack attack, int reaction);

  // hurt the enemy
  Sound *get_hurt_sound(void);
  bool is_in_normal_state(void);
  bool is_being_hurt(void);
  bool is_immobilized(void);
  bool is_killed(void);
  void hurt(MapEntity *source);
  void immobilize(void);
  void stop_immobilized(void);

 public:

  // creation and destruction
  virtual ~Enemy(void);

  static Enemy *create(EnemyType type, Rank rank, int savegame_variable,
		       const std::string &name, Layer layer, int x, int y, int direction,
		       PickableItem::Subtype pickable_item_subtype, int pickable_item_savegame_variable);

  EntityType get_type(void);
  void set_map(Map *map);

  // obstacles
  bool is_obstacle_for(MapEntity *other);
  bool is_water_obstacle(void);
  bool is_hole_obstacle(void);
  bool is_teletransporter_obstacle(Teletransporter *teletransporter);

  // enemy state
  virtual void update(void);
  virtual void set_suspended(bool suspended);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
  void collision(MapEntity *entity, Sprite *sprite_overlapping);

  void attack_hero(Hero *hero);
  void attack_stopped_by_hero_shield(void);
  void try_hurt(EnemyAttack attack, MapEntity *source);
  void kill(void);
};

#endif

