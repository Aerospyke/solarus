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
#ifndef SOLARUS_ENEMY_H
#define SOLARUS_ENEMY_H

#include "Common.h"
#include "Treasure.h"
#include "entities/Detector.h"
#include "entities/EnemyAttack.h"
#include "entities/Explosion.h"

/**
 * @brief Abstract class representing an enemy.
 *
 * This class stores the attack and defense properties of the enemy.
 * The subclasses have to set these properties and create the enemy's sprites.
 * Every enemy sprite must have at least the following animations
 * and with the four main directions:
 * - "walking": used when the enemy is in its normal state
 * - "hurt": used when it was just hurt
 * If the enemy can be immobilized (i.e. it is sensible to attacks like the boomerang),
 * the following animations are also required:
 * - "immobilized": used while the enemy is immobilized
 * - "shaking": used when the immobilized enemy is about to get back to its normal state.
 * The animation of its sprites
 * automically switches between those animations 
 * depending on its current movement and the attacks it is subject to.
 * Additional animations may be defined by the subclasses.
 */
class Enemy: public Detector {

  public:

    /**
     * @brief Subtypes of enemies.
     */
    enum Subtype {
      CUSTOM = -1,
      SIMPLE_GREEN_SOLDIER = 0,
      BUBBLE,
      TENTACLE,
      MINILLOSAUR,
      CHAIN_AND_BALL,

      PAPILLOSAUR_KING = 1000,
      KHORNETH,
      KHOTOR
    };

    /**
     * @brief Enemy ranks.
     */
    enum Rank {
      RANK_NORMAL,
      RANK_MINIBOSS,
      RANK_BOSS
    };

  protected:

    /**
     * @brief Defines the sounds that can be played when an enemy is hurt.
     */
    enum HurtSoundStyle {
      HURT_SOUND_NORMAL,			/**< "enemy_hurt" (and if necessary "enemy_killed") is played */
      HURT_SOUND_MONSTER,			/**< "monster_hurt" (and if necessary "enemy_killed") is played */
      HURT_SOUND_BOSS				/**< "boss_hurt" or "boss_killed" is played */
    };

    /**
     * @brief This structure contains the parameters needed by the subclasses constructors.
     */
    struct ConstructionParameters {
      Game &game;				/**< the game */
      const Treasure &treasure;			/**< the treasure dropped by the enemy */
      std::string name;				/**< name of the instance of enemy */
      Layer layer;				/**< initial layer of the enemy */
      int x;					/**< initial x coordinate of the enemy */
      int y;					/**< initial y coordinate of the enemy */
    };

  private:

    // attack/defense features of this type of enemy
    int damage_on_hero;					/**< number of heart quarters the player loses when he gets hurt by this enemy;
							 * this number is divided depending on the hero's tunic number (default: 1) */
    int magic_damage_on_hero;				/**< number of magic points the player loses when he gets hurt
							 * by this enemy (default: 0) */
    int life;						/**< number of health points of the enemy (default: 1) */
    HurtSoundStyle hurt_sound_style;			/**< the sound played when this kind of enemy gets hurt by the hero
							 * (default: HURT_SOUND_NORMAL) */
    bool pushed_back_when_hurt;				/**< indicates whether the enemy is pushed back when it gets hurt by the hero
							 * (default: true) */
    bool push_back_hero_on_sword;			/**< indicates whether the hero is pushed back when he hurts the enemy with his
							 * sword (default: false) */
    int minimum_shield_needed;				/**< shield number needed by the hero to avoid the attack of this enemy,
							 * or 0 to make the attack unavoidable (default: 0) */

    int attack_consequences[ATTACK_NUMBER];		/**< indicates how the enemy reacts to each attack
							 * (by default, it depends on the attacks):
							 * - a number greater than 0 represents the number of health points lost when
							 *   he is subject to this attack
							 *     - for a sword attack, this number is multiplied depending on
							 *       the sword strongness and the presence of a spin attack
							 *     - for a thrown item, this number is multiplied by the weight
							 * - a value of 0 means that the attack is just ignored (this is the case
							 *   for some special enemies like Octorok's stones),
							 * - a value of -1 means that the enemy is protected against this attack (a
							 *   sound is played),
							 * - a value of -2 means that this attack immobilizes the enemy
							 * - a value of -3 means a custom effect for the attack
							 *   (the custom_attack() fonction is called) */

    // enemy characteristics
    Rank rank;						/**< is this enemy a normal enemy, a miniboss or a boss? */
    int savegame_variable;				/**< index of the boolean variable indicating whether this enemy is killed,
							 * or -1 if it is not saved */

    // enemy state
    bool enabled;					/**< indicates that the enemy is enabled */
    bool being_hurt;					/**< indicates that the enemy is being hurt */
    uint32_t stop_hurt_date;				/**< date when the enemy stops being hurt */
    Movement *normal_movement;				/**< backup of the enemy's movement, which is replaced by
							 * a straight movement while it is hurt */
    bool invulnerable;					/**< indicates that the enemy cannot be hurt for now */
    uint32_t vulnerable_again_date;			/**< date when the enemy can be hurt again */
    bool can_attack;					/**< indicates that the enemy can currently attack the hero */
    uint32_t can_attack_again_date;			/**< date when the enemy can attack again */
    bool immobilized;					/**< indicates that the enemy is currently immobilized */
    uint32_t start_shaking_date;			/**< date when the enemy shakes */ 
    uint32_t end_shaking_date;				/**< date when the enemy stops shaking and walks again */ 

    // treasure
    Treasure treasure;					/**< pickable item that appears when this enemy gets killed */

    // boss or mini-boss
    bool exploding;					/**< indicates that the boss is dying and some explosions are triggered on him */
    int nb_explosions;					/**< number of explosions already played */
    uint32_t next_explosion_date;			/**< date of the next explosion */

  protected:

    // creation
    Enemy(const ConstructionParameters &params);

    /**
     * @brief Initializes the features, the sprites and the movement.
     */
    virtual void initialize() = 0;
    virtual void restart();

    void stop_movement();
    void restore_movement();

    // functions available to the subclasses to define the enemy type properties (they can also change directly the fields)
    void set_damage(int damage_on_hero);
    void set_damage(int damage_on_hero, int magic_damage_on_hero);
    void set_life(int life);
    int get_life();
    void set_pushed_back_when_hurt(bool pushed_back_when_hurt);
    void set_push_back_hero_on_sword(bool push_back_hero_on_sword);
    void set_features(int damage_on_hero, int life);
    void set_features(int damage_on_hero, int life, HurtSoundStyle hurt_sound_style);
    void set_features(int damage_on_hero, int life, HurtSoundStyle hurt_sound_style,
	bool pushed_back_when_hurt, bool push_back_hero_on_sword, int minimum_shield_needed);
    void set_attack_consequence(EnemyAttack attack, int consequence);
    void set_no_attack_consequences();
    void set_default_attack_consequences();

    // hurt the enemy
    void play_hurt_sound();
    bool is_in_normal_state();
    bool is_being_hurt();
    bool is_immobilized();
    bool is_killed();
    bool is_dying_animation_finished();
    void hurt(MapEntity &source);
    bool is_sprite_finished_or_looping();
    void immobilize();
    void stop_immobilized();
    virtual int custom_attack(EnemyAttack attack, Sprite *this_sprite);
    virtual void just_hurt(MapEntity &source, EnemyAttack attack, int life_points);
    virtual void just_dead();

    // animation
    const std::string& get_animation();
    void set_animation(const std::string &animation);

  public:

    // creation and destruction
    virtual ~Enemy();

    static CreationFunction parse;
    static MapEntity* create(Game& game, Subtype subtype,
	const std::string& breed, Rank rank, int savegame_variable,
	const std::string& name, Layer layer, int x, int y, int direction,
	const Treasure& treasure);

    EntityType get_type();
    void set_map(Map &map);
    Rank get_rank();

    // obstacles
    bool is_obstacle_for(MapEntity &other);
    bool is_sensor_obstacle(Sensor &sensor);
    bool is_destructible_item_obstacle(DestructibleItem &destructible_item);

    // enemy state
    virtual void update();
    virtual void set_suspended(bool suspended);
    void notify_enabled(bool enabled);
    void notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode);
    void notify_collision(MapEntity &other_entity, Sprite &other_sprite, Sprite &this_sprite);
    void notify_collision_with_explosion(Explosion &explosion, Sprite &sprite_overlapping);

    // attack the hero
    void attack_hero(Hero &hero, Sprite *this_sprite);
    void attack_stopped_by_hero_shield();

    // be subject to an attack
    int get_attack_consequence(EnemyAttack attack);
    virtual int get_attack_consequence(EnemyAttack attack, Sprite *this_sprite);
    void try_hurt(EnemyAttack attack, MapEntity &source, Sprite *this_sprite);
    void kill();
    bool is_dying();
};

#endif

