#ifndef DMG_INDICATOR_H
#define DMG_INDICATOR_H

#define MAX_DMG_INDICATORS 16
#define DMG_INDICATOR_DURATION 400  // ticks (0.4 s)

class DmgIndicator {
public:
    DmgIndicator();

    void init(int x, int y, int value, int duration);
    void update();
    void render();

    bool isActive() const;
    void deactivate();

private:
    int x, y;
    int value;
    int startTick;
    int duration;
    bool active;
};

#endif
