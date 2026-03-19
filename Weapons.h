#ifndef WEAPONS_H
#define WEAPONS_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <limits>
#include <random>
#include "Enemies.h"
#include <unordered_set>

// 子弹类型枚举
enum class BulletType {
    Simple_Bullet, // 简单子弹类型
    Laser,         // 激光子弹
    Bomb,          // 炸弹
    Explosion      // 爆炸
};

// 子弹基类
class Bullet {
protected:
    sf::RectangleShape shape;
    sf::CircleShape hitEffect;
    sf::Vector2f velocity;
    BulletType type;
    int damage;
    float speed;
    float lifeTime;
    bool isActive;
    // 对于可穿透子弹（如激光），记录已被此子弹命中的敌人，避免重复伤害
    std::unordered_set<int> hitEnemies;
    sf::Texture texture;
    sf::Sprite sprite;

public:
    Bullet(sf::Vector2f position, sf::Vector2f direction, BulletType bulletType = BulletType::Simple_Bullet, int weaponLevel = 0);
    virtual ~Bullet() = default;
    
    // 核心功能
    virtual void update(float deltaTime);
    virtual void draw(sf::RenderWindow& window) const;
    
    // 位置和状态管理
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    bool getActive() const { return isActive; }
    void setActive(bool active) { isActive = active; }
    
    // 属性访问器
    BulletType getType() const { return type; }
    int getDamage() const { return damage; }
    void setDamage(int dmg) { damage = dmg; }
    float getSpeed() const { return speed; }
    // 命中记录管理
    bool hasHitEnemy(int enemyId) const;
    void recordHitEnemy(int enemyId);
};

// 子弹管理器类
class BulletManager {
private:
    std::vector<std::unique_ptr<Bullet>> bullets;

public:
    BulletManager();
    ~BulletManager() = default;
    
    // 核心功能
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    
    // 子弹管理
    void addBullet(std::unique_ptr<Bullet> bullet);
    std::vector<std::unique_ptr<Bullet>>& getBullets() { return bullets; }
    const std::vector<std::unique_ptr<Bullet>>& getBullets() const { return bullets; }
    void clearBullets() { bullets.clear(); }
    
    // 状态查询
    size_t getActiveBulletCount() const;
    bool isEmpty() const { return bullets.empty(); }
    
    // 清理无效子弹
    void removeInactiveBullets();
};

// 武器基类
class Weapon {
protected:
    float lockOnRange;        // 锁定范围
    int level;               // 武器等级
    // 升级相关属性
    float baseLockOnRange;

public:
    Weapon(float range = 400.0f);
    virtual ~Weapon() = default;
    
    // 核心功能接口
    virtual void fire(sf::Vector2f playerPos, sf::Vector2f targetPos, BulletManager& bulletManager);
    virtual void update(float deltaTime);
    virtual void upgrade();  // 武器升级
    
    // 索敌功能
    // 查找多个目标，返回最多 targetCount 个在 lockOnRange 内的最近敌人
    virtual std::vector<Enemy*> findTargets(const std::vector<Enemy*>& enemies, sf::Vector2f playerPos, int targetCount);
    
    // 属性访问器
    void setLockOnRange(float range) { lockOnRange = range; }
    
    float getLockOnRange() const { return lockOnRange; }
    int getLevel() const { return level; }
    void setLevel(int lvl);
    
    // 升级相关
    virtual void applyUpgradeEffects();
};

// 微分切割器：特殊武器，发射激光（Laser）子弹
class DifferentialCutter : public Weapon {
public:
    DifferentialCutter(float range = 400.0f);
    virtual ~DifferentialCutter() = default;

    // 重写发射函数，使其发射 Laser 类型的子弹
    void fire(sf::Vector2f playerPos, sf::Vector2f targetPos, BulletManager& bulletManager) override;
};

// 定积分轰炸: 特殊武器，发射炸弹（Bomb）子弹
class IntegralBomber : public Weapon {
public:
    IntegralBomber(float range = 400.0f);
    virtual ~IntegralBomber() = default;

    // 重写发射函数，使其发射 Bomb 类型的子弹
    void fire(sf::Vector2f playerPos, sf::Vector2f targetPos, BulletManager& bulletManager) override;
};

#endif // WEAPONS_H