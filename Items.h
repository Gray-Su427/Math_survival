#ifndef ITEMS_H
#define ITEMS_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <random>

class Player; // 前向声明，避免循环包含

// 物品类型枚举
enum class ItemType {
    EXP_ORB,     // 经验球
    HEALTH_PACK  // 血包
};

// 物品基类
class Item {
protected:
    sf::ConvexShape shape;
    sf::Vector2f position;
    ItemType type;
    bool isActive;
    float pickupRange;  // 拾取范围
    float lifetime;     // 存活时间
    float maxLifetime;  // 最大存活时间

public:
    Item(sf::Vector2f pos, ItemType itemType);
    virtual ~Item() = default;
    
    // 核心功能
    virtual void update(float deltaTime);
    virtual void draw(sf::RenderWindow& window) const;
    
    // 位置和状态管理
    sf::Vector2f getPosition() const { return position; }
    sf::FloatRect getBounds() const;
    bool getActive() const { return isActive; }
    void setActive(bool active) { isActive = active; }
    
    // 拾取检测
    virtual bool canBePickedUp(sf::Vector2f playerPos) const;
    virtual void onPickup() = 0;  // 纯虚函数，子类必须实现
    
    // 生命周期管理
    bool isExpired() const { return lifetime >= maxLifetime; }
    
    // 属性访问器
    ItemType getType() const { return type; }
    float getPickupRange() const { return pickupRange; }
};

// 经验球类
class ExpOrb : public Item {
private:
    int expValue;  // 经验值

public:
    ExpOrb(sf::Vector2f pos, int value = 10);
    ~ExpOrb() = default;
    
    // 实现纯虚函数
    void onPickup() override;
    
    // 经验值获取
    int getExpValue() const { return expValue; }
};

// 血包类
class HealthPack : public Item {
private:
    int healAmount;  // 恢复血量

public:
    HealthPack(sf::Vector2f pos, int heal = 20);
    ~HealthPack() = default;
    
    // 实现纯虚函数
    void onPickup() override;
    
    // 恢复量获取
    int getHealAmount() const { return healAmount; }
};

// 物品管理器类
class ItemManager {
private:
    std::vector<std::unique_ptr<Item>> items;
    std::mt19937& generator;
    std::uniform_int_distribution<int> healthDropChance;  // 1/20的概率掉血包

public:
    ItemManager(std::mt19937& gen);
    ~ItemManager() = default;
    
    // 核心功能
    void update(float deltaTime, sf::Vector2f playerPos);
    void draw(sf::RenderWindow& window) const;
    
    // 物品管理
    void addItem(std::unique_ptr<Item> item);
    std::vector<std::unique_ptr<Item>>& getItems() { return items; }
    const std::vector<std::unique_ptr<Item>>& getItems() const { return items; }
    void clearItems() { items.clear(); }
    
    // 敌人死亡掉落处理
    void dropItemsFromEnemy(sf::Vector2f enemyPos, bool killedByPlayer);
    
    // 玩家拾取检测
    void checkPlayerPickup(sf::Vector2f playerPos, int& playerExp, int& playerHealth, int maxHealth);
    // 基于 Player 对象的拾取检测（更直接）
    void checkPlayerPickup(sf::Vector2f playerPos, Player& player);
    
    // 清理过期物品
    void removeExpiredItems();
    
    // 状态查询
    size_t getItemCount() const { return items.size(); }
    bool isEmpty() const { return items.empty(); }
};

#endif // ITEMS_H