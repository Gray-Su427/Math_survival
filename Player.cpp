#include "Player.h"
#include <algorithm>
#include <iostream>
#include <cmath>

Player::Player() 
    : position(600, 400), health(100), maxHealth(100), exp(0), level(1), speed(200.0f), requiredExp(100), sprite(texture)
    {
    if (!texture.loadFromFile("assets/player.png")) {
        std::cout << "Failed to load player texture" << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect({0, 0}, {30, 30}));
    texture.setSmooth(true);
    sprite.setScale({1.5f, 1.5f});
    sprite.setOrigin(sf::Vector2f(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f));
    sprite.setPosition(position);

    shape.setRadius(15.0f);
    shape.setFillColor(sf::Color::Transparent);
    shape.setOrigin(sf::Vector2f(15.0f, 15.0f));
    shape.setPosition(position);
    shape.setOutlineColor(sf::Color::Red);
    shape.setOutlineThickness(-1.0f);
}

void Player::reset(){
    position = sf::Vector2f(600, 400);
    maxHealth = 100;
    health = maxHealth;
    exp = 0;
    level = 1;
    requiredExp = 100;
    Wei_Notebook = 0;
    shape.setPosition(position);
    sprite.setPosition(position);
}

void Player::update(float deltaTime) {
    // 更新位置
    shape.setPosition(position);
    sprite.setPosition(position);
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
    // window.draw(shape);
}

void Player::move(sf::Vector2f direction, float deltaTime) {
    // 标准化方向向量
    float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (magnitude > 0) {
        direction.x /= magnitude;
        direction.y /= magnitude;
    }
    
    // 计算新位置
    sf::Vector2f newPosition = position + direction * speed * deltaTime;
    
    // 边界检测 - 保持玩家在窗口内
    const float radius = shape.getRadius();
    newPosition.x = std::max(radius, std::min(1200.0f - radius, newPosition.x));
    newPosition.y = std::max(radius, std::min(800.0f - radius, newPosition.y));
    
    position = newPosition;
    shape.setPosition(position);
    sprite.setPosition(position);
}



sf::Vector2f Player::getCenter() const {
    return position;
}

sf::FloatRect Player::getBounds() const {
    return shape.getGlobalBounds();
}

void Player::takeDamage(int damage) {
    health = std::max(0, health - damage);
    // 移除了伤害提示文字输出
}

void Player::heal(int amount) {
    if (health > 0) {
        health = std::min(maxHealth, health + amount);
        // 移除了治疗提示文字输出
    }
}

void Player::gainExp(int expAmount) {
    exp += expAmount;
    std::cout << "Gained " << expAmount << " EXP. Total: " << exp << std::endl;
    
    // 检查是否升级
    if (exp >= requiredExp) {
        levelUp();
    }
}

void Player::levelUp() {
    level++;
    exp -= requiredExp;
    requiredExp *= 1.5f;
    maxHealth += 20;  // 每级增加最大生命值
    health = maxHealth;  // 升级时满血
    std::cout << "Level Up! Now level " << level << std::endl;
    std::cout << "Max Health increased to " << maxHealth << std::endl;
    
}

void Player::notebooklevelup() {
    Wei_Notebook++;
    std::cout << "Notebook Level Up! Now level " << Wei_Notebook << std::endl;
}