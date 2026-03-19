#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <memory>

class Player {
private:
    sf::CircleShape shape;
    sf::Vector2f position;
    int health;
    int maxHealth;
    int exp;
    int requiredExp;
    int level;
    float speed;
    int Wei_Notebook;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Player();
    ~Player() = default;
    
    // 重置状态
    void reset();
    // 更新状态
    void update(float deltaTime);
    // 渲染主角
    void draw(sf::RenderWindow& window) const;
    
    // 移动控制
    void move(sf::Vector2f direction, float deltaTime);
    
    // 位置和状态管理
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getCenter() const;
    sf::FloatRect getBounds() const;
    
    // 生命值管理
    void takeDamage(int damage);
    void heal(int amount);
    bool isAlive() const { return health > 0; }
    
    // 经验和等级管理
    void gainExp(int expAmount);
    void levelUp();
    void notebooklevelup();

    // 属性访问器
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getExp() const { return exp; }
    int getLevel() const { return level; }
    float getHealthRatio() const { return static_cast<float>(health) / maxHealth; }
    int getRequiredExp() const { return requiredExp; }
    int getNotebookLevel() const { return Wei_Notebook; }
};

#endif // PLAYER_H