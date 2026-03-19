#include "items.h"
#include "Player.h"
#include <iostream>
#include <algorithm>

// Item基类实现
Item::Item(sf::Vector2f pos, ItemType itemType) 
    : position(pos), type(itemType), isActive(true), lifetime(0.0f), maxLifetime(15.0f) {
    // 仅设置位置；具体形状由子类设置（使用 ConvexShape）
    shape.setPosition(position);
    
    switch(type) {
        case ItemType::EXP_ORB:
            // 默认的拾取范围和生命周期，子类可覆盖
            pickupRange = 100.0f;
            maxLifetime = 20.0f;  // 经验球存活时间更长
            break;
        case ItemType::HEALTH_PACK:
            pickupRange = 40.0f;
            maxLifetime = 15.0f;
            break;
    }
}

void Item::update(float deltaTime) {
    if (!isActive) return;
    
    lifetime += deltaTime;
    
    // 如果超时则失效
    if (lifetime >= maxLifetime) {
        isActive = false;
    }
}

void Item::draw(sf::RenderWindow& window) const {
    if (!isActive) return;
    window.draw(shape);
}

sf::FloatRect Item::getBounds() const {
    return shape.getGlobalBounds();
}

bool Item::canBePickedUp(sf::Vector2f playerPos) const {
    if (!isActive) return false;
    
    float distance = std::sqrt(
        std::pow(playerPos.x - position.x, 2) + 
        std::pow(playerPos.y - position.y, 2)
    );
    
    return distance <= pickupRange;
}

// ExpOrb实现
ExpOrb::ExpOrb(sf::Vector2f pos, int value) 
    : Item(pos, ItemType::EXP_ORB), expValue(value) {
    // 经验球渲染为较小的绿色菱形（diamond），中心在 position
    float r = 6.0f; // 半径/半宽，用于控制菱形大小
    shape.setPointCount(4);
    shape.setPoint(0, sf::Vector2f(0.0f, -r));
    shape.setPoint(1, sf::Vector2f(r, 0.0f));
    shape.setPoint(2, sf::Vector2f(0.0f, r));
    shape.setPoint(3, sf::Vector2f(-r, 0.0f));
    shape.setFillColor(sf::Color(0, 200, 0)); // 绿色
    shape.setOutlineThickness(1.0f);
    shape.setOutlineColor(sf::Color::White);
    shape.setPosition(position);
}

void ExpOrb::onPickup() {
    // 经验球被拾取时的特效可以在后续添加
}

// HealthPack实现
HealthPack::HealthPack(sf::Vector2f pos, int heal) 
    : Item(pos, ItemType::HEALTH_PACK), healAmount(heal) {
    // 为保持原先圆形外观的感觉，使用多边形近似圆（16边形）
    float r = 8.0f;
    const int sides = 16;
    shape.setPointCount(sides);
    for (int i = 0; i < sides; ++i) {
        float angle = 2.0f * 3.14159265358979323846f * static_cast<float>(i) / static_cast<float>(sides);
        shape.setPoint(i, sf::Vector2f(std::cos(angle) * r, std::sin(angle) * r));
    }
    shape.setFillColor(sf::Color::Red);
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::White);
    shape.setPosition(position);
}

void HealthPack::onPickup() {
    // 血包被拾取时的特效可以在后续添加
}

// ItemManager实现
ItemManager::ItemManager(std::mt19937& gen) 
    : generator(gen), healthDropChance(1, 20) {  // 1/20的概率
}

void ItemManager::update(float deltaTime, sf::Vector2f playerPos) {
    // 更新所有物品
    for (auto& item : items) {
        if (item && item->getActive()) {
            item->update(deltaTime);
        }
    }
    
    // 移除过期或无效的物品
    removeExpiredItems();
}

void ItemManager::draw(sf::RenderWindow& window) const {
    for (const auto& item : items) {
        if (item && item->getActive()) {
            item->draw(window);
        }
    }
}

void ItemManager::addItem(std::unique_ptr<Item> item) {
    if (item) {
        items.push_back(std::move(item));
    }
}

void ItemManager::dropItemsFromEnemy(sf::Vector2f enemyPos, bool killedByPlayer) {
    if (!killedByPlayer) return;  // 只有被玩家击杀才掉落物品
    
    // 必定掉落经验球
    addItem(std::make_unique<ExpOrb>(enemyPos));
    
    // 1/20概率掉落血包
    if (healthDropChance(generator) == 1) {
        addItem(std::make_unique<HealthPack>(enemyPos));
    }
}

void ItemManager::checkPlayerPickup(sf::Vector2f playerPos, int& playerExp, int& playerHealth, int maxHealth) {
    // 保持向后兼容的签名：此函数仍然可被调用，但我们优先直接使用 Player 接口
    for (auto& item : items) {
        if (item && item->getActive() && item->canBePickedUp(playerPos)) {
            switch(item->getType()) {
                case ItemType::EXP_ORB: {
                    ExpOrb* expOrb = dynamic_cast<ExpOrb*>(item.get());
                    if (expOrb) {
                        playerExp += expOrb->getExpValue();
                        expOrb->onPickup();
                        expOrb->setActive(false);
                    }
                    break;
                }
                case ItemType::HEALTH_PACK: {
                    HealthPack* healthPack = dynamic_cast<HealthPack*>(item.get());
                    if (healthPack) {
                        playerHealth = std::min(maxHealth, playerHealth + healthPack->getHealAmount());
                        healthPack->onPickup();
                        healthPack->setActive(false);
                    }
                    break;
                }
            }
        }
    }
}

// 新增：基于 Player 对象的拾取处理，直接调用 Player 的接口以更干净地管理经验和治疗
void ItemManager::checkPlayerPickup(sf::Vector2f playerPos, Player& player) {
    for (auto& item : items) {
        if (item && item->getActive() && item->canBePickedUp(playerPos)) {
            switch(item->getType()) {
                case ItemType::EXP_ORB: {
                    ExpOrb* expOrb = dynamic_cast<ExpOrb*>(item.get());
                    if (expOrb) {
                        player.gainExp(expOrb->getExpValue());
                        expOrb->onPickup();
                        expOrb->setActive(false);
                    }
                    break;
                }
                case ItemType::HEALTH_PACK: {
                    HealthPack* healthPack = dynamic_cast<HealthPack*>(item.get());
                    if (healthPack) {
                        player.heal(healthPack->getHealAmount());
                        healthPack->onPickup();
                        healthPack->setActive(false);
                    }
                    break;
                }
            }
        }
    }
}

void ItemManager::removeExpiredItems() {
    items.erase(
        std::remove_if(items.begin(), items.end(),
            [](const std::unique_ptr<Item>& item) {
                return !item || !item->getActive() || item->isExpired();
            }),
        items.end()
    );
}