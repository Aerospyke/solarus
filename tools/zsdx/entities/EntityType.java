package zsdx.entities;

import java.util.*;

/**
 * Represents the different types of map entities available in the editor.
 */
public enum EntityType {

    TILE              (0, "Tile", TileOnMap.class, null),
    DESTINATION_POINT (1, "Destination point", DestinationPoint.class, null),
    /*
    TELETRANSPORTER   (2, "Teletransporter", Teletransporter.class, Teletransporter.Subtype.class),
    PICKABLE_ITEM     (3, "Pickable item", PickableItem.class, PickableItem.Subtype.class),
    DESTRUCTIBLE_ITEM (4, "Destructible item", DestructibleItem.class, DestructibleItem.Subtype.class),
    */
    CHEST             (5, "Chest", Chest.class, null),
    JUMP_SENSOR       (6, "Jump Sensor", JumpSensor.class, null),
    ENEMY             (7, "Enemy", Enemy.class, Enemy.Subtype.class),
    INTERACTIVE       (8, "Interactive entity", InteractiveEntity.class, InteractiveEntity.Subtype.class);

    private final int index;
    private Class<? extends MapEntity> entityClass;
    private Class<? extends Enum> subtypeEnum;
    private String name;

    /**
     * Creates an entity type.
     * @param index index of the entity type to create
     * @param name a human readable name describing this entity type
     * @param entityClass subclass of MapEntity representing the entities of this type
     * @param subtypeEnum enumeration describing the subtypes of this type (or null
     * if there is no notion of subtype)
     */
    private EntityType(int index, String name, Class<? extends MapEntity> entityClass,
	    Class<? extends Enum> subtypeEnum) {
	this.index = index;
	this.name = name;
	this.entityClass = entityClass;
	this.subtypeEnum = subtypeEnum;
    }

    /**
     * Returns the entity type with the specified index.
     * @param index index of the entity type to get
     * @return the entity type with this index
     * @throws NoSuchElementException if the index is incorrect
     */
    public static EntityType get(int index) throws NoSuchElementException {

	for (EntityType t: values()) {
	    if (t.getIndex() == index) {
		return t;
	    }
	}

	throw new NoSuchElementException("Unknown entity type: " + index);
    }

    /**
     * Returns the entity type with the specified class.
     * @param entityClass class of the entity type to get
     * @return the entity type represented by this class
     * @throws NoSuchElementException if the class is incorrect
     */
    public static EntityType get(Class<? extends MapEntity> entityClass) throws NoSuchElementException {

	for (EntityType t: values()) {
	    if (t.getEntityClass() == entityClass) {
		return t;
	    }
	}

	throw new NoSuchElementException("Unknown entity class: " + entityClass.getName());
    }

    /**
     * Returns the index of this entity type.
     * @return the index
     */
    public int getIndex() {
	return index;
    }

    /**
     * Returns the name of this kind of entity.
     * @return the name of this kind of entity
     */
    public String getName() {
	return name;
    }

    /**
     * Returns the subclass of MapEntity representing this type of entity.
     * @return the entity class
     */
    public Class<? extends MapEntity> getEntityClass() {
	return entityClass;
    }

    /**
     * Returns the enumeration describing the subtype of this kind of entity.
     * @return the enumeration describing the subtype of this kind of entity
     */
    public Class<? extends Enum> getSubtypeEnum() {
	return subtypeEnum;
    }
};
