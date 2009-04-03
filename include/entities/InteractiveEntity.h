#ifndef ZSDX_INTERACTIVE_ENTITY_H
#define ZSDX_INTERACTIVE_ENTITY_H

#include "Common.h"
#include "entities/Detector.h"
#include "KeysEffect.h"

/**
 * An interactive entity is an entity that triggers a message or an event
 * when the player presses the action key while facing it.
 *
 * Note that complex entities such as chests, pots and doors are not included
 * in this class because they have additional properties (e.g. some content
 * or some variables saved).
 */
class InteractiveEntity: public Detector {

 public:

  /**
   * The different kinds of interactions.
   * They indicate what happens when the player presses the action key in front of this entity.
   */
  enum SpecialInteraction {

    CUSTOM,                /**< no predetermined behavior: just displays the message specified or calls the script */
    NON_PLAYING_CHARACTER, /**< same thing except that the script can make the engine move an NPC */
    SIGN,                  /**< a sign with a message (TODO to make it destructible,
			    * an instance of Sign is actually created) */
    WATER_FOR_BOTTLE,      /**< a place where the hero can fill a bottle with water */
  };

 private:

  SpecialInteraction special_interaction;
  MessageId message_to_show;

  static const KeysEffect::ActionKeyEffect action_key_effects[];
  static const int animation_directions[];

  void initialize_sprite(SpriteAnimationSetId sprite_name, int initial_direction);
  void call_script(void);

 public:

  InteractiveEntity(std::string name, Layer layer, int x, int y, SpecialInteraction special_interaction,
		    SpriteAnimationSetId sprite_name, int initial_direction, MessageId message_to_show);
  ~InteractiveEntity(void);

  EntityType get_type(void);

  bool is_obstacle_for(MapEntity *other);
  bool is_teletransporter_obstacle(Teletransporter *teletransporter);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
  void action_key_pressed(void);

  void update(void);
  void walk(std::string path, bool loop, bool with_collisions);
  void walk_random(void);
  void jump(int dx, int dy, bool loop, bool with_collisions);
  void just_moved(void);
  void set_sprite_direction(int direction);
};

#endif
