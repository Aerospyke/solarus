package editors;

import java.util.*;
import editors.map_editor_actions.*;

/**
 * Represents the tiles selected in the map editor.
 */
public class MapTileSelection extends Observable implements Iterable {

    /**
     * The selected tiles.
     */
    private TileOnMapList tiles;

    /**
     * The map.
     */
    private Map map;

    /**
     * Constructor.
     * @param map the map
     */
    public MapTileSelection(Map map) {
	this.map = map;
	this.tiles = new TileOnMapList();
    }

    /**
     * Returns a selected tile.
     * @param index of the tile to get
     * @return the tile at this index
     */
    public TileOnMap getTile(int index) {
	return tiles.get(index);
    }

    /**
     * Returns all the selected tiles.
     * @return the selected tiles
     */
    public TileOnMapList getTiles() {
	return tiles;
    }

    /**
     * Returns an iterator over the selected tiles.
     * @return an iterator over the selected tiles.
     */
    public Iterator iterator() {
	return tiles.iterator();
    }

    /**
     * Returns whether or not a tile is selected.
     * @param tile a tile
     * @return true if the tile is selected, false otherwise
     */
    public boolean isSelected(TileOnMap tile) {
	return tiles.contains(tile);
    }

    /**
     * Returns the number of tiles selected.
     * @return the number of tiles selected.
     */
    public int getNbTilesSelected() {
	return tiles.size();
    }

    /**
     * Returns whether or not the selection is empty.
     * @return true if there is no tile selected, false otherwise
     */
    public boolean isEmpty() {
	return tiles.size() == 0;
    }

    /**
     * Selects a tile.
     * @param tile the tile to select
     */
    public void select(TileOnMap tile) {
	if (!isSelected(tile)) {
	    tiles.add(tile);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Unselects a tile.
     * @param tile the tile to unselect
     */
    public void unSelect(TileOnMap tile) {
	if (tiles.contains(tile)) {
	    tiles.remove(tile);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Unselects all tiles.
     */
    public void unSelectAll() {
	tiles.clear();
	setChanged();
	notifyObservers();
    }

    /**
     * Changes the selection state of a tile.
     * @param tile the tile to select or unselect
     */
    public void switchSelection(TileOnMap tile) {
	if (isSelected(tile)) {
	    unSelect(tile);
	}
	else {
	    select(tile);
	}
    }

    /**
     * Removes the selected tiles from the map.
     * If there is no tile selecting, nothing is done.
     */
    public void removeFromMap() throws MapException {

	if (tiles.size() > 0) {
	    map.getHistory().doAction(new ActionRemoveTiles(map, tiles));
	}
    }

 //    /**
//      * Selects the first tile under a point of the map, in the three layers,
//      * starting with the high layer.
//      * @param x x of the point
//      * @param y y of the point
//      * @return true if a tile was found and selected, false otherwise
//      */
//     public boolean select(int x, int y) {

// 	TileOnMap tile = map.getTileAt(x, y);

// 	if (tile == null) {
// 	    return false;
// 	}

// 	select(tile);
// 	return true;
//     }

//     /**
//      * Selects the first tile under a point of the map, in a specified layer.
//      * @param layer a layer: Tile.LAYER_LOW, Tile.LAYER_INTERMEDIATE or Tile.LAYER_HIGH
//      * @param x x of the point
//      * @param y y of the point
//      * @return true if a tile was found and selected, false otherwise
//      */
//     public boolean select(int layer, int x, int y) {

// 	TileOnMap tile = map.getTileAt(layer, x, y);

// 	if (tile == null) {
// 	    return false;
// 	}

// 	select(tile);
// 	return true;
//     }

    /**
     * Returns the layer of the selected tiles, if all selected tiles have the same layer.
     * Otherwise, returns -1.
     * @return the common layer, or -1 if all selected tiles have not the same layer
     */
    public int getLayer() {
	
	int layer = tiles.get(0).getLayer();
	
	for (int i = 1; i < tiles.size(); i++) {
	    if (tiles.get(i).getLayer() != layer) {
		return -1;
	    }
	}

	return layer;
    }

    /**
     * Changes the layer of the selected tiles.
     * @param layer the new layer
     */
    public void setLayer(int layer) throws MapException {

	map.getHistory().doAction(new ActionChangeLayer(map, tiles, layer));
    }

    /**
     * Brings the selected tiles to the front in their layer.
     */
    public void bringToFront() throws MapException {

	map.getHistory().doAction(new ActionBringToFront(map, tiles));
    }

    /**
     * Brings the selected tiles to the back in their layer.
     */
    public void bringToBack() throws MapException {

	map.getHistory().doAction(new ActionBringToBack(map, tiles));
    }

//     /**
//      * Changes the position of the selected tiles.
//      * @param dx number of pixels to move on x
//      * @param dy number of pixels to move on y
//      */
//     public void move(int dx, int dy) throws MapException {

// 	map.getHistory().doAction(new ActionMoveTiles(map, tiles, dx, dy));
//     }
}
