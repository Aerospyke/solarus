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
#ifndef SOLARUS_MAP_ENTITY_H
#define SOLARUS_MAP_ENTITY_H

#include "Common.h"
#include "entities/EntityType.h"
#include "entities/Layer.h"
#include "entities/CollisionMode.h"
#include "entities/EnemyAttack.h"
#include "lowlevel/Rectangle.h"
#include <map>
#include <list>

/**
 * @brief Abstract class for all objects placed on a map.
 *
 * Example of entities include tiles, enemies, the hero,
 * interactive objects, doors, chests, etc.
 * Each entity has:
 * - a bounding box that represents its position on the map (a rectangle),
 * - a layer on the map
 * - an origin point, relative to the rectangle's top-left corner
 * Some entities can also have a name, a movement and some sprites.
 */
class MapEntity {

  public:

    typedef MapEntity* (CreationFunction)(Game &game, std::istream &is, Layer layer, int x, int y);	/**< a function to parse a certain type of entity */
    static CreationFunction* creation_functions[];			/**< the creation functions of all types of entities */
    static const Rectangle directions_to_xy_moves[8];			/**< converts a direction (0 to 7) into a one-pixel xy move */

    /**
     * @brief Describes the features of each type of entity.
     *
     * Describes the features of each dynamic entity type:
     * is it an obstacle, can it detect collisions, etc.
     */
    struct EntityTypeFeatures {
      bool can_be_obstacle; 					/**< Allows entities of this type to be obstacles for other entities.
								 * If enabled, the function is_obstacle_for() will be called
								 * to determine whether this is an obstacle or not. */
      bool can_detect_entities;					/**< Allows entities of this type to detect the presence 
								 * of the hero or other entities (this is possible only for
								 * suclasses of Detector). If enabled, the function 
								 * collision() will be called when a collision is detected. */
      bool can_be_displayed;					/**< Allows entities of this type to be displayed. 
								 * If enabled, the sprites added by the add_sprite() calls will be 
								 * displayed (if any). */
      bool is_displayed_in_y_order;				/**< Allows an entity of this type to be displayed above
								 * the hero and other entities having this property when it is in front of them.
								 * This means that the displaying order of entities having this
								 * feature depends on their y position. The entities without this feature
								 * are displayed in the normal order (i.e. as specified by the map file), 
								 * and before the entities with the feature. */
    };

  private:

    static const EntityTypeFeatures entity_types_features[];	/**< The features of each entity type stored in map files. */

    Map *map;							/**< The map where this entity is, or NULL
								 * (automatically set by class MapEntities after adding the entity to the map) */

  private:

    Layer layer;						/**< Layer of the entity: LAYER_LOW, LAYER_INTERMEDIATE or LAYER_HIGH.
								 * The layer is constant for the tiles and can change for the hero and the dynamic entities.
								 * The layer is private to make sure the set_layer() function is always called. */

  protected:

    Rectangle bounding_box;					/**< This rectangle represents the position of the entity of the map and is
								 * used for the collision tests. It corresponds to the bounding box of the entity.
								 * It can be different from the sprite's rectangle of the entity.
								 * For example, the hero's bounding box is a 16*16 rectangle, but its sprite may be
								 * a 24*32 rectangle. */

    Rectangle origin;						/**< Coordinates of the origin point of the entity,
								 * relative to the top-left corner of its rectangle.
								 * Remember that when you call get_x() and get_y(), you get the coordinates
								 * of the origin point on the map, not the coordinates of the rectangle's
								 * top-left corner.
								 * This is useful because the top-left corner of the entity's bounding box does
								 * not represent the actual entity's coordinates and does not match necessarily
								 * the sprite's rectangle. */

    // other data, used for some kinds of entities only

    std::string name;						/**< name of the entity, not used for all kinds of entities;
								 * the name identifies the entity in the game (an empty string
								 * indicates that the entity has no name) */

    int direction;						/**< direction of the entity, not used for all kinds of entities */

    std::map<std::string, Sprite*> sprites;			/**< sprite(s) representing the entity, indexed by their animation set id;
								 * note that some entities manage their sprites themselves rather than using this field */
    Sprite *first_sprite;					/**< the first sprite that was created into the sprites map,
								 * stored here because the map does not keep the order from which its elements are added */
    bool visible;						/**< indicates that this entity's sprites are currently displayed */
    Movement *movement;						/**< movement of the entity, not used for all kinds of entities;
								 * NULL indicates that the entity has no movement */
    Movement *old_movement;					/**< an old movement to destroy as soon as possible */

    // entity state

    bool suspended;						/**< indicates that the animation and movement of this entity are suspended */
    uint32_t when_suspended;					/**< indicates when this entity was suspended */
    bool being_removed;						/**< indicates that the entity is not valid anymore because it is about to be removed */

    // creation
    MapEntity();
    MapEntity(Layer layer, int x, int y, int width, int height);
    MapEntity(const std::string &name, int direction, Layer layer, int x, int y, int width, int height);

    // method called by the subclasses to set their properties
    void set_direction(int direction);
    void set_size(int width, int height);
    void set_size(const Rectangle &size);
    void set_origin(int x, int y);
    void set_origin(const Rectangle &origin);
    void set_bounding_box_from_sprite();
    void set_bounding_box(const Rectangle &bounding_box);
    void create_sprite(const SpriteAnimationSetId &id, bool enable_pixel_collisions = false);
    void set_movement(Movement *movement);
    void clear_movement();

    // easy access to various game objects
    MapEntities& get_entities();
    MapScript& get_map_script();
    Equipment& get_equipment();
    KeysEffect& get_keys_effect();
    GameControls& get_controls();
    DialogBox& get_dialog_box();
    Savegame& get_savegame();
    Hero& get_hero();

  public:

    // destruction
    virtual ~MapEntity();
    void remove_from_map();
    void notify_being_removed();
    bool is_being_removed();

    // entity type features
 
    /**
     * @brief Returns the type of entity.
     * @return the type of entity
     */
    virtual EntityType get_type() = 0;
    bool is_hero();
    virtual bool can_be_obstacle();
    virtual bool can_detect_entities();
    virtual bool can_be_displayed();
    virtual bool is_displayed_in_y_order();

    // adding to a map
    bool is_on_map();
    virtual void set_map(Map &map);
    Map& get_map();
    Game& get_game();

    // position in the map
    Layer get_layer();
    void set_layer(Layer layer);

    int get_x();
    int get_y();
    void set_x(int x);
    void set_y(int y);
    const Rectangle get_xy();
    void set_xy(const Rectangle &xy);
    void set_xy(int x, int y);

    int get_width();
    int get_height();
    const Rectangle& get_bounding_box();
    const Rectangle& get_origin();
    int get_top_left_x();
    int get_top_left_y();
    void set_top_left_x(int x);
    void set_top_left_y(int y);
    void set_top_left_xy(int x, int y);

    virtual const Rectangle get_facing_point();
    virtual const Rectangle get_facing_point(int direction);
    const Rectangle get_center_point();

    bool is_aligned_to_grid();
    bool is_x_aligned_to_grid();
    bool is_y_aligned_to_grid();
    void set_aligned_to_grid();

    // properties
    const std::string& get_name() const;
    bool has_prefix(const std::string &prefix);
    int get_direction();

    // sprites
    Sprite& get_sprite(const SpriteAnimationSetId &id);
    Sprite& get_sprite();
    int get_nb_sprites();
    bool has_sprite();
    void remove_sprite(const SpriteAnimationSetId &id);
    void remove_sprites();
    virtual bool is_visible();
    virtual void set_visible(bool visible);
    void set_animation_ignore_suspend(bool ignore_suspend);
    void start_fading(int direction);

    // movement
    Movement* get_movement();
    virtual void notify_movement_tried(bool success);
    virtual void notify_position_changed();
    virtual void notify_layer_changed();
    virtual void notify_movement_changed();
    virtual void set_facing_entity(Detector *detector);
    static const Rectangle& direction_to_xy_move(int direction8);

    // geometry
    bool overlaps(const Rectangle &rectangle);
    bool overlaps(int x, int y);
    bool overlaps(MapEntity &other);
    bool is_origin_point_in(const Rectangle &rectangle);
    bool is_facing_point_in(const Rectangle &rectangle);
    bool is_center_in(const Rectangle &rectangle);

    double get_vector_angle(MapEntity &other);
    int get_distance(MapEntity &other);
    int get_distance(int x, int y);

    // collisions
    virtual bool is_obstacle_for(MapEntity &other);
    virtual bool has_layer_independent_collisions();
    virtual void notify_collision_with_enemy(Enemy &enemy);
    virtual void notify_collision_with_destructible_item(DestructibleItem &destructible_item, CollisionMode collision_mode);
    virtual void notify_collision_with_teletransporter(Teletransporter &teletransporter, CollisionMode collision_mode);
    virtual void notify_collision_with_conveyor_belt(ConveyorBelt &conveyor_belt, int dx, int dy);
    virtual void notify_collision_with_stairs(Stairs &stairs, CollisionMode collision_mode);
    virtual void notify_collision_with_jump_sensor(JumpSensor &jump_sensor);
    virtual void notify_collision_with_sensor(Sensor &sensor);
    virtual void notify_collision_with_explosion(Explosion &explosion, Sprite &sprite_overlapping);
    virtual void notify_collision_with_crystal_switch(CrystalSwitch &crystal_switch, CollisionMode collision_mode);
    virtual void notify_collision_with_crystal_switch(CrystalSwitch &crystal_switch, Sprite &sprite_overlapping);
    virtual void notify_collision_with_enemy(Enemy &enemy, Sprite &enemy_sprite, Sprite &this_sprite);
    virtual void notify_attacked_enemy(EnemyAttack attack, Enemy &victim, int result, bool killed);

    virtual bool is_water_obstacle();
    virtual bool is_hole_obstacle();
    virtual bool is_ladder_obstacle();
    virtual bool is_hero_obstacle(Hero &hero);
    virtual bool is_block_obstacle(Block &block);
    virtual bool is_teletransporter_obstacle(Teletransporter &teletransporter);
    virtual bool is_conveyor_belt_obstacle(ConveyorBelt &conveyor_belt);
    virtual bool is_stairs_obstacle(Stairs &stairs);
    virtual bool is_sensor_obstacle(Sensor &sensor);
    virtual bool is_raised_block_obstacle(CrystalSwitchBlock &raised_block);
    virtual bool is_crystal_switch_obstacle(CrystalSwitch &crystal_switch);
    virtual bool is_npc_obstacle(InteractiveEntity &npc);
    virtual bool is_enemy_obstacle(Enemy &enemy);
    virtual bool is_jump_sensor_obstacle(JumpSensor &jump_sensor);
    virtual bool is_destructible_item_obstacle(DestructibleItem &destructible_item);
    virtual bool is_sword_ignored();

    // suspended
    bool is_suspended();
    virtual void set_suspended(bool suspended);

    // update and display
    virtual void update();
    virtual void display_on_map();
};

#endif

