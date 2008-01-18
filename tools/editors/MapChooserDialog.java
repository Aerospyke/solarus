package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * A dialog box with a combo box to choose a map.
 */
public class MapChooserDialog extends JDialog {

    /**
     * Id of the tileset selected (or -1 if the user cancelled).
     */
    private int mapId = -1;

    /**
     * Constructor.
     */
    public MapChooserDialog() {

	super((Frame) null, "Select a map", true);

	Container contentPane = getContentPane();
	contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.PAGE_AXIS));
	setMinimumSize(new Dimension(300, 0));
	setResizable(false);
	
	final MapChooser mapChooser = new MapChooser();

	JPanel mapChooserPanel = new JPanel();
	mapChooserPanel.add(mapChooser);
	mapChooserPanel.setBorder(BorderFactory.createTitledBorder("Please select a map"));

	JButton buttonOK = new JButton("OK");
	buttonOK.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    mapId = mapChooser.getSelectedMapId();
		    dispose();
		}
	    });

	mapChooser.addKeyListener(new KeyAdapter() {
		public void keyPressed(KeyEvent ev) {
		    if (ev.getKeyCode() == KeyEvent.VK_ENTER) {
			mapId = mapChooser.getSelectedMapId();
			dispose();
		    }
		}
	    });

	mapChooser.addKeyListener(new KeyAdapter() {
		public void keyPressed(KeyEvent ev) {
		    if (ev.getKeyCode() == KeyEvent.VK_ESCAPE) {
			dispose();
		    }
		}
	    });

	JButton buttonCancel = new JButton("Cancel");
	buttonCancel.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    dispose();
		}
	    });

	GridLayout gridLayout = new GridLayout(1, 2); 
	JPanel buttonPanel = new JPanel(gridLayout);
	gridLayout.setHgap(10);

	buttonPanel.add(buttonCancel);
	buttonPanel.add(buttonOK);

	JPanel bottomPanel = new JPanel();
	bottomPanel.setLayout(new BoxLayout(bottomPanel, BoxLayout.LINE_AXIS));

	bottomPanel.add(Box.createHorizontalGlue());
	bottomPanel.add(buttonPanel);

	add(mapChooserPanel);
	add(Box.createVerticalStrut(20));
	add(bottomPanel);
	add(Box.createVerticalStrut(10));
    }

    /**
     * Returns the id of map that the user has just selected.
     * @return the id of the map, or -1 if he cancelled
     */
    public int getMapId() {

	int id = mapId;
	mapId = -1;
	return id;
    }
}
