#ifndef ZSDX_HERO_H
#define ZSDX_HERO_H

#include "Common.h"
#include "MapEntity.h"
#include "Controls.h"
#include "Enemy.h"

/**
 * The hero's entity.
 * It is animated and can be controlled with an 8 directions system.
 * This class handles the hero's actions: the movements and the animation of his sprites.
 */
class Hero: public MapEntity {

 public:

  /**
   * Possible states of the hero.
   * The state is considered only when the game is not suspended.
   * The hero can move (and walk on teletransporters) in states FREE, PUSHING, CARRYING, SWORD_LOADING and SWIMMING.
   * He can swing his sword in states FREE, PUSHING, CARRYING and SWORD_SWINGING.
   * He hero can be hurt in states <= SPIN_ATTACK.
   */
  enum State {
    FREE,                        /**< normal state (stopped or walking) */
    CARRYING,                    /**< the hero can walk but he is carrying a pot or a bush */
    SWORD_LOADING,               /**< the hero can walk but his sword is loading for a spin attack */
    SWIMMING,                    /**< the hero is swimming in deep water */
    PUSHING,                     /**< the hero is trying to push an obstacle */
    PULLING,                     /**< the hero is pulling an object */
    GRABBING,                    /**< the hero is grabbing an object and can pull it */
    SWORD_SWINGING,              /**< the hero is swinging his sword */
    SPIN_ATTACK,                 /**< the hero is releasing a spin attack */
    LIFTING,                     /**< the hero is lifting an destroyable item (a pot, a bush, etc.) */
    BRANDISHING_TREASURE,        /**< the hero is brandishing a treasure */
    JUMPING,                     /**< the hero is jumping */
    HURT,                        /**< the hero is hurt */
    PLUNGING,                    /**< the hero is plunging into water */
    DROWNING,                    /**< the hero is drowning */
    FREEZED,                     /**< the hero cannot move for various possible reasons */
  };

 private:

  // equipment
  Equipment *equipment;          /**< equipment of the player */

  Sprite *tunic_sprite;          /**< sprite of the current tunic */
  Sprite *sword_sprite;          /**< current sword sprite */
  Sprite *sword_stars_sprite;    /**< stars running along the sword when the sword is loading */
  Sprite *shield_sprite;         /**< current shield sprite */
  Sprite *shadow_sprite;         /**< shadow of the hero, only in state JUMPING (in other states,
			          * the shadow is with the tunic sprite) */
  Sprite *ground_sprite;         /**< ground displayed under the hero (e.g. grass or shallow water) */

  Sound *sword_sound;            /**< sound of the current sword */
  Sound *ground_sound;           /**< sound of the current ground displayed under the hero */

  static const SpriteAnimationSetId tunic_sprite_ids[];       /**< name of each tunic sprite */
  static const SpriteAnimationSetId sword_sprite_ids[];       /**< name of each sword sprite */
  static const SpriteAnimationSetId sword_stars_sprite_ids[]; /**< name of each sword stars sprite */
  static const SpriteAnimationSetId shield_sprite_ids[];      /**< name of each shield sprite */
  static const SpriteAnimationSetId ground_sprite_ids[];      /**< name of each shield sprite  */
  static const SoundId sword_sound_ids[];                     /**< name of each sword sound */
  static const SoundId ground_sound_ids[];                    /**< name of each ground sound */

  // movement
  PlayerMovement *normal_movement;
  static const int animation_directions[];                    /**< direction of the animation for each arrow key combination */
  bool move_tried;                                            /**< indicates that the hero just tried to move */
  int old_x;                                                  /**< x position of the hero before the last move try */
  int old_y;                                                  /**< y position of the hero before the last move try */

  // state
  State state;                   /**< current state of the hero (considered only when the game is not suspended) */
  Detector *facing_entity;       /**< the entity just in front of the hero */
  int animation_direction_saved; /**< direction of the hero's sprites, saved before
				  * showing a sprite animation having only one direction */
  Uint32 end_blink_date;         /**< when the hero's sprites stop blinking */

  int counter;                   /**< counter incremented every 100 ms in certain conditions:
				  * - in state FREE: counts for how long the hero is trying to walk
				  * to a wall (animation pushing is triggered at 800 ms)
				  * - in state SWORD_LOADING: counts for how long the hero is loading
				  * his sword (the spin attack is possible after 1000 ms) */

  Uint32 next_counter_date;      /**< when the counter will be incremented */

  // pushing
  Uint16 pushing_direction_mask; /**< direction of the hero's movement when pushing
				  * (0xFFFF indicates that he is currently not trying to push) */
  Detector *grabbed_entity;      /**< the entity the hero is pushing or pulling */

  // walking
  bool walking;                  /**< stopped or walking? (used in states FREE, PUSHING and CARRYING) */

  // spin attack
  bool sword_loaded;             /**< in state SWORD_LOADING, becomes true when the spin attack is possible */

  // lift and carry an object
  CarriedItem *lifted_item;      /**< item being lifted or carried */
  CarriedItem *thrown_item;      /**< item thrown and not broken yet */

  // brandish a treasure
  Treasure *treasure;            /**< the treasure being brandished (if any) */

  // jump
  int jump_y;                    /**< height of the hero's sprite when jumping, relative to its shadow on the ground */
  int last_ground_x;             /**< x coordinate of the last hero position on the ground (e.g. before jumping) */
  int last_ground_y;             /**< y coordinate of the last hero position on the ground (e.g. before jumping) */

  // special ground under the hero
  int ground;                    /**< kind of ground under the hero: grass, shallow water, etc. (from enum Map::Ground) */
  Uint32 next_ground_sound_date; /**< when the ground sound has to be played next time */

  // update functions
  void update_position(void);
  void update_sprites(void);
  void movement_just_changed(void);

  void try_snap_to_facing_entity(void);

  void action_key_pressed(void);
  void sword_key_pressed(void);
  void arrow_pressed(int direction);
  void arrow_released(int direction);

  // the hero's state
  void set_state(State state);

  void start_ground(void);
  void update_ground(void);
  bool is_ground_visible(void);

  void update_sword_swinging(void);

  void start_sword_loading(void);
  void update_sword_loading(void);
  void start_spin_attack(void);
  void update_spin_attack(void);
  bool can_start_sword(void);

  void stop_carrying(void);
  void start_throwing(void);
  void set_suspended_carried_items(bool suspended);
  void update_carried_items(void);
  void display_carried_items(void);
  void destroy_carried_items(void);

  void start_pushing(void);
  void update_pushing(void);
  void start_grabbing(void);
  void start_pulling(void);
  void update_grabbing_pulling(void);
  void update_moving_grabbed_entity(void);

  void update_treasure(void);
  void display_treasure(void);

  void update_jumping(void);
  bool can_be_hurt(void);
  void update_hurt(void);
  bool can_start_gameover_sequence(void);

  void start_deep_water(void);
  void start_plunging(void);
  void update_plunging(void);
  void update_drowning(void);
  void start_swimming(void);
  void stop_swimming(void);

  // animation of the sprites
  void set_animation_direction(int direction);
  bool is_direction_locked(void);
  void stop_displaying_sword(void);
  void blink(void);
  void stop_blinking(void);

  void set_animation_sword(void);
  void set_animation_walking(void);
  void set_animation_grabbing(void);
  void set_animation_pulling(void);
  void set_animation_pushing(void);
  void set_animation_lifting(void);
  void set_animation_jumping(void);
  void set_animation_hurt(void);
  void set_animation_plunging(void);

  void save_animation_direction(void);
  void restore_animation_direction(void);

  bool is_visible(void);
  bool is_sword_visible(void);
  bool is_sword_stars_visible(void);
  bool is_shield_visible(void);
  bool is_shadow_visible(void);

 public:

  // creation and destruction
  Hero(Equipment *equipment);
  ~Hero(void);

  // properties
  EntityType get_type(void);

  // movement
  PlayerMovement * get_normal_movement(void);
  int get_movement_direction(void);
  bool is_moving_towards(int direction);
  SDL_Rect get_facing_point(void);
  SDL_Rect get_facing_point(int direction);
  void just_moved(void);
  void set_facing_entity(Detector *detector);
  bool is_facing_obstacle(void);
  bool is_facing_point_in(const SDL_Rect *rectangle);

  // map
  void set_map(Map *map);
  void set_map(Map *map, int initial_direction);
  void update(void);
  void display_on_map(void);

  // sprites
  int get_animation_direction(void);
  void set_suspended(bool suspended);
  void rebuild_equipment(void);
  void restart_animation(void);
  void set_animation_stopped(void);

  // state of the hero
  State get_state(void);
  void set_ground(int ground);
  void start_free(void);
  void start_sword(void);
  bool is_grabbing_or_pulling(void);
  bool is_moving_grabbed_entity(void);
  void grabbed_entity_collision(void);
  void stop_moving_grabbed_entity(void);
  void start_lifting(DestructibleItem *item_to_lift);
  void start_carrying(void);
  void freeze(void);
  void unfreeze(void);
  void give_treasure(Treasure *treasure);
  void start_jumping(int direction, int length, bool with_collisions);
  void hurt(MapEntity *source, int life);
  void get_back_from_death(void);

  // keys
  void key_pressed(Controls::GameKey key);
  void key_released(Controls::GameKey key);

  // enemies and collisions
  bool is_obstacle_for(MapEntity *other);
  virtual void collision_with_enemy(Enemy *enemy);
  virtual void collision_with_enemy(Enemy *enemy, Sprite *sprite_overlapping);
  void just_attacked_enemy(Enemy::Attack attack, Enemy *victim);

  virtual void collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode);
  bool is_teletransporter_obstacle(Teletransporter *teletransporter);

};

#endif
