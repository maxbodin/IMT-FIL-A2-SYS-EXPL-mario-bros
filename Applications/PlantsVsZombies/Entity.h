#ifndef ENTITY_H
#define ENTITY_H

enum EntityState { ALIVE, DYING, DEAD, BLOCKED };

class Entity {
public:
    Entity(int x, int y, int hp);
    virtual ~Entity() {}

    virtual void update() = 0;
    virtual void render() = 0;

    void takeDamage(int amount);
    void heal(int amount);
    bool isDead() const;

    int getId() const;
    EntityState getState() const;
    int getX() const;
    int getY() const;

    virtual int getWidth() const;
    virtual int getHeight() const;
    
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
