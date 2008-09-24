package zsdx;

import java.util.*;
import java.io.*;
import java.awt.Dimension;
import java.awt.Point;
import java.awt.Rectangle;

/**
 * This class describes a map.
 * A map is observable.
 */
public class Map extends Observable {

    // map properties
    
    /**
     * Id of the map.
     */
    private String mapId;

    /**
     * Name of the map.
     */
    private String name;

    /**
     * Size of the map, in pixels (the width and the height
     * must be multiples of 8).
     */
    private Dimension size;

    /**
     * Id of the background music: name of a music file, Music.noneId or Music.unchangedId.
     */
    private String musicId;

    /**
     * Id of the tileset of the map (or an empty string if no tileset is set).
     */
    private String tilesetId;

    /**
     * Tileset of the map.
     * The tileset is the set of small images (tiles) used to build the map. 
     */
    private Tileset tileset;

    /**
     * Index of the world where this map is:
     * - 0 if the map is outside
     * - -1 if the map is inside (by default)
     * - 1 to 20 if the map is in a dungeon
     */
    private int world;
    
    /**
     * Floor of this map:
     * - a floor number between -16 and 15
     * - -99: special value to indicate an unknown floor, displayed with '?'
     * - -100: no floor (by default)
     */
    private int floor;
    
    /**
     * Location of this map in its floor or in its world. It is used to show
     * Link's position on the map menu.
     * - For a map in the outside world: coordinates of the top-left corner
     *   of the map in the whole world.
     * - For a map in the inside world: location of the map on the outside world.
     * - For a map in a dungeon: coordinates of the top-left corner of the map
     * in its floor.
     */
    private Point location;
    
    /**
     * Index of the variable of the savegame which stores the number of small keys
     * of this map.
     * A value of -1 indicates that this map has small key counter.
     */
    private int smallKeysVariable;

    // content of the map
    
    /**
     * Entities of the map.
     * This is an array of three entity lists, one for each layer.
     */
    private MapEntities[] allEntities;

    /**
     * The entities selected.
     */
    private MapEntitySelection entitySelection;
    
    /**
     * True if some tiles could not be found in the tileset
     * when the tileset was loaded.
     */
    private boolean badTiles;

    /**
     * The history of the actions made by the user on the map.
     */
    private MapEditorHistory history;

    /**
     * Minimum width of a map in pixels.
     */
    public static final int MINIMUM_WIDTH = 320;

    /**
     * Minimum height of a map in pixels.
     */
    public static final int MINIMUM_HEIGHT = 240;
    
    /**
     * Creates a new map.
     * @throws ZSDXException if the resource list could not be updated after the map creation
     */
    public Map() throws ZSDXException {
	super();

	this.size = new Dimension(MINIMUM_WIDTH, MINIMUM_HEIGHT);
	this.tileset = null;
	this.tilesetId = "";
	this.musicId = Music.noneId;
	this.world = -1;

	initialize();
	
	// compute an id and a name for this map
	this.name = "New map";
	Resource mapResource = Project.getResource(ResourceDatabase.RESOURCE_MAP);
	this.mapId = mapResource.computeNewId();

	setChanged();
	notifyObservers();
    }

    /**
     * Loads an existing map.
     * @param mapId id of the map to load
     * @throws ZSDXException if the map could not be loaded
     */
    public Map(String mapId) throws ZSDXException {
	this.mapId = mapId;
	initialize();
	load();
    }
	
    /**
     * Initializes the object.
     */
    private void initialize() {
	this.allEntities = new MapEntities[3];
	for (int i = 0; i < MapEntity.LAYER_NB; i++) {
	    this.allEntities[i] = new MapEntities();
	}

	this.entitySelection = new MapEntitySelection(this);
	this.history = new MapEditorHistory();
    }

    /**
     * Returns the id of the map.
     * @return the id of the map
     */
    public String getId() {
	return mapId;
    }

    /**
     * Returns the map name.
     * @return the name of the map, for example "Link's House"
     */
    public String getName() {
	return name;
    }

    /**
     * Changes the name of the map
     * @param name new name of the map
     */
    public void setName(String name) {

	if (!name.equals(this.name)) {
	    this.name = name;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the history of actions of the map.
     * @return the map history
     */
    public MapEditorHistory getHistory() {
	return history;
    }

    /**
     * Returns the map size.
     * @return the map size (in pixels)
     */
    public Dimension getSize() {
	return size;
    }

    /**
     * Returns the map width.
     * @return the map width (in pixels)
     */
    public int getWidth() {
	return size.width;
    }

    /**
     * Returns the map height.
     * @return the map height (in pixels)
     */
    public int getHeight() {
	return size.height;
    }

    /**
     * Changes the map size.
     * If the new size is lower than the old one, the tiles in the removed area
     * are not destroyed. The minimum size of a map is 320*240.
     * The width and the height must be multiples of 8.
     * @param size the new map size (in pixels)
     * @throws MapException if the width or the height is incorrect
     */
    public void setSize(Dimension size) throws MapException {
	if (size.width < MINIMUM_WIDTH || size.height < MINIMUM_HEIGHT) {
	    throw new RuntimeException("The minimum size of a map is " +
		    MINIMUM_WIDTH + '*' + MINIMUM_HEIGHT + '.');
	}
	
	
	if (size.width < MINIMUM_WIDTH || size.height < MINIMUM_HEIGHT) {
	    throw new MapException("The minimum size of a map is " +
		    MINIMUM_WIDTH + '*' + MINIMUM_HEIGHT + '.');
	}
	
	if (size.width % 8 != 0 || size.height % 8 != 0) {
	    throw new MapException("The width and the height of the map must be multiples of 8.");
	}

	this.size = size;

	setChanged();
	notifyObservers();
    }

    /**
     * Returns the tileset associated to this map.
     * The tileset is the set of small images (tiles) used to build the map. 
     * @return the tileset (null if no tileset is set)
     */
    public Tileset getTileset() {
	return tileset;
    }

    /**
     * Returns the id of the tileset associated to this map.
     * @return the tileset id (or an empty string if no tileset is set)
     */
    public String getTilesetId() {
	return tilesetId;
    }

    /**
     * Changes the tileset of the map.
     * @param tilesetId id of the new tileset, or an empty string to set no tileset
     * @return true if the tileset was loaded successfuly, false if some tiles could
     * not be loaded in this tileset
     * @throws MapException if this tileset could be applied
     */
    public boolean setTileset(String tilesetId) throws ZSDXException {

	this.badTiles = false;

	// if the tileset is removed
	if (tilesetId.length() == 0 && this.tilesetId.length() != 0) {

	    this.tilesetId = tilesetId;
	    this.tileset = null;

	    setChanged();
	    notifyObservers();
	}

	// if the tileset is changed
	else if (!tilesetId.equals(this.tilesetId)) {
	
	    this.tileset = new Tileset(tilesetId);

	    for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {

		List<TileOnMap> tiles = allEntities[layer].getTiles();

		for (int i = 0; i < tiles.size(); i++) {

		    try {
			TileOnMap tile = tiles.get(i);
			tile.setTileset(tileset);
		    }
		    catch (TilesetException ex) {
			// the tile is not valid anymore, we should remove it from the map
			tiles.remove(i);
			i--;
			badTiles = true;
		    }
		}
	    }

	    this.tilesetId = tilesetId;

	    setChanged();
	    notifyObservers(tileset);
	}

	return !badTiles;
    }

    /**
     * Returns the index of the world where this map is.
     * @return the world index: 0 if the map is outside, -1 if it is inside,
     * 1 to 20 if it is in a dungeon
     */
    public int getWorld() {
	return world;
    }
    
    /**
     * Returns whether this map belongs to a dungeon.
     * @return true if this map is in a dungeon
     */
    public boolean isInDungeon() {
	return world > 0;
    }

    /**
     * Returns whether this map belongs to the outside world.
     * @return true if this map is in the outside world
     */
    public boolean isInOutsideWorld() {
	return world == 0;
    }
    
    /**
     * Sets the world where this map is.
     * @param world the world index: 0 if the map is outside, -1 if it is inside,
     * 1 to 20 if it is in a dungeon
     * @throws MapException if the specified world is incorrect
     */
    public void setWorld(int world) throws MapException {
	
	if (world != this.world) {
	    
	    if (world > 20 || world < -1) {
		throw new MapException("Invalid world: " + world);
	    }
	    
	    this.world = world;

	    if (!isInDungeon()) { // no dungeon : no floor by default
		setFloor(-100);
	    }
	    else { // dungeon: first floor by default
/* TODO		setFloor(getDungeon().getDefaultFloor());*/
		setSmallKeysVariable(204 + 10 * (world - 1));
	    }

	    setChanged();
	    notifyObservers();
	}
    }
    
    /**
     * Returns the floor of this map.
     * @return the floor
     */
    public int getFloor() {
        return floor;
    }

    /**
     * Sets the floor of this map.
     * @param floor the floor: -100 for no floor, -99 for the unknown floor,
     * -16 to 15 for a normal floor
     * @throws MapException if you specify a floor on the oustide world
     * or if you specify an invalid floor
     */
    public void setFloor(int floor) throws MapException {
	
	if (floor != this.floor) {
	    
	    if (floor != -100 && floor != -99 && (floor < -16 || floor > 15)) {
		throw new MapException("Invalid floor: " + floor);
	    }
	    
	    if (isInOutsideWorld() && floor != -100) {
		throw new MapException("Cannot specify a floor in the outside world");
	    }

	    else if (isInDungeon() && floor != -99/* TODO && !getDungeon().hasFloor(floor)*/) {
		throw new MapException("This floor does not exists in this dungeon");
	    }
	    
	    this.floor = floor;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the index of the savegame variable that stores
     * the counter of small keys for this map.
     * @return the variable of the small key counter
     */
    public int getSmallKeysVariable() {
        return smallKeysVariable;
    }

    /**
     * Sets the index of the savegame variable that stores
     * the counter of small keys for this map.
     * -1 means that the small keys are not enabled on this map.
     * @param keysSavegameVariable the variable of the small key counter
     * @throws MapException if the specified value is not valid
     */
    public void setSmallKeysVariable(int smallKeysVariable) throws MapException {
	
	if (smallKeysVariable != this.smallKeysVariable) {
	    
	    if (smallKeysVariable < -1 || smallKeysVariable > 2048) {
		throw new MapException("Incorrect variable to save the small keys: " + smallKeysVariable);
	    }
	    
	    if (isInDungeon() && smallKeysVariable != 204 + 10 * (getWorld() - 1)) {
		throw new MapException("Cannot change the variable to save the small keys because this map is in a dungeon");
	    }
	    
	    this.smallKeysVariable = smallKeysVariable;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the location of this map in its floor or its world.
     * @return the location of this map
     */
    public Point getLocation() {
        return location;
    }

    /**
     * Sets the location of this map in its floor or its world.
     * @param location the location of this map
     */
    public void setLocation(Point location) {
        this.location = location;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the total number of entities of the map.
     * @return the total number of entities of the map.
     */
    public int getNbEntities() {
	
	int nbEntities = 0;

	// count the entities of each layer
	for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {
	    nbEntities += allEntities[layer].size();
	}

	return nbEntities;
    }

    /**
     * Returns the total number of tiles of the map.
     * @return the total number of tiles of the map.
     */
    public int getNbTiles() {
	
	int nbTiles = 0;

	// count the tiles of each layer
	for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {
	    nbTiles += allEntities[layer].getNbTiles();
	}

	return nbTiles;
    }

    /**
     * Returns the total number of interactive entities of the map.
     * @return the total number of interactive entities of the map.
     */
    public int getNbInteractiveEntities() {
	
	int nbInteractiveEntities = 0;

	// count the interactive entities of each layer
	for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {
	    nbInteractiveEntities += allEntities[layer].getNbInteractiveEntities();
	}

	return nbInteractiveEntities;
    }

    /**
     * Returns the total number of moving entities of the map.
     * @return the total number of moving entities of the map.
     */
    public int getNbMovingEntities() {
	
	int nbMovingEntities = 0;

	// count the moving entities of each layer
	for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {
	    nbMovingEntities += allEntities[layer].getNbMovingEntities();
	}

	return nbMovingEntities;
    }

    /**
     * Returns the entities of the map.
     * @return an array of 3 MapEntity sets: a set for each layer
     */
    public MapEntities[] getAllEntities() {
	return allEntities;
    }

    /**
     * Sets the entities of the map.
     * @param allEntities an array of 3 MapEntity sets: a set for each layer (this array is copied)
     */
    public void setAllEntities(MapEntities[] allEntities) {

	for (int layer = MapEntity.LAYER_LOW; layer < MapEntity.LAYER_NB; layer++) {
	    this.allEntities[layer] = new MapEntities(allEntities[layer]);
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Returns the entities of the map on a given layer.
     * @param layer the layer: MapEntity.LAYER_LOW, MapEntity.LAYER_INTERMEDIATE or MapEntity.LAYER_HIGH
     * @return the entities placed on this layer
     */
    public MapEntities getEntities(int layer) {
	return allEntities[layer];
    }

    /**
     * Returns all entities of a kind, except the tiles.
     * @param entityType a type of entity:
     * MapEntity.ENTITY_ENTRANCE, MapEntity.ENTITY_ENEMY...
     * @return the list of the entities of this kind on the map
     */
    public List<MapEntity> getEntitiesOfType(int entityType) {

	List<MapEntity> list = new LinkedList<MapEntity>();
	for (int layer = MapEntity.LAYER_LOW; layer < MapEntity.LAYER_NB; layer++) {
	    list.addAll(allEntities[layer].getEntitiesOfType(entityType));
	}
	return list;
    }
	
    /**
     * Returns the first entity under a point of the map, in the three layers,
     * starting with the high layer.
     * @param x x of the point
     * @param y y of the point
     * @return the entity found, or null if there is no entity here
     */
    public MapEntity getEntityAt(int x, int y) {

	MapEntity entity = getEntityAt(MapEntity.LAYER_HIGH, x, y);

	if (entity == null) {
	    entity = getEntityAt(MapEntity.LAYER_INTERMEDIATE, x, y);

	    if (entity == null) {
		entity = getEntityAt(MapEntity.LAYER_LOW, x, y);
	    }
	}

	return entity;
    }

    /**
     * Returns the first en tity under a point of the map, in the specified layer.
     * @param layer the layer
     * @param x x of the point
     * @param y y of the point
     * @return the entity found, or null if there is no entity here
     */
    public MapEntity getEntityAt(int layer, int x, int y) {
		
	MapEntities entities = allEntities[layer];
	ListIterator<MapEntity> iterator = entities.listIterator(entities.size());
	while (iterator.hasPrevious()) {

	    MapEntity entity = iterator.previous();
	    if (entity.containsPoint(x, y)) {
		return entity;
	    }
	}

	return null;
    }

    /**
     * Returns the entities located in a rectangle defined by two points.
     * @param x1 x coordinate of the first point
     * @param y1 y coordinate of the first point
     * @param x2 x coordinate of the second point
     * @param y2 y coordinate of the second point
     */
    public List<MapEntity> getEntitiesInRectangle(int x1, int y1, int x2, int y2) {

	List<MapEntity> entitiesInRectangle = new LinkedList<MapEntity>();

	int x = Math.min(x1, x2);
	int width = Math.abs(x2 - x1);

	int y = Math.min(y1, y2);
	int height = Math.abs(y2 - y1);

	Rectangle rectangle = new Rectangle(x, y, width, height);

	for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {
	    
	    for (MapEntity entity: allEntities[layer]) {
		if (rectangle.contains(entity.getPositionInMap())) {
		    entitiesInRectangle.add(entity);
		}
	    }
	}
	
	return entitiesInRectangle;
    }
    
    /**
     * Returns an entity, specifying its type and its name.
     * @param type the type of entity
     * @param name the name of the entity
     * @return the entity, or null if there is no entity with this name
     */
    public MapEntity getEntityWithName(int type, String name) {

	MapEntity entity = null;
	for (int layer = MapEntity.LAYER_LOW;
		layer < MapEntity.LAYER_NB && entity == null;
		layer++) {
	    
	    entity = allEntities[layer].getEntityWithName(type, name);
	}
	
	return entity;
    }

    /**
     * Adds a new entity on the map.
     * @param entity the entity to add
     */
    public void addEntity(MapEntity entity) {

	allEntities[entity.getLayer()].add(entity);

	setChanged();
	notifyObservers();
    }

    /**
     * Removes an entity from the map.
     * @param entity the entity to remove
     */
    public void removeEntity(MapEntity entity) {

	allEntities[entity.getLayer()].remove(entity);

	setChanged();
	notifyObservers();
    }

    /**
     * Changes the position of an entity on the map, by specifying the coordinates
     * of its origin point.
     * @param entity an entity
     * @param x x coordinate of the origin point
     * @param y y coordinate of the origin point
     * @throws MapException if the coordinates are not multiple of 8
     */
    public void setEntityPosition(MapEntity entity, int x, int y) throws MapException {
	entity.setPositionInMap(x, y);

	setChanged();
	notifyObservers();
    }

    /**
     * Changes the position of an entity on the map, by specifying two points.
     * The entity is resized (i.e. repeatX and repeatY are updated) so that
     * it fits exactly in the rectangle formed by the two points.
     * @param entity an entity
     * @param x1 x coordinate of the first point
     * @param y1 y coordinate of the first point
     * @param x2 x coordinate of the second point
     * @param y2 y coordinate of the second point
     * @throws MapException if the entity is not resizable of the rectangle width
     * or its height is zero or the coordinates or the coordinates are not multiple of 8
     */
    public void setEntityPosition(MapEntity entity, int x1, int y1, int x2, int y2) throws MapException {
	entity.setPositionInMap(x1, y1, x2, y2);

	setChanged();
	notifyObservers();
    }

    /**
     * Changes the position of an entity on the map, by specifying its rectangle.
     * The entity is resized so that it fits exactly in the rectangle.
     * @param entity an entity
     * @param position a rectangle
     * @throws MapException if the entity is not resizable of the rectangle width
     * or its height is zero
     */
    public void setEntityPosition(MapEntity entity, Rectangle position) throws MapException {
	entity.setPositionInMap(position);

	setChanged();
	notifyObservers();
    }

    /**
     * Changes the size of an entity on the map.
     * @param entity an entity
     * @param width the new width
     * @param height the new height
     * @throws MapException if the entity is not resizable or the size
     * specified is lower than or equal to zero
     * or the size specified is not divisible by 8
     */
    public void setEntitySize(MapEntity entity, int width, int height) throws MapException {
	entity.setSize(width, height);

	setChanged();
	notifyObservers();
    }

    /**
     * Changes the position of a group of entities.
     * @param entities the entities to move
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     * @throws MapException if the coordinates of an entity are not multiple of 8
     */
    public void moveEntities(List<MapEntity> entities, int dx, int dy) throws MapException {
			  
	for (MapEntity entity: entities) {
	    entity.move(dx, dy);
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Changes the direction of an entity.
     * @param entity the entity to change the direction
     * @param direction the new direction
     * @throws MapException if this entity has no direction
     */
    public void setEntityDirection(MapEntity entity, int direction) throws MapException {

	int oldDirection = entity.getDirection();

	if (direction != oldDirection) {
	    
	    entity.setDirection(direction);
	    entity.updateImageDescription();
	    
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Changes the layer of an entity. You should call this method instead of
     * calling directly MapEntity.setLayer(), because the entity of the 3 layers are
     * stored in 3 different structures.
     * @param entity the entity to change the layer
     * @param layer the new layer
     */
    public void setEntityLayer(MapEntity entity, int layer) {

	int oldLayer = entity.getLayer();

	if (layer != oldLayer) {
	    entity.setLayer(layer);
	    allEntities[oldLayer].remove(entity);
	    allEntities[layer].add(entity);
	    
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the specified entities, sorted in the order of the map.
     * The first entity is the lowest one, the last entity is the highest one.
     * @param entities the entities to sort
     * @return the same entities, sorted as they are in the map
     */
    private List<MapEntity> getSortedEntities(List<MapEntity> entities) {
	
	List<MapEntity> sortedEntities = new LinkedList<MapEntity>();
	
	// sort the entities so that they have the same order as in the map
	for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {

	    for (MapEntity entity: allEntities[layer]) {

		if (entities.contains(entity)) {
		    sortedEntities.add(entity);
		}
	    }
	}

	// now sortedEntities contains all selected entities, sorted in the same order as in the map
	return sortedEntities;
    }

    /**
     * Brings the specified entities to the back, keeping their layer.
     * The order of the specified entities in the map is unchanged.
     * @param entities the entities to move
     */
    public void bringToBack(List<MapEntity> entities) {

	List<MapEntity> sortedEntities = getSortedEntities(entities);

	// bring to back each entity from sortedEntities
	ListIterator<MapEntity> iterator = sortedEntities.listIterator(sortedEntities.size());
	while (iterator.hasPrevious()) {

	    MapEntity entity = iterator.previous();
	    int layer = entity.getLayer();
	    allEntities[layer].remove(entity);
	    allEntities[layer].addFirst(entity);
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Brings the specified entities to the front, keeping their layer.
     * The order of the specified entities in the map is unchanged.
     * @param entities the entities to move
     */
    public void bringToFront(List<MapEntity> entities) {

	List<MapEntity> sortedEntities = getSortedEntities(entities);

	// bring to front each entity from sortedEntities
	ListIterator<MapEntity> iterator = sortedEntities.listIterator(0);
	while (iterator.hasNext()) {

	    MapEntity entity = iterator.next();
	    int layer = entity.getLayer();
	    allEntities[layer].remove(entity);
	    allEntities[layer].addLast(entity);
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Returns the id of the background music of the map.
     * @return the name of the music, i.e. a music file name with the extension,
     * or Music.noneId or Music.unchangedId
     */
    public String getMusic() {
	return musicId;
    }

    /**
     * Changes the default background music of the map.
     * @param musicId the name of the music, i.e. a music file name with the extension,
     * or Music.noneId or Music.unchangedId
     */
    public void setMusic(String musicId) {

	if (!musicId.equals(this.musicId)) {

	    this.musicId = musicId;

	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the entities selected by the user.
     * @return the entities selected by the user
     */
    public MapEntitySelection getEntitySelection() {
	return entitySelection;
    }
    
    /**
     * Returns whether or not when the tileset was loaded, some tiles of the map refered
     * to non existent tiles in the tileset. These bad tiles were removed.
     * @return true if there was bad tiles when the tiles was loaded, false if all tiles
     * were loaded successfuly.
     */
    public boolean badTiles() {
	return badTiles;
    }

    /**
     * Loads the map from its file.
     * @throws ZSDXException if the file could not be read
     */
    public void load() throws ZSDXException {
	
	int lineNumber = 0;
	try {

	    // get the map name in the game resource database
	    Resource mapResource = Project.getResource(ResourceDatabase.RESOURCE_MAP);
	    setName(mapResource.getElementName(mapId));
	    
	    File mapFile = Project.getMapFile(mapId);
	    BufferedReader in = new BufferedReader(new FileReader(mapFile));

	    // read the map general info
	    // syntax: width height world floor x y small_keys_variable tileset_id music_id
	    String line = in.readLine();

	    if (line == null) {
		throw new ZSDXException("The map file is empty");
	    }

	    lineNumber++;
	    StringTokenizer tokenizer = new StringTokenizer(line);

	    int width = Integer.parseInt(tokenizer.nextToken());
	    int height = Integer.parseInt(tokenizer.nextToken());
	    setSize(new Dimension(width, height));

	    setWorld(Integer.parseInt(tokenizer.nextToken()));	    
	    setFloor(Integer.parseInt(tokenizer.nextToken()));
	    
	    int x = Integer.parseInt(tokenizer.nextToken());
	    int y = Integer.parseInt(tokenizer.nextToken());
	    setLocation(new Point(x, y));

	    setSmallKeysVariable(Integer.parseInt(tokenizer.nextToken()));
	    setTileset(tokenizer.nextToken());
	    setMusic(tokenizer.nextToken());

	    // read the map entities
	    line = in.readLine();
	    while (line != null) {
		lineNumber++;

		try {
		    MapEntity entity = MapEntity.createFromString(this, line);
		    addEntity(entity);
		}
		catch (TilesetException ex) {
		    badTiles = true;
		}
		line = in.readLine();
	    }

	    in.close();

	    history.setSaved();
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": Integer expected");
	}
	catch (IndexOutOfBoundsException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": Value expected");
	}
	catch (ZSDXException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": " + ex.getMessage());
	}
	catch (IOException ex) {
	    throw new ZSDXException(ex.getMessage());
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Saves the map into its file.
     * @throws ZSDXException if the file could not be written
     */
    public void save() throws ZSDXException {

	// check that a tileset is selected
	if (tilesetId.length() == 0) {
	    throw new ZSDXException("No tileset is selected");
	}

	try {

	    // open the map file
	    File mapFile = Project.getMapFile(mapId);
	    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(mapFile)));

	    // print the map general info
	    // syntax: width height world floor x y small_keys_variable tileset_id music_id
	    out.print(size.width);
	    out.print('\t');
	    out.print(size.height);
	    out.print('\t');
	    out.print(world);
	    out.print('\t');
	    out.print(floor);
	    out.print('\t');
	    out.print(location.x);
	    out.print('\t');
	    out.print(location.y);
	    out.print('\t');
	    out.print(smallKeysVariable);
	    out.print('\t');
	    out.print(tilesetId);
	    out.print('\t');
	    out.print(musicId);
	    out.println();

	    for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {

		MapEntities entities = allEntities[layer];

		// print the entities
		for (MapEntity entity: entities) {
		    out.print(entity.toString());
		    out.println();
		}
	    }

	    out.close();

	    history.setSaved();

	    // also update the map name in the global resource list
	    Resource mapResource = Project.getResource(ResourceDatabase.RESOURCE_MAP);
	    mapResource.setElementName(mapId, name);
	    Project.getResourceDatabase().save();
	}
	catch (IOException ex) {
	    throw new ZSDXException(ex.getMessage());
	}
    }
}
