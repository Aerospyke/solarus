package zsdx.entities;

import java.awt.*;
import java.lang.reflect.*;
import java.util.*;

import zsdx.*;
import zsdx.Map;

/**
 * Represents an entity placed on the map with the map editor,
 * and how the entity is placed on the map: its position and its layer.
 * 
 * To create a new kind of entity, you should do the following steps:
 * - Add your new type of entity into the EntityType enumeration.
 * - Add the entity in the AddEntitiesToolbar.cells array.
 * - Add the entity in the AddEntitiesMenu class.
 * - Create a class for your entity and:
 *   - If your entity has a notion of subtype, create a Subtype enumeration
 *       that implements the EntitySubtype interface
 *   - Add the specific fields of your type of entity and the corresponding
 *       get and set methods. TODO: only one method to set everything?
 *   - Create two constructors with the following signatures:
 *       public YourEntity(Map map, int x, int y):
 *           creates a new entity with some default properties
 *       public YourEntity(Map map, StringTokenizer tokenizer) throws ZSDXException:
 *           creates an existing entity from a string
 *   - Create the toString() method: public String toString()
 *       to export the entity into a string
 *   - Redefine if necessary the getObstacle() method: public int getObstacle()
 *       if your entity is an obstacle.
 *   - Redefine if necessary the methods getNbDirections(), hasName(), isResizable()
 *       to indicate how the graphical components will be organized.
 *       - public int getNbDirections(): returns the number of possible
 *           directions of this kind of entity (default is zero)
 *       - public boolean hasName(): indicates whether the entity has a 
 *           name field, i.e. if it is identifiable (default is false)
 *       - public boolean isResizable(): indicates whether the entity can
 *           be resized (default is false). If the entity is resizable, you can
 *           also redefine the getUnitarySize() method (default is 8*8).
 *   - Redefine the getOrigin() method: public Point getOrigin()
 *       if the origin is not the top-left corner of the entity.
 *   - Redefine the isValid() method: public boolean isValid()
 *       to check the validity of the fields (don't forget to call super.isValid()).
 *   - Define the static generalImageDescriptions field:
 *       public static final EntityImageDescription[] generalImageDescriptions
 *       to define a default 16*16 image representing this kind of entity for each subtype.
 *       These 16*16 images will be used to build the toolbar to add entities on the map.
 *   - Redefine the non-static updateImageDescription() method:
 *       public void updateImageDescription()
 *       which updates the image representing the entity on the map in its current state.
 *   - If your entity is not drawn from an image file but in a more complex way,
 *       you cannot use updateImageDescription() and you have to redefine directly the paint() method:
 *       public abstract void paint(Graphics g, double zoom, boolean showTransparency).
 *   - If your entity has a subtype, redefine the getSubtype() method:
 *       public EntitySubtype getSubtype() and change its return type.
 * - Create a subclass of EditEntityComponent and declare it in
 *     EditEntityComponent.editEntityComponentClasses.
 * - Create a subclass of ActionEditEntity.
 */
public abstract class MapEntity extends Observable {

    /**
     * The map.
     */
    protected Map map;

    /**
     * Indicates that the entity has been initialized.
     */
    protected boolean initialized;

    /**
     * Position of the entity in the map.
     */
    protected Rectangle positionInMap;

    /**
     * Layer of the entity on the map.
     */
    protected int layer;

    /**
     * Direction of the entity (0 to 3).
     * Not used by all kinds of entities.
     */
    protected int direction;

    /**
     * String identifying the entity.
     * Not used by all kinds of entities.
     */
    protected String name;

    /**
     * The subtype of entity, or null if the entity has no subtype.
     */
    protected EntitySubtype subtype;

    /**
     * Color to display instead of the transparent pixels of the image.
     */
    public static final Color bgColor = new Color(255, 0, 255);

    /**
     * Coordinates of the origin point of an entity by default.
     */
    private static final Point defaultOrigin = new Point(0, 0);

    /**
     * Default minimum size of a resizable entity.
     */
    private static final Dimension defaultUnitarySize = new Dimension(8, 8);

    /**
     * Description of the image representing currently this particular entity.
     */
    protected EntityImageDescription currentImageDescription;

    // constants to identify the layer

    public static final int LAYER_LOW = 0;
    public static final int LAYER_INTERMEDIATE = 1;
    public static final int LAYER_HIGH = 2;
    public static final int LAYER_NB = 3;

    // constants to identify the obstacles

    public static final int OBSTACLE_NONE = 0;
    public static final int OBSTACLE = 1;
    public static final int OBSTACLE_TOP_RIGHT = 2;
    public static final int OBSTACLE_TOP_LEFT = 3;
    public static final int OBSTACLE_BOTTOM_LEFT = 4;
    public static final int OBSTACLE_BOTTOM_RIGHT = 5;
    public static final int OBSTACLE_SHALLOW_WATER = 6;
    public static final int OBSTACLE_DEEP_WATER = 7;

    /**
     * Creates an entity.
     * If the entity is identifiable, a default name
     * is given to the entity (this name is computed such that it is
     * different from the other entities of the same kind on the map).
     * @param map the map
     * @param layer layer of the entity
     * @param x x position of the entity on the map
     * @param y y position of the entity on the map
     * @param width width of the entity
     * @param height height of the entity
     * @param computeDefaultName true to give a default name to the entity
     * (this name is computed such that it is different from the other
     * entities of the same kind on the map) or false if the entity is
     * not identifiable or if you want to set its name later 
     */
    protected MapEntity(Map map, int layer, int x, int y,
	    int width, int height, boolean computeDefaultName) {
	this.map = map;
	this.layer = layer;
	this.positionInMap = new Rectangle(x, y, width, height);
	
	initializeImageDescription();

	if (hasName() && computeDefaultName) {
	    computeDefaultName();
	}
    }

    /**
     * Parses the common information of the entity, i.e. the layer, the position,
     * the size (if the entity is resizable), the name (if the entity is identifiable)
     * and the direction (if the entity have a direction property).
     * This constructor should be called by the subclasses before parsing the specific part of the line.
     * @param map the map
     * @param tokenizer the string tokenizer parsing the entity line (the entity type has already been parsed)
     * @throws ZSDXException if the line contains an error
     */
    protected MapEntity(Map map, StringTokenizer tokenizer) throws ZSDXException {
	this(map, LAYER_LOW, 0, 0, 0, 0, false);

	try {
	    int layer = Integer.parseInt(tokenizer.nextToken());
	    int x = Integer.parseInt(tokenizer.nextToken());
	    int y = Integer.parseInt(tokenizer.nextToken());

	    setLayer(layer);
	    positionInMap.x = x; // for now the origin is (0,0)
	    positionInMap.y = y;

	    int width = 0;
	    int height = 0;
	    if (isSizeVariable()) {
		width = Integer.parseInt(tokenizer.nextToken());
		height = Integer.parseInt(tokenizer.nextToken());
	    }

	    if (hasName()) {
		setName(tokenizer.nextToken());
	    }

	    if (hasDirection()) {
		setDirection(Integer.parseInt(tokenizer.nextToken()));
	    }

	    if (hasSubtype()) {
		setSubtype(getSubtype(Integer.parseInt(tokenizer.nextToken())));
	    }

	    if (isSizeVariable()) {
		setSize(width, height); // some entities need to know their direction before they can be resized
	    }
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("Value expected");
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected: " + ex.getMessage());
	}
    }
    
    /**
     * Creates a new map entity with the specified type.
     * @param map the map
     * @param entityType the type of entity to create (except TILE)
     * @param entitySubtype the subtype of entity to create
     */
    public static MapEntity create(Map map, EntityType entityType, EntitySubtype entitySubtype) throws MapException {

	MapEntity entity = null;
	Class<?> entityClass = null;
	try {
	    entityClass = entityType.getEntityClass();
	    Class<?>[] paramTypes = {Map.class, int.class, int.class};
	    Object[] paramValues = {map, 0, 0};
	    Constructor<?> constructor = entityClass.getConstructor(paramTypes);
	    entity = (MapEntity) constructor.newInstance(paramValues);
	    entity.setSubtype(entitySubtype);
	    entity.initializeImageDescription();
	}
	catch (NoSuchMethodException ex) {
	    System.err.println("Cannot find the constructor of class " + entityClass);
	    System.exit(1);
	}
	catch (InvocationTargetException ex) {
	    System.err.println("Cannot create the entity: " + ex.getMessage());
	    ex.getCause().printStackTrace();
	    System.exit(1);
	}
	catch (InstantiationException ex) {
	    System.err.println("Cannot create the entity: " + ex.getMessage());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Cannot create the entity: " + ex.getMessage());
	    ex.printStackTrace();
	    System.exit(1);
	}

	entity.initialized = true;
	return entity;
    }
    
    /**
     * Creates a map entity from a string description as returned by toString().
     * @param map the map (needed for some types of entities)
     * @param description a string describing the entity, as returned by toString()
     * @throws ZSDXException if the string is incorrect
     */
    public static MapEntity createFromString(Map map, String description) throws ZSDXException {

	StringTokenizer tokenizer = new StringTokenizer(description, "\t");
	EntityType entityType = EntityType.get(Integer.parseInt(tokenizer.nextToken()));

	MapEntity entity = null;
	
	Class<? extends MapEntity> entityClass = entityType.getEntityClass();
	Constructor<? extends MapEntity> constructor;
	
	try {
	    constructor = entityClass.getConstructor(Map.class, StringTokenizer.class);
	    entity = constructor.newInstance(map, tokenizer);
	    entity.updateImageDescription();

	    // now the origin is valid
	    entity.setPositionInMap(entity.positionInMap.x, entity.positionInMap.y);
	}
	catch (NoSuchMethodException ex) {
	    System.err.println("Unexpected error: " + ex.getMessage());
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Unexpected error: " + ex.getMessage());
	    System.exit(1);
	}
	catch (InstantiationException ex) {
	    System.err.println("Unexpected error: " + ex.getMessage());
	    System.exit(1);
	}
	catch (InvocationTargetException ex) {
	    
	    Throwable cause = ex.getCause();

	    if (cause instanceof Error) {
		throw (Error) cause;
	    }
	    else if (cause instanceof RuntimeException) {
		throw (RuntimeException) cause;
	    }
	    else {
		throw (ZSDXException) cause;
	    }
	}

	entity.setInitialized(true);
	return entity;
    }

    /**
     * Creates a copy of the specified entity.
     * If the entity has a name, a different name is automatically assigned to the copy.
     * @param map the map
     * @param other an entity
     * @return the copy created
     */
    public static MapEntity createCopy(Map map, MapEntity other) {

	try {
	    return createFromString(map, other.toString());
	}
	catch (ZSDXException ex) {
	    System.err.println("Unexpected error: cannot create a copy of entity '" + other + "'");
	    return null;
	}
    }

    /**
     * Returns a string describing this entity: the entity type, the layer, the coordinates,
     * the id (if any) and the direction (if any).
     * Subclasses should redefine this method to add their own information (if any).
     * @return a string representation of the entity.
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();
	buff.append(getType());
	buff.append('\t');
	buff.append(layer);
	buff.append('\t');
	buff.append(getX());
	buff.append('\t');
	buff.append(getY());

	if (isSizeVariable()) {
	    buff.append('\t');
	    buff.append(getWidth());
	    buff.append('\t');
	    buff.append(getHeight());
	}

	if (hasName()) {
	    buff.append('\t');
	    buff.append(getName());
	}

	if (hasDirection()) {
	    buff.append('\t');
	    buff.append(getDirection());
	}

	if (hasSubtype()) {
	    buff.append('\t');
	    buff.append(getSubtypeId());
	}

	return buff.toString();
    }

    /**
     * Sets whether the entity is initialized.
     * @param initialized true if the entity is initialized
     */
    private void setInitialized(boolean initialized) {
	this.initialized = initialized;
    }

    /**
     * Checks the entity validity. An entity must be valid before it is saved.
     * @return true if the entity is valid
     */
    public boolean isValid() {
	
	try {
	    checkPositionTopLeft(positionInMap.x, positionInMap.y);
	    
	    if (isResizable()) {
		checkSize(positionInMap.width, positionInMap.height);
	    }
	    return true;
	}
	catch (MapException ex) {
	    return false;
	}
    }
    
    /**
     * Returns the entity's obstacle property (default is OBSTACLE_NONE).
     * @return OBSTACLE_NONE, OBSTACLE, OBSTACLE_TOP_RIGHT,
     * OBSTACLE_TOP_LEFT, OBSTACLE_BOTTOM_LEFT or OBSTACLE_BOTTOM_RIGHT
     */
    public int getObstacle() {
	return OBSTACLE_NONE;
    }
    
    /**
     * Returns the coordinates of the origin point of the entity.
     * By default, it is (0,0) (the origin is the top-left corner).
     * Redefine this method in a subclass to change the origin.
     * @return the coordinates of the origin point
     */
    protected Point getOrigin() {
	return defaultOrigin;
    }

    /**
     * Returns the y coordinate of the origin point of the entity.
     * By default, it is zero (the origin is the top-left corner).
     * Redefine this method in a subclass to change the origin.
     * @return the y coordinate of the origin point
     */
    protected int getOriginY() {
	return 0;
    }
    
    /**
     * Returns the position of the entity on the map.
     * @return the position of the entity on the map
     */
    public Rectangle getPositionInMap() {
	return positionInMap;
    }

    /**
     * Changes the position of the entity on the map, by specifying its rectangle.
     * @param position a rectangle
     * @throws MapException if the rectangle's size is zero
     */
    public void setPositionInMap(Rectangle position) throws MapException {

	checkPositionTopLeft(position.x, position.y);
	checkSize(position.width, position.height);
	
	setPositionTopLeft(position.x, position.y);
	setSize(position.width, position.height);
    }

    /**
     * Changes the position of the entity on the map, by specifying two points
     * (the top-left corner and the bottom-right corner).
     * @param x1 x coordinate of the first point
     * @param y1 y coordinate of the first point
     * @param x2 x coordinate of the second point
     * @param y2 y coordinate of the second point
     * @throws MapException if the entity is not resizable, or the rectangle width
     * or its height is zero, or the coordinates are wrong
     */
    public void setPositionInMap(int x1, int y1, int x2, int y2) throws MapException {

	int x = Math.min(x1, x2);
	int width = Math.abs(x2 - x1);
	
	int y = Math.min(y1, y2);
	int height = Math.abs(y2 - y1);

	checkPositionTopLeft(x, y);
	checkSize(width, height);

	setPositionTopLeft(x, y);
	setSize(width, height);
    }
    
    /**
     * Changes the position of the entity on the map, by specifying the coordinates of its hotspot.
     * The size of the entity is not changed.
     * @param x x coordinate of the hotspot
     * @param y y coordinate of the hotspot
     * @throws MapException if the coordinates of the top-left corner are not divisible by 8
     */
    public void setPositionInMap(int x, int y) throws MapException {

	// calculate the new coordinates of the top-left corner
	Point origin = getOrigin();
	setPositionTopLeft(x - origin.x, y - origin.y);
    }

    /**
     * Checks whether the specified position of the entity is correct (i.e. whether the
     * coordinates of the top-left corner are multiple of 8).
     * @param x the x coordinate of the top-left corner to check
     * @param y the y coordinate of the top-left corner to check
     * @throws MapException if the coordinates specified are not divisible by 8
     */
    protected void checkPositionTopLeft(int x, int y) throws MapException {

	if (x % 8 != 0) {
	    throw new MapException("Wrong x value (" + x + "): the coordinates of the top-left corner must divisible by 8");
	}

	if (y % 8 != 0) {
	    throw new MapException("Wrong y value (" + y + "): the coordinates of the top-left corner must divisible by 8");
	}
    }

    /**
     * Changes the position of the entity on the map, by specifying the coordinates of its
     * top-left corner.
     * @param x x coordinate of the top-left corner
     * @param y y coordinate of the top-left corner
     * @throws MapException if the coordinates specified are not divisible by 8
     */
    public void setPositionTopLeft(int x, int y) throws MapException {
	
	checkPositionTopLeft(x, y);
	setPositionTopLeftImpl(x, y);
	
	// notify
	setChanged();
	notifyObservers();
    }
    
    /**
     * Changes the position of the entity on the map, by specifying the coordinates of its
     * top-left corner.
     * @param x x coordinate of the top-left corner
     * @param y y coordinate of the top-left corner
     */
    protected void setPositionTopLeftImpl(int x, int y) throws MapException {
	positionInMap.x = x;
	positionInMap.y = y;
    }

    /**
     * Returns whether or not the entity is resizable.
     * By default, an entity is not resizable. The subclasses which represent
     * resizable entities should redefine this method and return true.
     * @return whether or not the entity is resizable
     */
    public boolean isResizable() {
	return false;
    }

    /**
     * Returns whether two entities of this type can have different sizes.
     * This might happen for some kind of entities even if they are not resizable.
     * If this function returns true, the editor will parse the width an the height 
     * for this kind of entity.
     * @return true if two entities of this type can have different sizes
     */
    public boolean isSizeVariable() {
	return isResizable();
    }

    /**
     * Returns whether or not the entity can be resized by extending it
     * horizontally or vertically.
     * This function only makes sense for resizable entities, otherwise
     * the behavior is unspecified.
     * By default, this function returns true. The subclasses which represent
     * resizable entities should can redefine this method if they want
     * to put some constraints on the resizing direction.
     * @param direction one of the two main directions (0: horizontal, 1: vertical)
     * @return whether or not the entity can be expanded in that direction
     */
    public boolean isExtensible(int direction) {
	return true;
    }

    /**
     * Returns whether the entity has to remain square when it is being resized.
     * By default, false is returned.
     * @return true if the entity must be square
     */
    public boolean mustBeSquare() {
	return false;
    }

    /**
     * Checks whether the specified size of the entity is correct (i.e. whether it is
     * a multiple of getUnitWidth() and getUnitHeight()).
     * @param width the width to check
     * @param height the height to check
     * @throws MapException if the size is not correct
     */
    protected void checkSize(int width, int height) throws MapException {

	if (!isResizable()) {
	    throw new MapException("This entity is not resizable");
	}

	if (width <= 0 || height <= 0) {
	    throw new MapException("The entity's size must be positive (the size specified is ("
		    + width + "x" + height + "))"); 
	}
	
	Dimension unitSize = getUnitarySize();
	if (width % unitSize.width != 0) {
	    throw new MapException("The entity's width must be a multiple of " + unitSize.width
		    + " (the width specified is " + width + ')');
	}

	if (height % unitSize.height != 0) {
	    throw new MapException("The entity's height must be a multiple of " + unitSize.height
		    + " (the height specified is " + height + ')');
	}
    }

    /**
     * Changes the size of the entity on the map.
     * The location of the entity is not changed.
     * @param width width of the entity in pixels
     * @param height height of the entity in pixels
     * @throws MapException if the entity is not resizable,
     * or the size specified is lower than or equal to zero,
     * or the size specified is not divisible by 8
     */
    public void setSize(int width, int height) throws MapException {

	checkSize(width, height);
	setSizeImpl(width, height);
	
	// notify
	setChanged();
	notifyObservers();
    }

    /**
     * Changes the size of the entity.
     * @param width width of the entity in pixels
     * @param height height of the entity in pixels
     */
    protected void setSizeImpl(int width, int height) {
	positionInMap.width = width;
	positionInMap.height = height;
    }

    /**
     * Returns whether the entity has a direction property.
     * The subclasses which represent entities with a direction
     * should return true.
     * @return true if the entity has a direction, false otherwise
     */
    public boolean hasDirection() {
	return getNbDirections() > 1;
    }

    /**
     * Returns the number of possible directions of the entity.
     * By default, an entity has no direction property and this function returns 0.
     * @return the number of possible directions of the entity (or 0
     * if the entity has not a direction property)
     */
    public int getNbDirections() {
	return 0;
    }

    /**
     * Returns the direction of the entity.
     * The entity should have a direction (i.e. hasDirection() returns true).
     * @return the entity's direction
     */
    public int getDirection() {
	return direction;
    }

    /**
     * Changes the direction of the entity.
     * The entity must have a direction (i.e. hasDirection() returns true).
     * By default, an entity has no direction and this method throws an exception.
     * @param direction the entity's direction
     * @throws UnsupportedOperationException if the entity has no direction
     * @throws IllegalArgumentException if the direction is invalid
     */
    public void setDirection(int direction) throws UnsupportedOperationException, IllegalArgumentException {

	if (!hasDirection()) {
	    throw new UnsupportedOperationException("This entity has no direction");
	}

	if (direction < 0 || direction >= getNbDirections()) {
	    throw new IllegalArgumentException("The direction '" + direction
		    	+ "' is incorrect: the number of possible directions is " + getNbDirections());
	}

	this.direction = direction;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether the entity has a name.
     * By default, an entity has no name and this function returns false.
     * The subclasses which represent identifiable entities should return true.
     * @return true if the entity has a name, false otherwise
     */
    public boolean hasName() {
	return false;
    }

    /**
     * Returns the name of the entity.
     * The name of an entity permits to identify it.
     * @return the entity's name, or null if this entity is not identifiable
     */
    public String getName() {
	return name;
    }

    /**
     * Changes the name of the entity.
     * The entity must be identifiable (i.e. hasName() returns true).
     * By default, an entity is not identifiable and this method throws an exception.
     * If the name specified is already used, another name is automatically set.
     * @param name the new entity's name
     * @throws UnsupportedOperationException if the entity is not identifiable
     * @throws MapException if this name is not valid
     */
    public void setName(String name) throws UnsupportedOperationException, MapException {

	if (!hasName()) {
	    throw new UnsupportedOperationException("This entity is not identifiable");
	}

	if (name.indexOf(' ') != -1 || name.indexOf('\t') != -1) {
	    throw new MapException("The entity name cannot have whitespaces");
	}

	MapEntity other = map.getEntityWithName(getType(), name);
	if (other != null && other != this) {

	    int counter = 2;
	    String[] words = name.split("_");
	    StringBuffer prefix;

	    if (words.length > 1) {

		prefix = new StringBuffer();
		for (int i = 0; i < words.length - 1; i++) {
		    prefix.append(words[i]);
		    prefix.append('_');
		}
		String suffix = words[words.length - 1];

		try {
		    counter = Integer.parseInt(suffix);
		}
		catch (NumberFormatException ex) {
		    prefix.append(suffix);
		    prefix.append('_');
		}
	    }
	    else {
		prefix = new StringBuffer(name);
		prefix.append('_');
	    }

	    counter = 2;
	    while (map.getEntityWithName(getType(), prefix.toString() + counter) != null) {
		counter++;
	    }
	    name = prefix.toString() + counter;
	}

	// debug
	other = map.getEntityWithName(getType(), name);
	if (other != null && other != this) {
	    throw new MapException("An entity with this name already exists and I was unable to compute a new one");
	}

	this.name = name;
	setChanged();
	notifyObservers();
    }

    /**
     * Sets a default name to the entity.
     * @param map the map
     */
    private void computeDefaultName() {
	
	EntityType entityType = getType();
	String prefix = entityType.getName().toLowerCase().replace(' ', '_');

	try {
	    setName(prefix);
	}
	catch (MapException ex) {
	    // should not happen since setName() makes sure the name is unique
	    System.err.println("Unexcepted error: " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Makes sure the name of this entity is unique on the map, renaming it if necessary.
     * setName() guarantees that no other entity has the same name on the map, but two
     * entities may have been created with the same name without detecting it because they are
     * not added to the map yet. This can happen with copy/paste operations.
     * If the entity is not identifiable, this method does nothing.
     * @param map the map
     */
    public void ensureNameIsUnique() {
	
	if (hasName()) {
	    try {
		setName(name);
	    }
	    catch (MapException ex) {
		// should not happen
		System.err.println("Unexcepted error: " + ex.getMessage());
		System.exit(1);
	    }
	}
    }

    /**
     * Returns the minimum size of the entity (for a resizable entity).
     * When the entity is resized, its new size must be a multiple of this minimum size.
     * @return the minimum size of the entity
     */
    public Dimension getUnitarySize() {
	return defaultUnitarySize;
    }

    /**
     * Changes the position of the entity.
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     * @throws MapException if the new coordinates are not divisible by 8
     */
    public void move(int dx, int dy) throws MapException {
	
	int x = getX() + dx;
	int y = getY() + dy;

	setPositionInMap(x, y);

	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether or not a point is in the entity's rectangle.
     * @param x x of the point
     * @param y y of the point
     * @return true if the point is in the entity's rectangle, false otherwise
     */
    public boolean containsPoint(int x, int y) {
	return positionInMap.contains(x, y);
    }

    /**
     * Returns the layer of the entity.
     * @return the layer of the entity: MapEntity.LAYER_LOW (most of the tiles),
     * MapEntity.LAYER_INTERMEDIATE or MapEntity.LAYER_HIGH.
     */
    public int getLayer() {
	return layer;
    }

    /**
     * Changes the layer of the tile.
     * @param layer the layer of the tile: MapEntity.LAYER_LOW (most of the tiles),
     * MapEntity.LAYER_INTERMEDIATE or MapEntity.LAYER_HIGH.
     */
    public void setLayer(int layer) {
	if (layer != this.layer) {
	    this.layer = layer;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the x coordinate of the entity's origin point on the map.
     * @return the x coordinate of the entity's origin point on the map
     */
    public int getX() {
	return positionInMap.x + getOrigin().x;
    }

    /**
     * Returns the y coordinate of the entity's origin point on the map.
     * @return the y coordinate of the entity's origin point on the map
     */
    public int getY() {
	return positionInMap.y + getOrigin().y;
    }

    /**
     * Returns the x coordinate of the entity's top left corner.
     * @return the x coordinate of the entity's top left corner
     */
    public int getXTopLeft() {
	return positionInMap.x;
    }

    /**
     * Returns the y coordinate of the entity's top left corner.
     * @return the y coordinate of the entity's top left corner
     */
    public int getYTopLeft() {
	return positionInMap.y;
    }

    /**
     * Returns the width of the entity
     * @return the entity's width
     */
    public int getWidth() {
	return positionInMap.width;
    }

    /**
     * Returns the height of the entity
     * @return the entity's height
     */
    public int getHeight() {
	return positionInMap.height;
    }

    /**
     * Returns the size of the entity
     * @return the entity's size
     */
    public Dimension getSize() {
	return new Dimension(getWidth(), getHeight());
    }

    /**
     * Initializes the description of the image currently representing the entity.
     * By default, the image description is initialized to a copy of the first general
     * image description of this kind of entity.
     */
    protected void initializeImageDescription() {

	EntityImageDescription generalImageDescription = getImageDescription(getType(), subtype);

	if (generalImageDescription != null) {
	    currentImageDescription = new EntityImageDescription(generalImageDescription);
	    updateImageDescription();
	}
    }
    
    /**
     * Updates the description of the image currently representing the entity.
     * By default, the image description is a copy of the general image description of this kind of entity.
     * Redefine this method to display the entity with an image containing
     * the entity's current properties, by modifying the currentImageDescription field.
     */
    public void updateImageDescription() {
	// do nothing by default (the image description remains a copy of the description of the entity type)
    }

    /**
     * Returns the images representing a specified kind entity.
     * @param type a type of entity
     * @return the image descriptions of this kind of entity
     */
    public static EntityImageDescription[] getImageDescriptions(EntityType type) {

	EntityImageDescription imageDescriptions[] = null;

	Class<? extends MapEntity> entityClass = type.getEntityClass();
	try {
	    Field imageDescriptionField = entityClass.getField("generalImageDescriptions");
	    imageDescriptions = (EntityImageDescription[]) imageDescriptionField.get(null);
	}
	catch (NoSuchFieldException ex) {
	    System.err.println("The field 'generalImageDescriptions' is missing in class " + entityClass.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Cannot access the field 'generalImageDescriptions' in class " + entityClass.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	return imageDescriptions;
    }
    
    /**
     * Returns the number of images representing a specified kind entity.
     * @param type a type of entity
     * @return the number of image descriptions of this kind of entity
     */
    public static int getNbImageDescriptions(EntityType type) {
	return getImageDescriptions(type).length;
    }

    /**
     * Returns the description of an image representing a specified kind entity.
     * @param type a type of entity
     * @param subtype the subtype of entity
     * @return an image description of this subtype of entity
     */
    public static EntityImageDescription getImageDescription(EntityType type, EntitySubtype subtype) {

	EntityImageDescription[] imageDescriptions = getImageDescriptions(type);
	if (imageDescriptions == null) {
	    return null;
	}

	int index = (subtype != null) ? ((Enum) subtype).ordinal() : 0;
	return getImageDescriptions(type)[index];
    }

    /**
     * Draws the entity on the map view.
     * This method draws the entity's image as described by the currentImageDescription field,
     * which you can modify by redefining the updateImageDescription() method.
     * To draw a more complex image, redefine the paint() method.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {
	currentImageDescription.paint(g, zoom, showTransparency, positionInMap);
    }

    /**
     * Returns a value identifying the kind of entity: TILE, DESTINATION_POINT...
     * @return the type of entity
     */
    public final EntityType getType() throws IllegalStateException {
	return EntityType.get(getClass());
    }

    /**
     * Returns whether this entity has a subtype.
     * You don't need to redefine this method. Whether the entity has
     * a subtype depends on the subtype class specified in the EntityType enumeration.
     * @return true if this entity has a subtype
     */
    public final boolean hasSubtype() {
	return getType().getSubtypeEnum() != null;
    }

    /**
     * Returns the subtype id of this entity.
     * @return the subtype id
     */
    public final int getSubtypeId() {

	if (subtype == null) {
	    return 0;
	}
	
	return subtype.getId();
    }

    /**
     * Returns the subtype of this entity.
     * @return the subtype, or null if this type of entity has no subtype
     */
    public EntitySubtype getSubtype() {
	return subtype;
    }

    /**
     * Returns the subtype corresponding to the specified subtype id
     * for this type of entity.
     * You might redefine this method to declare a more specific return type.
     * @param id the subtype id
     * @return the subtype
     */
    public EntitySubtype getSubtype(int id) {
	return getType().getSubtype(id);
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype
     * @throws MapException if the subtype is not valid
     */
    public void setSubtypeId(int subtype) throws MapException {
	setSubtype(getSubtype(subtype));
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype to set
     * @throws MapException if the subtype is not valid
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {
	this.subtype = subtype;
	setChanged();
	notifyObservers();
    }
}
