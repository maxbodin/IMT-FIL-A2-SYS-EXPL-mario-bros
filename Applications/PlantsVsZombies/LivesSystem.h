#ifndef LIVES_SYSTEM_H
#define LIVES_SYSTEM_H

class LivesSystem {
public:
    LivesSystem();

    void loseLife();
    bool isGameOver() const;
    void draw() const;

private:
    static const int MAX_LIVES = 3;
    int lives;
};

#endif
