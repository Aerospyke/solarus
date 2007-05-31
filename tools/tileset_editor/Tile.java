package tileset_editor;

import java.awt.*;
import java.io.*;
import java.util.*;

/**
 * Represents a tile for the tileset editor.
 */
public class Tile extends Observable implements Serializable {

    /**
     * Version number of the class serialization.
     */
    public static final long serialVersionUID = 1L;

    // Constants to identify the obstacles

    public static final int OBSTACLE_NONE = 0;
    public static final int OBSTACLE = 1;
    public static final int OBSTACLE_TOP_RIGHT = 2;
    public static final int OBSTACLE_TOP_LEFT = 3;
    public static final int OBSTACLE_BOTTOM_LEFT = 4;
    public static final int OBSTACLE_BOTTOM_RIGHT = 5;

    // Constants to identify the animation type

    public static final int ANIMATION_NONE = 0;
    public static final int ANIMATION_SEQUENCE_012 = 1;
    public static final int ANIMATION_SEQUENCE_0121 = 2;

    public static final int ANIMATION_SEPARATION_HORIZONTAL = 0;
    public static final int ANIMATION_SEPARATION_VERTICAL = 1;

    /**
     * Coordinates and dimensions of the tile.
     */
    private Rectangle positionInTileset;

    /**
     * Type of obstacle.
     */
    private int obstacle;

    /**
     * Animation sequence.
     */
    private int animationSequence;

    /**
     * Type of separation of the 3 animation frames in the tileset.
     */
    private int animationSeparation;

    /**
     * Simple constructor with default parameters: obstacle and no animation.
     */
    public Tile(Rectangle positionInTileset) {
	this(positionInTileset, OBSTACLE_NONE, ANIMATION_NONE, 0);
    }

    /**
     * Constructor.
     */
    public Tile(Rectangle positionInTileset, int obstacle,
		int animationSequence, int animationSeparation) {
	super();

	this.positionInTileset = positionInTileset;
	this.obstacle = obstacle;
	this.animationSequence = animationSequence;
	this.animationSeparation = animationSeparation;
    }

    /**
     * Returns the tile's rectangle.
     * @return the tile's rectangle
     */
    public Rectangle getPositionInTileset() {
	return positionInTileset;
    }

    /**
     * Returns the tile's obstacle property.
     * @return Tile.OBSTACLE_NONE, Tile.OBSTACLE, Tile.OBSTACLE_TOP_RIGHT,
     * Tile.OBSTACLE_TOP_LEFT, Tile.OBSTACLE_BOTTOM_LEFT or Tile.OBSTACLE_BOTTOM_RIGHT
     */
    public int getObstacle() {
	return obstacle;
    }

    /**
     * Changes the tile's obstacle property.
     * @param obstacle Tile.OBSTACLE_NONE, Tile.OBSTACLE, Tile.OBSTACLE_TOP_RIGHT,
     * Tile.OBSTACLE_TOP_LEFT, Tile.OBSTACLE_BOTTOM_LEFT or Tile.OBSTACLE_BOTTOM_RIGHT
     */
    public void setObstacle(int obstacle) {
	this.obstacle = obstacle;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the tile's animation sequence.
     * @return the tile's animation sequence: Tile.NO_ANIMATION,
     * Tile.ANIMATION_012 or Tile.ANIMATION_0121
     */
    public int getAnimationSequence() {
	return animationSequence;
    }

    /**
     * Sets the tile's animation sequence.
     * @param animationSequence the tile's animation sequence: Tile.NO_ANIMATION,
     * Tile.ANIMATION_012 or Tile.ANIMATION_0121
     */
    public void setAnimationSequence(int animationSequence) {
	this.animationSequence = animationSequence;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the type of separation of the 3 animation frames if the tile is animated.
     * @return the type of separation of the 3 animation frames:
     * Tile.ANIMATION_HORIZONTAL_SEPARATION or Tile.ANIMATION_VERTICAL_SEPARATION
     */
    public int getAnimationSeparation() {
	return animationSeparation;
    }

    /**
     * Sets the type of separation of the 3 animation frames if the tile is animated.
     * @param animationSeparation the type of separation of the 3 animation frames:
     * Tile.ANIMATION_HORIZONTAL_SEPARATION or Tile.ANIMATION_VERTICAL_SEPARATION
     */
    public void setAnimationSeparation(int animationSeparation) {
	this.animationSeparation = animationSeparation;
	setChanged();
	notifyObservers();
    }

}
