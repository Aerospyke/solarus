package editors;

import java.io.*;
import java.awt.*;

/**
 * This class provides functionnalities to generate
 * the C++ code corresponding to a tileset.
 */
public class TilesetCodeGenerator {

    private Tileset tileset;       // the tileset
    private String name;           // the tileset's name
    private String upperCaseName;  // the upper case tileset's name
    private String zsdxRootPath;   // the root directory of ZSDX
    private String headerFileName; // name of the header file (without the path)

    /**
     * Symbolic constants for the type of obstacle in the generated code.
     */
    private static final String[] obstacleConstants = {
	"OBSTACLE_NONE",
	"OBSTACLE",
	"OBSTACLE_TOP_RIGHT",
	"OBSTACLE_TOP_LEFT",
	"OBSTACLE_BOTTOM_LEFT",
	"OBSTACLE_BOTTOM_RIGHT"
    };

    /**
     * Symbolic constants for the animation sequence in the generated code.
     */
    private static final String[] animationSequenceConstants = {
	"ANIMATION_SEQUENCE_012",
	"ANIMATION_SEQUENCE_0121"
    };

    /**
     * Constructor.
     */
    public TilesetCodeGenerator() {
    }

    /**
     * Generates the code corresponding to a tileset.
     * @param tileset the tileset
     */
    public void generate(Tileset tileset) throws IOException {
	this.tileset = tileset;
	zsdxRootPath = Configuration.getInstance().getZsdxRootPath();
	name = tileset.getName();
	upperCaseName = name.toUpperCase();
	
	// generate the header file
	generateHeaderFile();

	// generate the source file
	generateSourceFile();

	// update include/TilesetList.inc.h
	updateEnumFile();

	// update src/TilesetsCreation.inc.cpp
	updateCreationFile();
    }

    /**
     * Generates the header file, for example include/tilesets/TilesetHouse.h
     */
    private void generateHeaderFile() throws IOException {

	headerFileName = "Tileset" + name + ".h";
	String fileName = zsdxRootPath + File.separator + "include" +
	    File.separator + "tilesets" + File.separator + headerFileName;

	PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));
	
	out.println("#ifndef ZSDX_TILESET_" + upperCaseName + "_H");
	out.println("#define ZSDX_TILESET_" + upperCaseName + "_H");
	out.println();
	out.println("#include \"Tileset.h\"");
	out.println();
	out.println("/*");
	out.println(" * Tileset generated automatically by the tileset editor.");
	out.println(" */");
	out.println("class Tileset" + name + ": public Tileset {");
	out.println();
	out.println(" public:");
	out.println("  Tileset" + name + "(void);");
	out.println("  inline ~Tileset" + name + "(void) { }");
	out.println();
	out.println("  void load(void);");
	out.println("};");
	out.println();
	out.println("#endif");

	out.close();
    }

    /**
     * Generates the source file, for example src/tilesets/TilesetHouse.cpp
     */
    private void generateSourceFile() throws IOException {

	String fileName = zsdxRootPath + File.separator + "src" +
	    File.separator + "tilesets" + File.separator + "Tileset" +
	    name + ".cpp";

	String className = "Tileset" + name;
	Color bgColor = tileset.getBackgroundColor();

	PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(fileName)));
	
	out.println("/*");
	out.println(" * Tileset generated automatically by the tileset editor.");
	out.println(" */");
	out.println();
	out.println("#include \"tilesets/" + headerFileName + "\"");
	out.println();
	out.println("/**");
	out.println(" * Constructor.");
	out.println(" */");
	out.println(className + "::" + className + "(void):");
	out.println("  Tileset(get_color(" + bgColor.getRed() + ", "
		    + bgColor.getGreen() + ", " + bgColor.getBlue() + ")) {");
	out.println("}");
	out.println();
	out.println("/**");
	out.println(" * Loads the tileset.");
	out.println(" */");
	out.println("void " + className + "::load(void) {");
	out.println("  load_tileset_image(\"images/tilesets/" + name + ".png\");");

	out.println();

	// create each tile
	for (int id: tileset.getTileIds()) {

	    Tile tile = tileset.getTile(id);
	    Rectangle positionInTileset = tile.getPositionInTileset();
	    int x = positionInTileset.x;
	    int y = positionInTileset.y;
	    int width = positionInTileset.width;
	    int height = positionInTileset.height;

	    String obstacleConstant = obstacleConstants[tile.getObstacle()];

	    if (!tile.isAnimated()) {
		// simple tile
		out.println("  SIMPLE_TILE(" + id + ", " + obstacleConstant + ", " + x + ", " + y + ", " + width + ", " + height + ");");
	    }
	    else {
		// animated tile
		int dx, dy;
		if (tile.getAnimationSeparation() == Tile.ANIMATION_SEPARATION_HORIZONTAL) {
		    width = width / 3;
		    dx = width;
		    dy = 0;
		}
		else {
		    height = height / 3;
		    dx = 0;
		    dy = height;
		}

		String animationSequenceConstant = animationSequenceConstants[tile.getAnimationSequence()];
		
		out.println("  ANIMATED_TILE(" + id + ", " + obstacleConstant + ", " + animationSequenceConstant + ", "
			    + width + ", " + height + ", " + x + ", " + y + ", " + (x + dx) + ", " + (y + dy) + ", "
			    + (x + 2 * dx) + ", " + (y + 2 * y) + ");");
	    }
	}
	
	out.println("}");
	out.close();
    }

    /**
     * Updates (if needed) the file include/TilesetList.inc.h.
     * This file contains the declaration of the tileset symbolic constants.
     */
    private void updateEnumFile() throws IOException {
	String fileName = zsdxRootPath + File.separator + "include" +
	    File.separator + "TilesetList.inc";
	
	// we have to put the following line into the file unless it is already present
	String lineWanted = "TILESET_" + upperCaseName + ",";

	FileTools.ensureFileHasLine(fileName, lineWanted);
    }
    
    /** 
     * Updates (if needed) the file src/TilesetsCreation.inc.cpp.
     * This file contains the creation of each tileset object.
     */
    private void updateCreationFile() throws IOException {
	String fileName = zsdxRootPath + File.separator + "src" +
	    File.separator + "TilesetsCreation.inc";
	
	// we have to put the following line into the file unless it is already present
	String lineWanted = "tilesets[TILESET_" + upperCaseName + "] = new Tileset" + name + "();";
	
	FileTools.ensureFileHasLine(fileName, lineWanted);
    }
}
