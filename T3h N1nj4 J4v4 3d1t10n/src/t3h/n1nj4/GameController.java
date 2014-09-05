package t3h.n1nj4;

import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

public class GameController {

    private Entity entity = new Entity("resources/images/sprite.png");
    private int x = 0; // TODO: Don't do stupid shit like this
    private int y = 0; 

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
        entity.draw(g, viewPort.x + viewPort.width/2 + x, viewPort.y + viewPort.height/2 - y);
    }

    public KeyListener getKeyListener() {
        return new KeyAdapter() {

        // TODO FUCK TABS
			@Override
			public void keyPressed(KeyEvent e) {
				if (e.getKeyCode() == KeyEvent.VK_RIGHT) {
					x += 5;
				}
				if (e.getKeyCode() == KeyEvent.VK_LEFT) {
					x += -5;
				}
				if (e.getKeyCode() == KeyEvent.VK_UP) {
					y += 5; 
				}// SUCK ALL THE COCKS
			}
		};
    }

}
