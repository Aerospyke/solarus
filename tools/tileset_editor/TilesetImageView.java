package tileset_editor;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.util.*;
import java.io.*;

/**
 * This component shows the tileset image and allow the user to select the tiles.
 */
public class TilesetImageView extends JComponent implements Observer {

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * The current selected tile.
     */
    private Tile currentSelectedTile;

    // information about the selection

    /**
     * Point where the selection started,
     * or null if there in no new tile selection.
     */
    private Point selectionStartPoint;

    /**
     * Point where the selection ends currently,
     * or null if there in no new tile selection.
     */
    private Point selectionCurrentPoint;

    /**
     * A popup menu with an item "Create", displayed when the user selects a new tile.
     */
    private JPopupMenu popupMenuCreate;

    /**
     * A popup menu with an item "Delete", displayed when the user right-clicks on a tile.
     */
    private JPopupMenu popupMenuDelete;

    /**
     * Constructor.
     */
    public TilesetImageView() {
	super();

	addMouseListener(new TilesetImageMouseListener());
	addMouseMotionListener(new TilesetImageMouseMotionListener());

	JMenuItem item;
	JMenuItem itemCancelCreate = new JMenuItem("Cancel");

	// popup menu to create a tile
	popupMenuCreate = new JPopupMenu();
	item = new JMenuItem("Create");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    tileset.addTile();
		}
	    });
	popupMenuCreate.add(item);
	itemCancelCreate.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    tileset.setSelectedTileIndex(-1);
		}
	    });
	popupMenuCreate.add(itemCancelCreate);

	// popup menu to delete a tile
	popupMenuDelete = new JPopupMenu();
	item = new JMenuItem("Delete");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    tileset.removeTile();
		}
	    });
	popupMenuDelete.add(item);
	popupMenuDelete.add(new JMenuItem("Cancel"));

    }

    /**
     * Returns the tileset's image size.
     * If no image is loaded, returns a default size.
     * @return the image size or a default size.
     */
    public Dimension getPreferredSize() {
	int width, height;

	if (!isImageLoaded()) {
	    width = 500;
	    height = 500;
	}
	else {
	    Image scaledImage = tileset.getDoubleImage();
	    width = scaledImage.getWidth(tileset);
	    height = scaledImage.getHeight(tileset);
	}

	return new Dimension(width, height);
    }

    /**
     * Sets the observed tileset.
     */
    public void setTileset(Tileset tileset) {
	this.tileset = tileset;
	tileset.addObserver(this);

	// load the tileset's image
	update(tileset, null);
    }

    /**
     * This function is called when the tileset changes.
     */
    public void update(Observable o, Object obj) {

	// observe the new selected tile
	Tile newSelectedTile = tileset.getSelectedTile();
	if (newSelectedTile != currentSelectedTile) {

	    if (currentSelectedTile != null) {
		currentSelectedTile.deleteObserver(this);
	    }

	    if (newSelectedTile != null) {
		newSelectedTile.addObserver(this);
	    }

	    currentSelectedTile = newSelectedTile;
	}

	// redraw the image
	repaint();
    }

    /**
     * Returns true if the image exists.
     * @return true if the tileset image exists
     */
    public boolean isImageLoaded() {
	return tileset != null && tileset.getDoubleImage() != null;
    }

    /**
     * This function is called to display the component.
     * @param g the graphic context
     */
    public void paint(Graphics g) {

	if (isImageLoaded()) { // the image exists

	    Tile selectedTile = null;
	    Image scaledImage = tileset.getDoubleImage();

	    // draw the image
// 	    g.drawImage(scaledImage, 0, 0, scaledImage.getWidth(this) * 2, scaledImage.getHeight(this) * 2, this);
	    g.drawImage(scaledImage, 0, 0, tileset);

	    // determine the selected area
	    Rectangle selectedRectangle = null;
	    if (tileset.isSelectingNewTile()) {

		// the selected tile doesn't exist yet
		selectedRectangle = tileset.getNewTileArea();

		if (tileset.isNewTileAreaOverlapping()) {
		    // invalid area
		    g.setColor(Color.RED);
		}
		else {
		    // valid area
		    g.setColor(Color.GREEN);
		}
	    }
	    else {
		selectedTile = tileset.getSelectedTile();

		if (selectedTile != null) {
		    // an existing tile is selected
		    selectedRectangle = selectedTile.getPositionInTileset();
		    g.setColor(Color.BLUE);
		}
	    }
	    
	    // draw the selected rectangle
	    if (selectedRectangle != null) {

		int x1 = selectedRectangle.x * 2;
		int x2 = (selectedRectangle.x + selectedRectangle.width) * 2 - 1;
		int y1 = selectedRectangle.y * 2;
		int y2 = (selectedRectangle.y + selectedRectangle.height) * 2 - 1;

		g.drawLine(x1, y1, x2, y1);
		g.drawLine(x2, y1, x2, y2);
		g.drawLine(x2, y2, x1, y2);
		g.drawLine(x1, y2, x1, y1);

		g.drawLine(x1, y1 + 1, x2, y1 + 1);
		g.drawLine(x2 - 1, y1, x2 - 1, y2);
		g.drawLine(x2, y2 - 1, x1, y2 - 1);
		g.drawLine(x1 + 1, y2, x1 + 1, y1);

		// for an animated tile, also draw the separation between the 3 frames
		if (selectedTile != null && selectedTile.isAnimated()) {

		    if (selectedTile.getAnimationSeparation() == Tile.ANIMATION_SEPARATION_HORIZONTAL) {
			int frame_width = (x2 - x1 + 1) / 3;
			x1 += frame_width;
			x2 -= frame_width;

			g.drawLine(x2, y1, x2, y2);
			g.drawLine(x1, y2, x1, y1);
			g.drawLine(x2 - 1, y1, x2 - 1, y2);
			g.drawLine(x1 + 1, y2, x1 + 1, y1);
		    }
		    else {
			int frame_height = (y2 - y1 + 1) / 3;
			y1 += frame_height;
			y2 -= frame_height;
			
			g.drawLine(x1, y1, x2, y1);
			g.drawLine(x2, y2, x1, y2);
			g.drawLine(x1, y1 + 1, x2, y1 + 1);
			g.drawLine(x2, y2 - 1, x1, y2 - 1);
		    }
		}
	    }

	}
	else if (tileset != null) {

	    // a tileset is open but the image doesn't exist: display an error message
	    g.drawString("Unable to load image '" + tileset.getImagePath() + "'.", 10, 20);
	}
    }

    /**
     * The mouse listener associated to the tileset image.
     */
    private class TilesetImageMouseListener extends MouseAdapter {
	
	/**
	 * This function is called when the user clicks on the tileset image.
	 * If no tile was selected, then the tile clicked (if any) is selected.
	 * For a right click: a popup menu allow to removed the tile clicked.
	 * @param mouseEvent information about the click
	 */
	public void mouseClicked(MouseEvent mouseEvent) {
	    if (isImageLoaded()) {

		Image scaledImage = tileset.getDoubleImage();
		
		int x = Math.min(Math.max(mouseEvent.getX(), 0), scaledImage.getWidth(TilesetImageView.this)) / 2;
		int y = Math.min(Math.max(mouseEvent.getY(), 0), scaledImage.getHeight(TilesetImageView.this)) / 2;
		
		// search the tile clicked
		boolean found = false;
		int clickedTileIndex = -1;
		for (int i = 0; i < tileset.getNbTiles() && !found; i++) {
			
		    Rectangle tileRectangle = tileset.getTile(i).getPositionInTileset();
		    if (tileRectangle.contains(x, y)) {
			found = true;
			clickedTileIndex = i;
		    }
		}
		
		// a tile was just clicked
		if (clickedTileIndex != -1) {

		    // select the tile
		    tileset.setSelectedTileIndex(clickedTileIndex);
		    
		    // right click: show a popup menu to remove the tile
		    if (mouseEvent.getButton() == MouseEvent.BUTTON3) {
			popupMenuDelete.show(TilesetImageView.this,
					     mouseEvent.getX(),
					     mouseEvent.getY());
		    }
		}
	    }
	}
	
	/**
	 * This function is called when the mouse button is pressed on the tileset image.
	 * Only left clicks are considered.
	 * @param mouseEvent information about the click
	 */
	public void mousePressed(MouseEvent mouseEvent) {

	    // only consider left clicks
	    if (isImageLoaded() && mouseEvent.getButton() == MouseEvent.BUTTON1) {
		
		Image scaledImage = tileset.getDoubleImage();

		int x = Math.min(Math.max(mouseEvent.getX(), 0), scaledImage.getWidth(TilesetImageView.this)) / 2;
		int y = Math.min(Math.max(mouseEvent.getY(), 0), scaledImage.getHeight(TilesetImageView.this)) / 2;

		Tile selectedTile = tileset.getSelectedTile();

		if (selectedTile != null && !selectedTile.getPositionInTileset().contains(x,y)) {
		    // an existing tile was selected and the user pressed the mouse button outside: unselect it
		    tileset.setSelectedTileIndex(-1);
		}

		else if (tileset.isSelectingNewTile()) {
		    // a new tile was selected: unselect it
		    tileset.setSelectedTileIndex(-1);
		}

		// begin a selection
		if (!tileset.isSelectingNewTile()) {
		    selectionStartPoint = mouseEvent.getPoint();

		    selectionStartPoint.x = (x + 4) / 8 * 8;
		    selectionStartPoint.y = (y + 4) / 8 * 8;
		}
	    }
	}

	/**
	 * This function is called when the mouse button is released on the tileset image.
	 * Only left clicks are considered.
	 * @param mouseEvent information about the click
	 */
	public void mouseReleased(MouseEvent mouseEvent) {

	    // only consider left clicks
	    if (isImageLoaded() && mouseEvent.getButton() == MouseEvent.BUTTON1) {

		Image scaledImage = tileset.getDoubleImage();

		// keep the new selected tile only if it really exists
		Rectangle newTileArea = tileset.getNewTileArea();
		if (newTileArea != null
		    && newTileArea.width > 0
		    && newTileArea.height > 0
		    && !tileset.isNewTileAreaOverlapping()) {

		    // the area is valid: show a popup menu with an item "Create"
		    popupMenuCreate.show(TilesetImageView.this,
					 mouseEvent.getX(),
					 mouseEvent.getY());
		}
		else {
		    // the area doesn't exist or is not valid: we cancel the selection
		    selectionStartPoint = null;
		    selectionCurrentPoint = null;
		    tileset.setSelectedTileIndex(-1);
		    newTileArea = null;
		}
	    }
	}
    }

    /**
     * The mouse motion listener associated to the tileset image.
     */
    private class TilesetImageMouseMotionListener extends MouseMotionAdapter {
	
	/**
	 * This method is called when a mouse button is pressed on the component and then dragged.
	 * It is called again until the mouse button is released.
	 */
	public void mouseDragged(MouseEvent mouseEvent) {

	    if (isImageLoaded() && selectionStartPoint != null) {

		Image scaledImage = tileset.getDoubleImage();

		// compute the selected area
		Point selectionPreviousPoint = selectionCurrentPoint;
		selectionCurrentPoint = mouseEvent.getPoint();

		selectionCurrentPoint.x = Math.min(Math.max(mouseEvent.getX(), 0), scaledImage.getWidth(TilesetImageView.this));
		selectionCurrentPoint.y = Math.min(Math.max(mouseEvent.getY(), 0), scaledImage.getHeight(TilesetImageView.this));

		selectionCurrentPoint.x = (selectionCurrentPoint.x + 8) / 16 * 8;
		selectionCurrentPoint.y = (selectionCurrentPoint.y + 8) / 16 * 8;
		
		if (!selectionCurrentPoint.equals(selectionPreviousPoint)) {

		    Rectangle newTileArea = new Rectangle();

		    // the selected area has changed: recalculate the rectangle
		    if (selectionStartPoint.x < selectionCurrentPoint.x) {
			newTileArea.x = selectionStartPoint.x;
			newTileArea.width = selectionCurrentPoint.x - selectionStartPoint.x;
		    }
		    else {
			newTileArea.x = selectionCurrentPoint.x;
			newTileArea.width = selectionStartPoint.x - selectionCurrentPoint.x;
		    }
		    
		    if (selectionStartPoint.y < selectionCurrentPoint.y) {
			newTileArea.y = selectionStartPoint.y;
			newTileArea.height = selectionCurrentPoint.y - selectionStartPoint.y;
		    }
		    else {
			newTileArea.y = selectionCurrentPoint.y;
			newTileArea.height = selectionStartPoint.y - selectionCurrentPoint.y;
		    }
		    
		    tileset.setSelectedTileIndex(tileset.getNbTiles());
		    tileset.setNewTileArea(newTileArea);
		}
	    }
	}
    }
}
