package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * A dialog box with some options to edit a map entity.
 */
public class EditEntityDialog extends JDialog {

    /**
     * The entity to edit
     */
    private MapEntity entity;
    
    /**
     * Constructor.
     */
    public EditEntityDialog(MapEntity entity) {

	super((Frame) null, "Edit an entity", true);

	this.entity = entity;

	Container contentPane = getContentPane();
	contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.PAGE_AXIS));
	setResizable(false);
	
	final EditEntityComponent entityComponent = new EditEntityComponent(entity);

	JPanel entityPanel = new JPanel();
	entityPanel.add(entityComponent);
	entityPanel.setBorder(BorderFactory.createTitledBorder("Entity properties"));

	JButton buttonOK = new JButton("OK");
	buttonOK.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    entityComponent.applyModifications();
		    dispose();
		}
	    });

	entityComponent.addKeyListener(new KeyAdapter() {
		public void keyPressed(KeyEvent ev) {
		    if (ev.getKeyCode() == KeyEvent.VK_ENTER) {
			entityComponent.applyModifications();
			dispose();
		    }
		}
	    });

	entityComponent.addKeyListener(new KeyAdapter() {
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

	add(entityPanel);
	add(Box.createVerticalStrut(20));
	add(bottomPanel);
	add(Box.createVerticalStrut(10));
    }
}
