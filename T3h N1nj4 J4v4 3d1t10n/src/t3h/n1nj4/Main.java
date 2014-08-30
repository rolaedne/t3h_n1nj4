package t3h.n1nj4;

import java.awt.Canvas;
import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.Rectangle;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferStrategy;

import javax.swing.JFrame;

public class Main {

    public static void main(String[] args) {
        Main m = new Main();
        m.start();
    }

    private Rectangle viewPort = new Rectangle(0, 0, 800, 600);
    private Canvas canvas;

    private Main() {
        JFrame frame = new JFrame("t3h n1nj4: j4v4 3d1t10n");

        Container contentPane = frame.getContentPane();
        contentPane.setPreferredSize(new Dimension(viewPort.width, viewPort.height));
        contentPane.setLayout(null);

        canvas = new Canvas();
        canvas.setBounds(viewPort.x, viewPort.y, viewPort.width, viewPort.height);
        canvas.setIgnoreRepaint(true);
        contentPane.add(canvas);

        frame.pack();
        frame.setResizable(false);

        canvas.createBufferStrategy(2);

        frame.setVisible(true);
        frame.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                System.exit(0);
            }
        });

        canvas.requestFocus();
    }


    private void start() {
        GameController controller = new GameController();
        canvas.addKeyListener(controller.getKeyListener());

        Timer timer = new Timer().init();

        while (true) {
            // Get the buffer to draw on
            BufferStrategy buffer = canvas.getBufferStrategy();
            Graphics2D g = (Graphics2D) buffer.getDrawGraphics();

            // Clear buffer
            g.setColor(Color.WHITE);
            g.fillRect(viewPort.x, viewPort.y, viewPort.width, viewPort.height);

            controller.step(timer.delta());
            controller.draw(viewPort, g);

            // Release graphics resources
            g.dispose();

            // Swap display buffers
            buffer.show();

            Utils.sleep(1000/30);
        }
    }

}
