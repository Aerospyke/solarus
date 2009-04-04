#ifndef ZSDX_MAP_ENTITY_H
#define ZSDX_MAP_ENTITY_H

#include "Common.h"
#include <vector>

/**
 * Abstract class for all objects placed on a map: tiles,
 * enemies, the hero, interactive objects, etc.
 * Each entity has:
 * - a layer on the map
 * - a position on the map (a rectangle),
 * - an origin point, relative to the rectangle's top-left corner
 */
class MapEntity {

 public:

  /**
   * Constants to identify each kind of map entity.
   */
  enum EntityType {
    TILE                      = 0,    /**< a tile on the map, obstacle or not */
    DESTINATION_POINT         = 1,    /**< a destination point on the map */
    TELETRANSPORTER           = 2,    /**< a teletransporter: the hero is transported
				       * to a destination point when walking on it */
    PICKABLE_ITEM             = 3,    /**< an item that the hero can pick: a rupee, a heart, a fairy... */
    DESTRUCTIBLE_ITEM         = 4,    /**< an item that the hero can cut or lift: a pot, a bush, a stone... */
    CHEST                     = 5,    /**< a chest (small or big) with a treasure */
    JUMP_SENSOR               = 6,    /**< a sensor that makes the hero jump in a direction */
    ENEMY                     = 7,    /**< an enemy */
    INTERACTIVE_ENTITY        = 8,    /**< an entity the hero can interact with by pressing the action key
				       * in front of it (typically an NPC) */
    BLOCK                     = 9,    /**< a block or a statue that the hero can push */
    DYNAMIC_TILE              = 10,   /**< a dynamic tile (that can be enabled or disabled) */
    SWITCH                    = 11,   /**< a switch */
    CUSTOM_OBSTACLE           = 12,   /**< an obstacle for some other type of entities */

    HERO                      = 1000, /**< the hero */
    CARRIED_ITEM              = 1001, /**< item carried and thrown by the hero (comes from a destructible item) */
  };

  /**
   * Obstacle property for the tiles or the active objects.
   * Values lower than or equal to OBSTACLE_NONE correspond to entities the hero can walk on.
   * Values higher than OBSTACLE_NONE correspond to obstacles.
   */
  enum Obstacle {
    OBSTACLE_SHALLOW_WATER = -2, /**< the entity is some shallow water */
    OBSTACLE_DEEP_WATER = -1,    /**< the entity is some deep water */
    OBSTACLE_NONE = 0,           /**< the entity is normal normal ground without obstacle */
    OBSTACLE,                    /**< the entity is entirely an obstacle */
    OBSTACLE_TOP_RIGHT,          /**< the upper-right half of the entity is an obstacle */
    OBSTACLE_TOP_LEFT,           /**< the upper-left half of the entity is an obstacle */
    OBSTACLE_BOTTOM_LEFT,        /**< the lower-left half of the entity is an obstacle */
    OBSTACLE_BOTTOM_RIGHT,       /**< the lower-right half of the entity is an obstacle */
  };

  /**
   * Layer of a tile or an active object.
   */
  enum Layer {
    LAYER_LOW,             /**< the entity is always below the hero (floor, walls, chests,
			      enemies and 99% of the tiles and active objects) */
    LAYER_INTERMEDIATE,    /**< the hero can be below or above the entity (platforms or objects on a plaform) */
    LAYER_HIGH,            /**< the entity is always above the hero (trees, top of doors...) */
    LAYER_NB               /**< number of layers */
  };

 public:

  /**
   * Describes the features of each dynamic entity type:
   * is it an obstacle, can it detect collisions, etc.
   */
  struct EntityTypeFeatures {
    bool can_be_obstacle;         /**< Allows entities of this type to be obstacles for other entities.
				   * If enabled, the function is_obstacle_for() will be called
				   * to determine whether this is an obstacle or not. */
    bool can_detect_entities;     /**< Allows entities of this type to detect the presence 
				   * of the hero or other entities (this is possible only for
				   * suclasses of Detector). If enabled, the function 
				   * collision() will be called when a collision is detected. */
    bool can_be_displayed;        /**< Allows entities of this type to be displayed. 
				   * If enabled, the sprites added by the add_sprite() calls will be 
			 	   * displayed (if any). */
    bool is_displayed_in_y_order; /**< Allows an entity of this type to be displayed above
				   * the hero and other entities having this property when it is in front of them.
				   * This means that the displaying order of entities having this
				   * feature depends on their y position. The entities without this feature
				   * are displayed in the normal order (i.e. as specified by the map file), 
				   * and before the entities with the feature. */
  };

 private:

  /**
   * The features of each entity type.
   */
  static const EntityTypeFeatures entity_types_features[];

 protected:

  Map *map;                 /**< the map: this field is automatically set by class MapEntities after creating the entity */

  // position (mandatory for all kinds of entities)

  Layer layer;              /**< layer of the entity: LAYER_LOW, LAYER_INTERMEDIATE or LAYER_HIGH:
			     * the layer is constant for the tiles and can change for the hero and the enemies
			     */

  SDL_Rect position_in_map; /**< Position of the entity on the map. The position is defined as a rectangle.
			     * This rectangle represents the position of the entity of the map and is
			     * used for the collision tests. It can be different from the sprite's
			     * rectangle of the entity.
			     * For example, the hero's position is a 16*16 rectangle, but its sprite may be
			     * a 24*32 rectangle.
			     */

  SDL_Rect origin;          /**< Coordinates of the origin point of the entity,
			     * relative to the top-left corner of its rectangle.
			     * Remember that when you call get_x() and get_y(), you get the coordinates
			     * of the origin point on the map, not the coordinates of the rectangle's
			     * top-left corner.
			     * This is useful because the top-left corner of the entity's rectangle does
			     * not represent the actual entity's coordinates.
			     */

  // other data, used for some kinds of entities

  std::string name;             /**< name of the entity, not used for all kinds of entities;
		            * the name identifies the entity in the game (an empty string
		            * indicates that the entity has no name) */

  int direction;           /**< direction of the entity, not used for all kinds of entities;
			    * if the entity has a sprite, this direction is considered to be
			    * the direction of animation of the sprite */

  std::vector<Sprite*> sprites; /**< sprite(s) representing the entity, not used for all kinds of entities because
			    * some of them are invisible, and some of them handle their sprites themselves */
  Movement *movement;      /**< movement of the entity, not used for all kinds of entities;
			    * NULL indicates that the entity has no movement */

  // entity state

  bool suspended;          /**< indicates that the animation and movement of this entity are suspended */
  Uint32 when_suspended;   /**< indicates when this entity was suspended */
  bool being_removed;      /**< indicates that the entity is not valid anymore because it is about to be removed */

  // creation
  MapEntity(void);
  MapEntity(Layer layer, int x, int y, int width, int height);
  MapEntity(std::string name, int direction, Layer layer, int x, int y, int width, int height);

  // method called by the subclasses to set their properties
  void set_direction(int direction);
  void set_layer(Layer layer);
  void set_size(int width, int height);
  void set_size(SDL_Rect &size);
  void set_origin(int x, int y);
  void set_origin(const SDL_Rect *origin);
  void set_rectangle_from_sprite(void);
  void create_sprite(SpriteAnimationSetId id);
  void set_movement(Movement *movement);
  void clear_movement(void);

  // collisions
  static bool is_point_in(const SDL_Rect *rectangle, int x, int y);

 public:

  // destruction
  virtual ~MapEntity(void);
  void set_being_removed(void);
  bool is_being_removed(void);

  // entity type features
  virtual bool is_hero(void);
  virtual EntityType get_type(void) = 0;
  bool can_be_obstacle(void);
  bool can_detect_entities(void);
  bool can_be_displayed(void);
  bool is_displayed_in_y_order(void);

  // position in the map
  Layer get_layer(void);
  int get_x(void);
  int get_y(void);
  void set_x(int x);
  void set_y(int y);
  int get_width(void);
  int get_height(void);
  const SDL_Rect * get_position_in_map(void);
  void set_position_in_map(const SDL_Rect *position_in_map);
  void set_position_in_map(int x, int y);
  const SDL_Rect * get_origin(void);
  int get_top_left_x(void);
  int get_top_left_y(void);
  void set_top_left_x(int x);
  void set_top_left_y(int y);

  virtual SDL_Rect get_facing_point(void);
  virtual SDL_Rect get_facing_point(int direction);

  // properties
  virtual void set_map(Map *map);
  std::string get_name(void) const;
  int get_direction(void);

  // sprites
  Sprite * get_sprite(int index);
  Sprite * get_sprite(void);
  bool has_sprite(void);

  // movement
  Movement * get_movement(void);
  virtual void movement_just_changed(void);
  virtual void just_moved(void);
  virtual void set_facing_entity(Detector *detector);

  // collisions
  virtual bool is_obstacle_for(MapEntity *other);
  bool overlaps(const SDL_Rect *rectangle);
  bool is_origin_point_in(const SDL_Rect *rectangle);
  bool is_facing_point_in(const SDL_Rect *rectangle);
  bool is_center_in(const SDL_Rect *rectangle);
  double get_vector_angle(MapEntity *other);
  virtual void collision_with_enemy(Enemy *enemy);
  virtual void collision_with_enemy(Enemy *enemy, Sprite *sprite_overlapping);
  virtual void collision_with_teletransporter(Teletransporter *teletransporter, int collision_mode);

  // suspended
  bool is_suspended(void);
  virtual void set_suspended(bool suspended);

  // update and display
  virtual void update(void);
  virtual void display_on_map(void);
};

#endif
