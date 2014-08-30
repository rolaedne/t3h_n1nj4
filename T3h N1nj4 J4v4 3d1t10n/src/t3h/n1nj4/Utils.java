package t3h.n1nj4;

import java.awt.Image;
import java.io.IOException;

import javax.imageio.ImageIO;

public class Utils {

    public static void sleep(long duration) {
        try {
            Thread.sleep(duration);
        } catch (Exception ignored) {}
    }

    public static Image loadImage(String resource) {
        try {
            return ImageIO.read(ClassLoader.getSystemClassLoader().getResource(resource));
        } catch (IOException e) {
            System.err.println("Exception: " + e.getMessage());
            System.exit(-1);
        };
        return null;
    }
}
