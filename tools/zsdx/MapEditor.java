package zsdx;

import javax.swing.*;

/**
 * Main class of the map editor.
 */
public class MapEditor {

    /**
     * Entry point.
     */
    public static void main(String[] args) {

	// create the main window
	MapEditorWindow window = new MapEditorWindow();
	window.pack();
	window.setVisible(true);
    }

}
