package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import java.io.*;

/**
 * A combo box component to select an element from the resource database.
 */
public class ResourceChooser extends JComboBox implements Observer {

    /**
     * The kind of resource displayed in the combo box (maps, musics...).
     */
    private int resourceType;
    
    /**
     * Indicates whether the first element of the combo box should be
     * an empty option (showing that no element is selected).
     */
    private boolean showEmptyOption;

    /**
     * Constructor.
     * @param resourceType the kind of resource displayed in the combo box:
     * ResourceDatabase.RESOURCE_MAPS, ResourceDatabase.RESOURCE_MUSICS, etc.
     */
    public ResourceChooser(int resourceType, boolean showEmptyOption) {

	super();
	this.resourceType = resourceType;
	this.showEmptyOption = showEmptyOption;

	Configuration.getInstance().addObserver(this);
	update(Configuration.getInstance(), null);
    }

    /**
     * This function is called when the configuration is changed.
     * The list is reloaded using the game resource database.
     */
    public void update(Observable o, Object obj) {

	removeAllItems();
	buildList();
    }

    /**
     * Loads the list using the game resource database.
     */
    protected void buildList() {

	if (showEmptyOption) {
	    addItem(new KeyValue("", ""));
	}

	try {
	
	    Resource resource = ResourceDatabase.getResource(resourceType);
	    String[] ids = resource.getIds();
	    String name;
	    
	    for (int i = 0; i < ids.length; i++) {
		name = resource.getElementName(ids[i]);
		addItem(new KeyValue(ids[i], name));
	    }
	}
	catch (ZSDXException ex) {
	    WindowTools.errorDialog("Unexpected error: " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Returns the id of the currently selected element.
     * @return the id of the selected element, or an empty string if no element is selected
     */
    public String getSelectedId() {

	KeyValue item = (KeyValue) getSelectedItem();

	if (item == null) {
	    return "";
	}

	return item.getKey();
    }

    /**
     * Changes the currently selected elements.
     * @param id id of the element you want to make selected in the combo box,
     * or an empty string to select no element
     */
    public void setSelectedId(String id) {

	KeyValue item = new KeyValue(id, null);
	setSelectedItem(item);
    }
}
