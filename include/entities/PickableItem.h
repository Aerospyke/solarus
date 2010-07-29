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
#ifndef SOLARUS_PICKABLE_ITEM_H
#define SOLARUS_PICKABLE_ITEM_H

#include "Common.h"
#include "entities/Detector.h"
#include "movements/FallingHeight.h"

/**
 * @brief An item that the hero can take.
 *
 * Example of pickable items include rupees, hearts, bombs, fairies, etc.
 */
class PickableItem: public Detector {

  public:

    /**
     * @brief Subtypes of pickable items.
     */
    enum Subtype {

      // special values
      RANDOM          = -1,   /**< special value to indicate to choose another value randomly (including NONE) */
      NONE            = 0,    /**< special value to indicate that there is no pickable item */

      // items not necessarily saved
      RUPEE_1         = 1,
      RUPEE_5         = 2,
      RUPEE_20        = 3,
      HEART           = 4,
      SMALL_MAGIC     = 5,
      BIG_MAGIC       = 6,
      FAIRY           = 7,
      BOMB_1          = 8,
      BOMB_5          = 9,
      BOMB_10         = 10,
      ARROW_1         = 11,
      ARROW_5         = 12,
      ARROW_10        = 13,

      // items always saved
      SMALL_KEY       = 14,
      BIG_KEY         = 15,
      BOSS_KEY        = 16,
      PIECE_OF_HEART  = 17,
      HEART_CONTAINER = 18
    };

  protected:

    /**
     * @brief Defines the features of a pickable item subtype.
     */
    struct Features {
      SpriteAnimationSetId animation_set_id; /**< animation set used for this subtype of pickable item */
      std::string animation_name;            /**< name of the animation */
      bool big_shadow;                       /**< true if the pickable item has a big shadow, false for a small shadow */
      SoundId sound;                         /**< the sound played when the player gets the item */
      bool can_disappear;                    /**< indicates that the item can disappear after a delay */
      bool must_be_saved;                    /**< indicates that the item must be saved */
      bool must_be_in_dungeon;               /**< indicates that the item can exist only in a dungeon */
    };

    static const Features features[];

    Subtype subtype;                         /**< subtype of pickable item */
    int savegame_variable;                   /**< savegame variable of the possession state of this item,
					      * for certain kinds of items only: a key, a piece of heart... */

    Sprite *shadow_sprite;                   /**< sprite of the shadow (except for a fairy). */
    FallingHeight falling_height;            /**< indicates whether the item is falling when it appears (except for a fairy) */
    bool will_disappear;                     /**< indicates whether the item will disappear after an amount of time
				              * (only possible for items not saved) */

    // current state
    int shadow_x;                            /**< x coordinate of the shadow (which does not move while the item does) */
    int shadow_y;                            /**< x coordinate of the shadow (which does not move while the item does) */

    uint32_t appear_date;                    /**< date when the item is created */
    uint32_t allow_pick_date;                /**< date when the item can be picked */
    bool can_be_picked;                      /**< indicates that the item can be picked now (i.e. allow_picked_date is past) */
    uint32_t blink_date;                     /**< date when the item starts blinking */
    uint32_t disappear_date;                 /**< date when the item disappears */
    bool is_following_boomerang;             /**< true if this item is currently attached to the boomerang */

    // creation and initialization
    PickableItem(Layer layer, int x, int y, Subtype subtype, int savegame_variable);

    static Subtype choose_random_subtype(Equipment *equipment);
    static bool is_subtype_locked(Subtype subtype, Equipment *equipment);
    virtual void initialize_sprites(void);
    virtual void initialize_movement(void);

    // falling
    bool is_falling(void);

    // item
    virtual void give_item_to_player(void);
    void set_blinking(bool blinking);

  public:

    // creation and destruction
    static PickableItem * create(Game *game, Layer layer, int x, int y, Subtype subtype, int savegame_variable,
	FallingHeight falling_height, bool will_disappear);

    virtual ~PickableItem(void);
    static CreationFunction parse;

    EntityType get_type(void);

    // item state
    static bool can_disappear(Subtype subtype);
    static bool can_be_saved(Subtype subtype);
    static bool must_be_saved(Subtype subtype);
    virtual void set_suspended(bool suspended);
    void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
    virtual void update(void);
    virtual void display_on_map(void);
};

#endif

