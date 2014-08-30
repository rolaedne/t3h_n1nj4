package t3h.n1nj4;

import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyListener;

public class GameController {

    private Entity entity = new Entity("resources/images/sprite.png");

    // Advance the game world by the given duration
    public void step(final long duration) {

        // Handle input
        // Handle movement/collision/etc.
    }

    // Draw the game world to the given view port
    public void draw(final Rectangle viewPort, final Graphics2D g) {
        // Parallax background
        // Background animated objects
        // Foreground geometry/blocks (collision layer?)
        // Foreground animated objects (decoration layer)
        // Enemies
        // Player
        // Foremost animated objects
        // UI elements
        entity.draw(g, viewPort.x + viewPort.width/2, viewPort.y + viewPort.height/2);
    }

    public KeyListener getKeyListener() {
        return new KeyAdapter() {

        };
    }


}