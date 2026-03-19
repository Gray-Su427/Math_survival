#include "Enemies.h"
#include <algorithm>
#include <iostream>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

// Enemy implementation
Enemy::Enemy(sf::Vector2f position, int enemyId, int level, float enemySpeed) 
    : speed(enemySpeed), health(3 + (level / 5)), maxHealth(3 + (level / 5)), isActive(true), id(enemyId), texture("assets/enemy.png"), sprite(texture) {
    counted = false;

    if (texture.loadFromFile("assets/enemy.png")){
        std::cout << "Enemy texture loaded successfully." << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setScale({0.5f, 0.5f});
    sprite.setOrigin(sf::Vector2f(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f));
    shape.setPosition(position);

    shape.setSize(sf::Vector2f(20, 20));
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineColor(sf::Color::Red);
    shape.setOutlineThickness(-0.5f);
    shape.setPosition(position);
    
    // 初始化血条组件
    // 血条背景（灰色）
    healthBarBackground.setSize(sf::Vector2f(24, 4));  // 稍宽于敌人
    healthBarBackground.setFillColor(sf::Color(128, 128, 128, 200));  // 灰色半透明
    healthBarBackground.setOrigin(sf::Vector2f(12, 2));  // 居中
    
    // 血条填充（红色）
    healthBarFill.setSize(sf::Vector2f(24, 4));  // 与背景相同大小
    healthBarFill.setFillColor(sf::Color::Red);
    healthBarFill.setOrigin(sf::Vector2f(12, 2));  // 居中
    
    // 初始位置设置
    updateHealthBar();
}

void Enemy::update(sf::Vector2f targetPos, float deltaTime) {
    if (!isActive) return;
    
    sf::Vector2f enemyCenter = getCenter();
    sf::Vector2f direction = targetPos - enemyCenter;
    
    // Normalize direction vector
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction /= length;
    }
    
    // 更新位置
    shape.move(direction * speed * deltaTime);
    sprite.setPosition(shape.getPosition());
    
    // 更新血条位置
    updateHealthBar();
    
    // 边界检查
    if (isOutOfBounds()) {
        isActive = false;
    }
}

void Enemy::draw(sf::RenderWindow& window) const {
    if (isActive) {
        // window.draw(shape);
        window.draw(sprite);
        drawHealthBar(window);  // 绘制血条
    }
}

void Enemy::updateHealthBar() {
    if (!isActive) return;
    
    // 血条位置：敌人上方约15像素
    sf::Vector2f enemyPos = shape.getPosition();
    sf::Vector2f barPosition(enemyPos.x + 10, enemyPos.y - 15);
    
    healthBarBackground.setPosition(barPosition);
    healthBarFill.setPosition(barPosition);
    
    // 根据当前血量更新填充宽度
    float healthRatio = getHealthRatio();
    healthBarFill.setSize(sf::Vector2f(24 * healthRatio, 4));
}

void Enemy::drawHealthBar(sf::RenderWindow& window) const {
    if (isActive && health < maxHealth) {  // 只有受伤时才显示血条
        window.draw(healthBarBackground);
        window.draw(healthBarFill);
    }
}

sf::Vector2f Enemy::getPosition() const {
    return shape.getPosition();
}

sf::Vector2f Enemy::getCenter() const {
    sf::Vector2f pos = shape.getPosition();
    return sf::Vector2f(pos.x + 10, pos.y + 10);
}

sf::FloatRect Enemy::getBounds() const {
    return shape.getGlobalBounds();
}

bool Enemy::isOutOfBounds() const {
    sf::Vector2f pos = shape.getPosition();
    return (pos.x < -150 || pos.x > WINDOW_WIDTH + 150 || 
            pos.y < -150 || pos.y > WINDOW_HEIGHT + 150);
}

void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health <= 0) {
        health = 0;
        isActive = false;
    }
    // 血条会自动更新，因为updateHealthBar在update中被调用
}

// EnemySpawner implementation
EnemySpawner::EnemySpawner(std::mt19937& gen, float interval, int baseCount)
        : spawnTimer(0.f), spawnInterval(interval), generator(gen), 
            disX(0, 1200), disY(0, 800), baseSpawnCount(baseCount), nextId(1) {}

void EnemySpawner::update(float deltaTime, std::vector<std::unique_ptr<Enemy>>& enemies, int playerLevel) {
    spawnTimer += deltaTime;
    
    if (spawnTimer >= spawnInterval) {
        int enemiesToSpawn = baseSpawnCount + (playerLevel - 1) * 2;
        
        for (int i = 0; i < enemiesToSpawn; i++) {
            float spawnX, spawnY;
            if (disX(generator) > 600) {
                spawnX = (disX(generator) > 600) ? 1250 : -50;
                spawnY = disY(generator);
            } else {
                spawnX = disX(generator);
                spawnY = (disY(generator) > 400) ? 850 : -50;
            }
            
            // 分配 id，达到 INT_MAX 后回绕到1
            int assignedId = nextId;
            if (nextId == std::numeric_limits<int>::max()) nextId = 1;
            else ++nextId;
            enemies.push_back(std::make_unique<Enemy>(sf::Vector2f(spawnX, spawnY), assignedId, playerLevel));
        }
        
        spawnTimer = 0.f;
    }
}

void EnemySpawner::reset() {
    spawnTimer = 0.f;
}

// // EnemyManager implementation
// EnemyManager::EnemyManager(std::mt19937& gen) 
//     : spawner(gen, 6.f, 3), generator(gen), itemManager(nullptr) {}

// void EnemyManager::update(float deltaTime, sf::Vector2f playerPos, int playerLevel) {
//     // 更新生成器
//     spawner.update(deltaTime, enemies, playerLevel);
    
//     // 更新现有敌人
//     for (auto& enemy : enemies) {
//         if (enemy->getActive()) {
//             enemy->update(playerPos, deltaTime);
//         }
//     }
    
//     // 移除死亡或越界的敌人
//     removeDeadEnemies();
// }

// void EnemyManager::draw(sf::RenderWindow& window) const {
//     for (const auto& enemy : enemies) {
//         if (enemy->getActive()) {
//             enemy->draw(window);
//         }
//     }
// }

// bool EnemyManager::checkPlayerCollision(const sf::FloatRect& playerBounds) const {
//     for (const auto& enemy : enemies) {
//         if (enemy->getActive() && enemy->checkCollision(playerBounds)) {
//             // 使用手动的距离计算进行精确碰撞检测
//             sf::FloatRect enemyBounds = enemy->getBounds();
//             sf::Vector2f enemyCenter(enemyBounds.position.x + enemyBounds.size.x/2, 
//                                    enemyBounds.position.y + enemyBounds.size.y/2);
//             sf::Vector2f playerCenter(playerBounds.position.x + playerBounds.size.x/2,
//                                     playerBounds.position.y + playerBounds.size.y/2);
            
//             float distance = std::sqrt(
//                 std::pow(enemyCenter.x - playerCenter.x, 2) +
//                 std::pow(enemyCenter.y - playerCenter.y, 2)
//             );
//             if (distance < 25.f) {  // 碰撞距离阈值
//                 return true;
//             }
//         }
//     }
//     return false;
// }

// int EnemyManager::checkBulletCollisions(const std::vector<std::unique_ptr<Bullet>>& bullets) {
//     int killedEnemies = 0;
    
//     for (auto& enemy : enemies) {
//         if (!enemy->getActive()) continue;
        
//         for (const auto& bullet : bullets) {
//             if (bullet->getActive() && enemy->checkBulletCollision(bullet.get())) {
//                 enemy->takeDamage(bullet->getDamage(), true);  // 标记为玩家造成的伤害
//                 bullet->setActive(false);  // 子弹失效
                
//                 if (enemy->isDead()) {
//                     killedEnemies++;
//                     handleEnemyDeath(enemy.get());  // 处理敌人死亡掉落
//                 }
//                 break;  // 每个敌人每次只被一颗子弹击中
//             }
//         }
//     }
    
//     return killedEnemies;
// }

// void EnemyManager::removeDeadEnemies() {
//     enemies.erase(
//         std::remove_if(enemies.begin(), enemies.end(),
//             [](const std::unique_ptr<Enemy>& enemy) {
//                 return !enemy->getActive();
//             }),
//         enemies.end()
//     );
// }

// void EnemyManager::handleEnemyDeath(Enemy* enemy) {
//     if (itemManager && enemy->wasKilledByPlayer()) {
//         // 只有被玩家击杀的敌人才掉落物品
//         itemManager->dropItemsFromEnemy(enemy->getCenter(), true);
//     }
// }