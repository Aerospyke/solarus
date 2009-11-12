/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package zsdx;

import zsdx.gui.*;

/**
 * Main class of the map editor.
 */
public class MapEditor {

	// initialization
	/* Java 1.6 Update 10 introduces by default a Direct3D handling for the Java2D operations,
	 * but under Windows drawing an image without transparency becomes extremely slow
	 * so we disable Direct3D until the bug is fixed.
	 */
	static {
		System.setProperty("sun.java2d.d3d", "false"); 
	}

    /**
     * Entry point.
     */
    public static void main(String[] args) {

	// create the main window
	MapEditorWindow window = new MapEditorWindow();
	window.pack();
	window.setExtendedState(MapEditorWindow.MAXIMIZED_BOTH);
	window.setVisible(true);
    }

}
