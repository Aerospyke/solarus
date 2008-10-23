package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import java.util.Hashtable;
import zsdx.*;
import zsdx.entities.*;

/**
 * A component to set the rendering options of the map view,
 * i.e. what tiles are displayed depending on their layer, etc.
 */
public class MapViewRenderingOptionsView extends JPanel {

    /**
     * The options visualised by this component.
     */
    private MapViewRenderingOptions renderingOptions;

    /**
     * Constructor.
     * @param renderingOptions the options to visualise with this component
     */
    public MapViewRenderingOptionsView(MapViewRenderingOptions renderingOptions) {
	super(new BorderLayout());
	
	this.renderingOptions = renderingOptions;

	ZoomChooser zoomChooser = new ZoomChooser();
	JPanel boxesPanel = new JPanel(new GridLayout(2, 3));
	
	JCheckBox box;

	box = new JCheckBox("Show low layer");
	box.setSelected(renderingOptions.getShowLayer(MapEntity.LAYER_LOW));
	box.addItemListener(new ItemListenerLayer(MapEntity.LAYER_LOW));
	boxesPanel.add(box);

	box = new JCheckBox("Show intermediate layer");
	box.setSelected(renderingOptions.getShowLayer(MapEntity.LAYER_INTERMEDIATE));
	box.addItemListener(new ItemListenerLayer(MapEntity.LAYER_INTERMEDIATE));
	boxesPanel.add(box);

	box = new JCheckBox("Show high layer");
	box.setSelected(renderingOptions.getShowLayer(MapEntity.LAYER_HIGH));
	box.addItemListener(new ItemListenerLayer(MapEntity.LAYER_HIGH));
	boxesPanel.add(box);

	box = new JCheckBox("Show obstacles");
	box.setSelected(renderingOptions.getShowObstacle(MapEntity.OBSTACLE));
	box.addItemListener(new ItemListenerObstacle(MapEntity.OBSTACLE));
	boxesPanel.add(box);

	box = new JCheckBox("Show non obstacles");
	box.setSelected(renderingOptions.getShowObstacle(MapEntity.OBSTACLE_NONE));
	box.addItemListener(new ItemListenerObstacle(MapEntity.OBSTACLE_NONE));
	boxesPanel.add(box);

	box = new JCheckBox("Show transparency");
	box.setSelected(renderingOptions.getShowTransparency());
	box.addItemListener(new ItemListenerTransparency());
	boxesPanel.add(box);
	
	add(zoomChooser, BorderLayout.WEST);
	add(boxesPanel, BorderLayout.CENTER);
    }

    /**
     * Listener invoked when the state of a layer checkbox has changed. 
     */
    private class ItemListenerLayer implements ItemListener {

	/**
	 * The layer controlled by this checkbox.
	 */
	private int layer;

	/**
	 * Constructor.
	 * @param layer the layer controlled by the checkbox.
	 */
	public ItemListenerLayer(int layer) {
	    this.layer = layer;
	}

	/**
	 * Method invoked when the user clicks on the checkbox.
	 */
	public void itemStateChanged(ItemEvent itemEvent) {

	    Map map = renderingOptions.getMap();
	    
	    if (map != null) {
		// unselect everything
		map.getEntitySelection().unSelectAll();
	    }

	    // get the new checkbox state
	    boolean show = (itemEvent.getStateChange() == ItemEvent.SELECTED);
	    
	    // update the options
	    renderingOptions.setShowLayer(layer, show);
	}
    }

    /**
     * Listener invoked when the state of an obstacle checkbox has changed. 
     */
    private class ItemListenerObstacle implements ItemListener {

	/**
	 * The type of obstacle controlled by this checkbox.
	 */
	private int obstacle;

	/**
	 * Constructor.
	 * @param obstacle the type of obstacle controlled by the checkbox.
	 */
	public ItemListenerObstacle(int obstacle) {
	    this.obstacle = obstacle;
	}

	/**
	 * Method invoked when the user clicks on the checkbox.
	 */
	public void itemStateChanged(ItemEvent itemEvent) {
	    
	    Map map = renderingOptions.getMap();
	    
	    if (map != null) {
		// unselect everything
		map.getEntitySelection().unSelectAll();
	    }

	    // get the new checkbox state
	    boolean show = (itemEvent.getStateChange() == ItemEvent.SELECTED);

	    // update the options
	    renderingOptions.setShowObstacle(obstacle, show);
	}
    }

    /**
     * Listener invoked when the state of the transparency checkbox has changed.
     */
    private class ItemListenerTransparency implements ItemListener {

	/**
	 * Constructor.
	 */
	public ItemListenerTransparency() {

	}

	/**
	 * Method invoked when the user clicks on the checkbox.
	 */
	public void itemStateChanged(ItemEvent itemEvent) {
	    
	    // get the new checkbox state
	    boolean show = (itemEvent.getStateChange() == ItemEvent.SELECTED);

	    // update the options
	    renderingOptions.setShowTransparency(show);
	}
    }

    /**
     * A component to select the zoom applied to the map view.
     */
    private class ZoomChooser extends JPanel implements ChangeListener {

	private final double[] zooms = {0.25, 0.5, 1, 2}; 

	private JSlider slider;

	public ZoomChooser() {
	    super();

	    setBorder(BorderFactory.createTitledBorder("Zoom"));

	    slider = new JSlider(0, 3, 3);
	    add(slider, BorderLayout.CENTER);

	    Hashtable<Integer, JLabel> labelTable = new Hashtable<Integer, JLabel>();
	    labelTable.put(0, new JLabel("25%"));
	    labelTable.put(1, new JLabel("50%"));
	    labelTable.put(2, new JLabel("100%"));
	    labelTable.put(3, new JLabel("200%"));
	    slider.setLabelTable(labelTable);

	    slider.setMajorTickSpacing(10);
	    slider.setMinorTickSpacing(1);
	    slider.setPaintTicks(true);
	    slider.setPaintLabels(true);
	    slider.setSnapToTicks(true);

	    slider.addChangeListener(this);
	}

	public void stateChanged(ChangeEvent ev) {
	    renderingOptions.setZoom(zooms[slider.getValue()]);
	}
    }
}
