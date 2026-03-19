#include "Weapons.h"
#include <cmath>
#include <limits>
#include <iostream>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

// Bullet实现
Bullet::Bullet(sf::Vector2f position, sf::Vector2f direction, BulletType bulletType, int weaponLevel) 
    : type(bulletType), isActive(true), texture("assets/bullet.png"), sprite(texture){

    // 根据子弹类型设置不同的属性
    switch (type) {
        case BulletType::Simple_Bullet:
            // 简单子弹：6x6 黄色方形（等同于直径6的圆）
            if (texture.loadFromFile("assets/bullet.png")){
                std::cout << "Loaded bullet texture\n";
            }
            sprite.setTexture(texture);
            sprite.setTextureRect(sf::IntRect({0, 0}, {50, 50}));
            sprite.setScale({0.5f, 0.5f});
            shape.setSize({6.f + 2.f * weaponLevel, 6.f  + 2.f * weaponLevel});
            shape.setFillColor(sf::Color::Transparent);
            shape.setOutlineColor(sf::Color::Red);
            shape.setOutlineThickness(-0.5f);
            shape.setOrigin({3.f + 1.f * weaponLevel, 3.f + 1.f * weaponLevel});
            speed = 200.f;
            damage = 3 * weaponLevel; // 根据武器等级增加伤害
            lifeTime = std::numeric_limits<float>::max();
            break;
        case BulletType::Laser:
            // 激光：长条状，3像素宽，50像素长，伤害2，不在命中后消失
            sprite.setScale({0.f, 0.f});
            shape.setSize({50.f, 3.f + 1.f * weaponLevel});
            shape.setFillColor(sf::Color::Cyan);
            shape.setOutlineColor(sf::Color(0, 255, 255, 128));
            shape.setOutlineThickness(0.5f);
            shape.setOrigin({0.f, 1.5f}); // 以左中为原点
            speed = 300.f; // 激光移动更快
            damage = 2 * weaponLevel;
            lifeTime = std::numeric_limits<float>::max();
            break;
        case BulletType::Bomb:
            // 炸弹：圆形，30像素宽，30像素高，在命中后消失
            if (texture.loadFromFile("assets/bomb.png")){
                std::cout << "Loaded bomb texture\n";
            }
            sprite.setTexture(texture);
            sprite.setTextureRect(sf::IntRect({0, 0}, {50, 50}));
            shape.setSize({30.f, 30.f});
            shape.setFillColor(sf::Color::Transparent);
            shape.setOutlineColor(sf::Color::Red);
            shape.setOutlineThickness(-0.5f);
            shape.setOrigin({5.f, 5.f});
            speed = 0.f;
            damage = 5 * weaponLevel;
            lifeTime = 5.f;
            break;
        case BulletType::Explosion:
            // 爆炸：方形，200像素宽，200像素高，伤害5，在命中后消失
            if (texture.loadFromFile("assets/explosion.png")){
                std::cout << "Loaded explosion texture\n";
            }
            sprite.setTexture(texture);
            sprite.setTextureRect(sf::IntRect({0, 0}, {200, 200}));
            shape.setSize({200.f, 200.f});
            shape.setFillColor(sf::Color::Transparent);
            shape.setOrigin({100.f, 100.f});
            shape.setOutlineColor(sf::Color::Red);
            shape.setOutlineThickness(-0.5f);
            speed = 0.f;
            damage = 5;
            lifeTime = 0.5f;
    }
    
    shape.setPosition(position);
    sprite.setOrigin(sf::Vector2f(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f));
    sprite.setPosition(position);

    texture.setSmooth(true);
    
    // 根据方向设置速度
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        velocity = (direction / length) * speed;
    } else {
        velocity = sf::Vector2f(0, 0);
    }
    // 根据方向旋转形状，使长边平行于发射方向
    float angleRad = std::atan2(direction.y, direction.x);
    float angleDeg = angleRad * 180.0f / 3.14159265358979323846f;
    shape.setRotation(sf::degrees(angleDeg));
    sprite.setRotation(shape.getRotation());
}

void Bullet::update(float deltaTime) {
    if (!isActive) return;
    
    // 更新位置
    // 对于激光，speed为0且不移动；对于普通子弹则按速度移动
    if (speed != 0.f) {
        shape.move(velocity * deltaTime);
    }
    
    //  lifetime
    if (lifeTime > 0.f) {
        lifeTime -= deltaTime;
        if (lifeTime <= 0.f) {
            isActive = false;
        }
    }
    // 边界检查
    sf::Vector2f pos = shape.getPosition();
    if (pos.x < -50 || pos.x > WINDOW_WIDTH + 50 || 
        pos.y < -50 || pos.y > WINDOW_HEIGHT + 50) {
        isActive = false;
    }
    sprite.setPosition(pos);
    shape.setPosition(pos);
}

void Bullet::draw(sf::RenderWindow& window) const {
    if (isActive) {
    window.draw(sprite);
    // window.draw(shape);
    if (type == BulletType::Laser) {
        window.draw(shape);
    }
    }
}

sf::Vector2f Bullet::getPosition() const {
    return shape.getPosition();
}

sf::FloatRect Bullet::getBounds() const {
    return shape.getGlobalBounds();
}

bool Bullet::hasHitEnemy(int enemyId) const {
    return hitEnemies.find(enemyId) != hitEnemies.end();
}

void Bullet::recordHitEnemy(int enemyId) {
    hitEnemies.insert(enemyId);
}

// BulletManager实现
BulletManager::BulletManager() {
    // 构造函数可以初始化一些管理器特定的属性
}

void BulletManager::update(float deltaTime) {
    for (auto& bullet : bullets) {
        if (bullet && bullet->getActive()) {
            bullet->update(deltaTime);
        }
    }
    
    std::vector<std::unique_ptr<Bullet>> newBullets;
    for (auto& bullet : bullets) {
        if (bullet && !bullet->getActive() && bullet->getType() == BulletType::Bomb) {
            sf::Vector2f bompos = bullet->getPosition();
            auto explosion = std::make_unique<Bullet>(bompos, sf::Vector2f(0, 0), BulletType::Explosion, 1);
            newBullets.push_back(std::move(explosion));
        }
    }
    
    for (auto& newBullet : newBullets) {
        bullets.push_back(std::move(newBullet));
    }

    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const std::unique_ptr<Bullet>& bullet) {
                return bullet && !bullet->getActive();
            }),
        bullets.end()
    );
}

void BulletManager::draw(sf::RenderWindow& window) const {
    // 绘制所有活跃子弹
    for (const auto& bullet : bullets) {
        if (bullet->getActive()) {
            bullet->draw(window);
        }
    }
}

void BulletManager::addBullet(std::unique_ptr<Bullet> bullet) {
    bullets.push_back(std::move(bullet));
}

size_t BulletManager::getActiveBulletCount() const {
    return std::count_if(bullets.begin(), bullets.end(),
        [](const std::unique_ptr<Bullet>& bullet) {
            return bullet->getActive();
        });
}

// Weapon基类实现
Weapon::Weapon(float range) 
        : lockOnRange(range), level(1),
            baseLockOnRange(range) {
    // debug: report initial weapon level from constructor
    std::cout << "Weapon ctor: initial level=" << level << "\n";
}

void Weapon::fire(sf::Vector2f playerPos, sf::Vector2f targetPos, BulletManager& bulletManager) {
    // 计算射击方向
    sf::Vector2f direction = targetPos - playerPos;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction /= length;
    }
    
    // 创建子弹并添加到管理器
    auto bullet = std::make_unique<Bullet>(playerPos, direction, BulletType::Simple_Bullet, getLevel());
    // 调试：打印子弹的伤害值以确认来源
    std::cout << "Created bullet with damage=" << bullet->getDamage() << "\n";
    bulletManager.addBullet(std::move(bullet));
}

// DifferentialCutter 实现
DifferentialCutter::DifferentialCutter(float range)
    : Weapon(range) {
    // debug: report level before and after forcing to 0
    std::cout << "DifferentialCutter ctor: before setLevel, level=" << getLevel() << "\n";
    // 初始等级设为 0
    setLevel(0);
    std::cout << "DifferentialCutter ctor: after setLevel, level=" << getLevel() << "\n";
}

void DifferentialCutter::fire(sf::Vector2f playerPos, sf::Vector2f targetPos, BulletManager& bulletManager) {
    // 计算射击方向
    sf::Vector2f direction = targetPos - playerPos;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction /= length;
    }

    // 激光子弹的起点稍微向外偏移一点，避免与玩家重叠
    sf::Vector2f spawnPos = playerPos + direction * 20.f;

    auto laser = std::make_unique<Bullet>(spawnPos, direction, BulletType::Laser, getLevel());
    std::cout << "DifferentialCutter fired a laser with damage=" << laser->getDamage() << "\n";
    bulletManager.addBullet(std::move(laser));
}

IntegralBomber::IntegralBomber(float range)
    : Weapon(range) {
    // 默认等级为 0
    setLevel(0);
}

void IntegralBomber::fire(sf::Vector2f playerPos, sf::Vector2f targetPos, BulletManager& bulletManager) {
    if (getLevel() == 0) return;
    // 创建炸弹并添加到管理器
    auto bomb = std::make_unique<Bullet>(playerPos, sf::Vector2f(0, 0), BulletType::Bomb, getLevel());
    std::cout << "IntegralBomber fired a bomb with damage=" << bomb->getDamage() << "\n";
    bulletManager.addBullet(std::move(bomb));
}

void Weapon::update(float deltaTime) {
    // 武器更新逻辑
}

void Weapon::upgrade() {
    level++;
    std::cout << "Weapon::upgrade -> new level=" << level << "\n";
    applyUpgradeEffects();
}

void Weapon::applyUpgradeEffects() {
    // 基础升级效果：提升伤害，锁定范围保持固定
    lockOnRange = baseLockOnRange;
}

void Weapon::setLevel(int lvl) {
    // 允许等级为0（表示未解锁/不可用）
    int old = level;
    level = std::max(0, lvl);
    std::cout << "Weapon::setLevel from " << old << " to " << level << "\n";
    applyUpgradeEffects();
}

std::vector<Enemy*> Weapon::findTargets(const std::vector<Enemy*>& enemies, sf::Vector2f playerPos, int targetCount) {
    std::vector<std::pair<float, Enemy*>> inRange;
    for (auto enemy : enemies) {
        if (!enemy || !enemy->getActive()) continue;
        sf::Vector2f ec = enemy->getCenter();
        float dist = std::sqrt(std::pow(playerPos.x - ec.x, 2) + std::pow(playerPos.y - ec.y, 2));
        if (dist <= lockOnRange) {
            inRange.emplace_back(dist, enemy);
        }
    }

    // 按距离排序
    std::sort(inRange.begin(), inRange.end(), [](const auto& a, const auto& b){ return a.first < b.first; });

    std::vector<Enemy*> result;
    int maxTargets = targetCount;
    if (level <= 0) return result;
    for (size_t i = 0; i < inRange.size() && static_cast<int>(result.size()) < maxTargets; ++i) {
        result.push_back(inRange[i].second);
    }

    return result;
}