package tileset_editor;

import java.awt.*;
import javax.swing.*;
import javax.swing.table.*;
import javax.swing.event.*;
import java.util.*;

/**
 * This component shows a list of tiles and allows the user to edit them.
 */
public class TileList extends JPanel implements Observer {

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * Text indicating the tileset name.
     */
    private JLabel labelTilesetName;

    /**
     * The tile table.
     */
    private JTable tileTable;

    /**
     * The tile table model.
     */
    private TileTableModel tileTableModel;

    // the buttons
    private JButton buttonAdd;
    private JButton buttonRemove;

    /**
     * Constructor.
     */
    public TileList() {
	super();

	setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

	// north : a vertical box layout with the tileset name and the buttons
	// center : the table

	// tileset name
	labelTilesetName = new JLabel("Tileset name: ");
	labelTilesetName.setMaximumSize(new Dimension(400, 30));
	labelTilesetName.setAlignmentX(Component.LEFT_ALIGNMENT);

	// buttons
	JPanel buttons = new JPanel(new GridLayout(1, 3, 5, 0));
	buttons.setMaximumSize(new Dimension(200, 30));
	buttons.setAlignmentX(Component.LEFT_ALIGNMENT);

	add(Box.createHorizontalGlue());
	add(Box.createHorizontalGlue());
	buttonAdd = new JButton("Add");
	buttonRemove = new JButton("Remove");
	buttonAdd.setEnabled(false);
	buttonRemove.setEnabled(false);
	buttons.add(buttonAdd);
	buttons.add(buttonRemove);


	// table
	tileTableModel = new TileTableModel();
	tileTable = new JTable(tileTableModel);
	tileTable.setColumnSelectionAllowed(false);
	tileTable.setRowSelectionAllowed(true);
	tileTable.setDragEnabled(false);
	tileTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

	tileTable.getSelectionModel().addListSelectionListener(new TileListSelectionListener());

	JScrollPane tableScroll = new JScrollPane(tileTable);
	tableScroll.setAlignmentX(Component.LEFT_ALIGNMENT);

	add(labelTilesetName);
	add(Box.createRigidArea(new Dimension(0, 5)));
	add(buttons);
	add(Box.createRigidArea(new Dimension(0, 5)));
	add(tableScroll);
    }

    /**
     * Sets the observed tileset.
     */
    public void setTileset(Tileset tileset) {
	this.tileset = tileset;
	tileset.addObserver(this);

	update(tileset, null);
    }

    /**
     * This function is called when the tileset changes.
     */
    public void update(Observable o, Object obj) {

	// update the tileset name
	labelTilesetName.setText("Tileset name: " + tileset.getName());

	// update the enabled state of the buttons
	int selectedTileIndex = tileset.getSelectedTileIndex();
	int selectedRow = tileTable.getSelectedRow();

 	System.out.println("updating the tiletable: selected row = " + selectedRow +
 			   ", selected tile = " + selectedTileIndex);

	buttonAdd.setEnabled(false);
	buttonRemove.setEnabled(false);

	if (selectedTileIndex == tileset.getNbTiles()) {
	    // a new tile is selected, let's authorise the user to create it
	    buttonAdd.setEnabled(true);
	}
	else if (selectedTileIndex >= 0) {
	    // an existing tile is selected, so the user can remove it
	    buttonRemove.setEnabled(true);

	    if (selectedRow != selectedTileIndex) {
		System.out.println("selecting row " + selectedTileIndex);
		tileTable.getSelectionModel().setSelectionInterval(selectedTileIndex, selectedTileIndex);
		System.out.println("done");
	    }
	}
	else {
	    // no tile is selected
	    if (selectedRow != -1) {
		System.out.println("trying to unselect\n");
		tileTable.getSelectionModel().removeSelectionInterval(selectedRow, selectedRow);
	    }
	}


	// update the tile list
	tileTable.repaint();
	//	tileTableModel.fireTableDataChanged();

	// also select the row in the table
// 	System.out.println("updating the tiletable selection: selected row = " + tileTable.getSelectedRow() +
// 			   ", selected tile = " + selectedTileIndex);
// 	if (tileTable.getSelectedRow() != selectedTileIndex) {
// 	    System.out.println("selecting row " + selectedTileIndex);
// 	    tileTable.setRowSelectionInterval(selectedTileIndex, selectedTileIndex);
// 	    System.out.println("done");
// 	}

    }

    /**
     * The table model for the tile table.
     */
    private class TileTableModel extends AbstractTableModel {

	/**
	 * The column names.
	 */
	private final String[] columnNames = {
	    "Image",
	    "Coordinates",
	    "Obstacle",
	    "Animation"
	};

	/**
	 * Returns the number of rows.
	 * @return the number of rows
	 */
	public int getRowCount() {
	    if (tileset == null) {
		// no tileset loaded yet
		return 0;
	    }
	    else {
		// number of tiles + a header row
		return tileset.getNbTiles();
	    }
	}

	/**
	 * Returns the number of columns.
	 * @return the number of columns
	 */
	public int getColumnCount() {
	    return 4;
	}

	/**
	 * Returns the name of a column.
	 * @param column number
	 * @return name of this column
	 */
	public String getColumnName(int column) {
	    return columnNames[column];
	}

	/**
	 * Returns whether a cell is editable or not.
	 * @param row the cell's row
	 * @param column the cell's column
	 * @return true if the cell is editable, false otherwise
	 */
	public boolean isCellEditable(int row, int column) {
	    // all columns are editable except the first one (the image)
	    return row > 0 && column > 0;
	}

	/**
	 * Returns the object to display in a cell.
	 * @param row the cell's row
	 * @param column the cell's column
	 * @return the object to display in this cell
	 */
	public Object getValueAt(int row, int column) {
	    return "1";
	}
    }

    /**
     * List selection listener associated to the tile table.
     * When the user selects a row, the corresponding tileset becomes selected in the image.
     */
    private class TileListSelectionListener implements ListSelectionListener {

	/**
	 * This function is called when the selection is changed.
	 */
	public void valueChanged(ListSelectionEvent e) {
	    // get the row whose state has changed
	    int changedRow = e.getFirstIndex();

	    System.out.println("row " + changedRow + " has changed");

	    if (tileTable.isRowSelected(changedRow)) {
		System.out.println("it became selected");
		// the row has just been selected		
		tileset.setSelectedTileIndex(changedRow);
	    }
	    else {
		System.out.println("it became unselected");
		//throw new RuntimeException("where the hell do i come from?");
		// the row has just been unselected
		tileset.setSelectedTileIndex(-1);
	    }
	}
    }
}
