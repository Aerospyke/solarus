package zsdx;

import java.awt.*;
import java.util.*;

/**
 * Represents an item that Link can pick on the map,
 * such as a rupee, a heart, a key, etc.
 */
public class PickableItem extends InteractiveEntity {

    // types of pickable items

    public static final int PICKABLE_ITEM_RANDOM      = -1;   /**< special value to indicate to choose another value
							       * randomly (including PICKABLE_ITEM_NONE) */
    public static final int PICKABLE_ITEM_NONE        = 0;    /**< special value to incicate that there is no pickable item */
    public static final int PICKABLE_ITEM_RUPEE_1     = 1;
    public static final int PICKABLE_ITEM_RUPEE_5     = 2;
    public static final int PICKABLE_ITEM_RUPEE_20    = 3;
    public static final int PICKABLE_ITEM_HEART       = 4;
    public static final int PICKABLE_ITEM_SMALL_MAGIC = 5;
    public static final int PICKABLE_ITEM_BIG_MAGIC   = 6;
    public static final int PICKABLE_ITEM_FAIRY       = 7;
    public static final int PICKABLE_ITEM_BOMB_1      = 8;
    public static final int PICKABLE_ITEM_BOMB_5      = 9;
    public static final int PICKABLE_ITEM_BOMB_10     = 10;
    public static final int PICKABLE_ITEM_ARROW_1     = 11;
    public static final int PICKABLE_ITEM_ARROW_5     = 12;
    public static final int PICKABLE_ITEM_ARROW_10    = 13;
    
    /**
     * Creates a new pickable item at the specified location.
     * @param map the map
     * @param x x coordinate of the item
     * @param y y coordinate of the item
     */
    public PickableItem(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);

	// TODO default field values
    }

    /**
     * Creates an existing pickable item from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public PickableItem(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);
	setSizeImpl(16, 16);

	// TODO parse the fields
    }

    /**
     * Returns a string describing this pickable item.
     * @return a string representation of the pickable item
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string
	buff.append(super.toString());

	// TODO add the specific properties of a pickable item
	buff.append('\t');

	return buff.toString();
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_PICKABLE_ITEM
     */
    public int getType() {
	return ENTITY_PICKABLE_ITEM;
    }

    /**
     * Returns the number of directions of the entity.
     * @return 0 (a pickable item has no direction)
     */
    public int getNbDirections() {
	return 0;
    }
    
    /**
     * Returns whether the entity has an identifier.
     * @return false
     */
    public boolean hasName() {
	return false;
    }

    /**
     * Returns whether or not the entity is resizable.
     * A pickable item is not resizable.
     * @return false
     */
    public boolean isResizable() {
	return false;
    }

    /**
     * Draws the pickable item on the map editor.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {
	// TODO
    }

}
