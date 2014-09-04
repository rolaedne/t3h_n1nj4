package t3h.n1nj4;

import java.awt.Graphics2D;
import java.awt.Image;

public class Entity {

    private Image image;


    public Entity(String resource) {
        image = Utils.loadImage(resource);

        /*
        BufferedImage bi = new BufferedImage(image.getWidth(null), image.getHeight(null), BufferedImage.TYPE_INT_ARGB);

        Graphics2D big = bi.createGraphics();

        AffineTransform mirrorTransform = new AffineTransform();
        mirrorTransform.scale(-1.0, 1.0);
        mirrorTransform.translate(-image.getWidth(null), 0);

        big.drawImage(image, mirrorTransform, null);

        System.out.println("MirrorTransform: " + mirrorTransform.toString());
        image = bi;
		*/

    }

    public void draw(Graphics2D g, int x, int y) {
        g.drawImage(image, x, y, null);
    }
}

