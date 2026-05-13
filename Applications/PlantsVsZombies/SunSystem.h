#ifndef SUN_SYSTEM_H
#define SUN_SYSTEM_H

class SunSystem {
public:
    SunSystem();

    void init(int compt);
    void update(int compt);
    void draw(int compt) const;

    bool canAfford(int cost) const;
    void spend(int cost);

private:
    static const int INITIAL          = 150;
    static const int TICK_INTERVAL    = 10000;
    static const int TICK_AMOUNT      = 25;
    static const int DISPLAY_DURATION = 2000;

    int suns;
    int lastTick;
    int displayEnd;
};

#endif
