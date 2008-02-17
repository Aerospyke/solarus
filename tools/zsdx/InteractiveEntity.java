package zsdx;

/**
 * This class represents an interactive map entity, such as a chest, a door, a switch...
 */
public abstract class InteractiveEntity extends MapEntity {

    /**
     * Constructor.
     * @param map the map
     * @param layer layer of the entity
     * @param x x position of the entity on the map
     * @param y y position of the entity on the map
     * @param width width of the entity
     * @param height height of the entity
     */
    protected InteractiveEntity(Map map, int layer, int x, int y, int width, int height) {
	super(map, layer, x, y, width, height);
    }

    /**
     * Empty constructor.
     */
    protected InteractiveEntity() {
	super();
    }
}
