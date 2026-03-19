#ifndef ENEMIES_H
#define ENEMIES_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <memory>
// #include "Weapons.h"
// #include "items.h"  // 添加物品系统头文件

class Enemy {
private:
    sf::RectangleShape shape;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    float speed;
    int health;
    int maxHealth;  // 最大血量，用于血条比例计算
    bool isActive;
    int id; // 敌人唯一 id
    bool counted; // 是否已被计数（用于击杀计数）
    
    // 血条显示组件
    sf::RectangleShape healthBarBackground;  // 灰色底色
    sf::RectangleShape healthBarFill;        // 红色填充
    
    // 窗口边界常量
    static const int WINDOW_WIDTH = 1200;
    static const int WINDOW_HEIGHT = 800;

public:
    Enemy(sf::Vector2f position, int enemyId, int level, float enemySpeed = 50.f);
    ~Enemy() = default;
    
    // 核心功能
    void update(sf::Vector2f targetPos, float deltaTime);
    void draw(sf::RenderWindow& window) const;
    
    // 位置和状态管理
    sf::Vector2f getPosition() const;
    sf::Vector2f getCenter() const;
    sf::FloatRect getBounds() const;
    bool getActive() const { return isActive; }
    void setActive(bool active) { isActive = active; }
    
    // 边界检查
    bool isOutOfBounds() const;
    
    // 生命值管理
    void takeDamage(int damage);
    bool isDead() const { return health <= 0; }
    bool isCounted() const { return counted; }
    void setCounted(bool v) { counted = v; }
    
    // 血条管理
    void updateHealthBar();
    void drawHealthBar(sf::RenderWindow& window) const;
    
    // 属性访问器
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    float getHealthRatio() const { return static_cast<float>(health) / maxHealth; }
    float getSpeed() const { return speed; }
    int getId() const { return id; }
};

class EnemySpawner {
private:
    float spawnTimer;
    float spawnInterval;
    std::mt19937& generator;
    std::uniform_real_distribution<float> disX;
    std::uniform_real_distribution<float> disY;
    int baseSpawnCount;
    int nextId; // 用于生成唯一敌人 ID

public:
    EnemySpawner(std::mt19937& gen, float interval = 6.f, int baseCount = 3);
    ~EnemySpawner() = default;
    
    void update(float deltaTime, std::vector<std::unique_ptr<Enemy>>& enemies, int playerLevel);
    void reset();
    
    void setSpawnInterval(float interval) { spawnInterval = interval; }
    float getSpawnInterval() const { return spawnInterval; }
};

#endif // ENEMIES_H