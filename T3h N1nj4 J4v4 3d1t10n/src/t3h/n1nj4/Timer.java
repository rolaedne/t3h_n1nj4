package t3h.n1nj4;

public class Timer {

    private long lastUpdateTimeMs;

    public Timer() { }

    public Timer init() {
        lastUpdateTimeMs = System.currentTimeMillis();
        return this;
    }

    public long delta() {
        long now = System.currentTimeMillis();
        long delta = now - lastUpdateTimeMs;
        lastUpdateTimeMs = now;
        return delta;
    }
}
