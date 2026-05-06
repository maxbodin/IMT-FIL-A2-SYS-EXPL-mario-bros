#ifndef ENTITY_H
#define ENTITY_H

enum EntityState { ALIVE, DYING, DEAD };

class Entity {
public:
    Entity(int x, int y, int hp);

    virtual void update() = 0;
    virtual void render() = 0;

    void takeDamage(int amount);
    bool isDead() const;

    int getId() const;
    EntityState getState() const;
    int getX() const;
    int getY() const;
    int getHp() const;
    int getMaxHp() const;

    void renderHpBar(int barW, int spriteH) const;

protected:
    int _id;
    int x;
    int y;
    int hp;
    int maxHp;
    EntityState state;

private:
    static int nextId;
};

#endif
