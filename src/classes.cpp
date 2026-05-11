#include "classes.hpp"

// Act

Act::Act(string id, string name, string display_text, int mercy_impact)
    : id(id), name(name), display_text(display_text), mercy_impact(mercy_impact) {}

// Item

Item::Item(string name, string type, int value, int quantity)
    : name(name), type(type), value(value), quantity(quantity) {}

void Item::use() {
    if (quantity > 0) quantity--;
}

// GameStats

void GameStats::add_spared(Monster* m) { spared.push_back(m); }
void GameStats::add_killed(Monster* m) { killed.push_back(m); }

int GameStats::victory_count() const {
    return (int)(spared.size() + killed.size());
}

Ending GameStats::get_ending() const {
    if (killed.empty()) return Ending::PACIFIST;
    if (spared.empty()) return Ending::GENOCIDE;
    return Ending::NEUTRAL;
}

// Character

Character::Character(string name, int hp, int atk, int def)
    : name(name), max_health(hp), curr_health(hp), attack(atk), defense(def) {}

// Player

Player::Player(string name, int hp, int lv, int atk, int def)
    : Character(name, hp, atk, def), lv(lv) {}

void Player::receive_attack(int damages) {
    curr_health = max(0, curr_health - damages);
}

void Player::use_item(int index) {
    if (index < 0 || index >= (int)inventory.size()) return;
    Item& item = inventory[index];
    if (item.get_quantity() <= 0) return;
    curr_health = min(max_health, curr_health + item.get_value());
    item.use();
}

void Player::add_item(const Item& item) {
    inventory.push_back(item);
}

// Monster

Monster::Monster(string name, int hp, int atk, int def, int mercy_goal, vector<string> act_ids)
    : Character(name, hp, atk, def), mercy(0), mercy_goal(mercy_goal), act_ids(act_ids) {}

void Monster::receive_attack(int damages) {
    curr_health = max(0, curr_health - damages);
}

void Monster::modify_mercy(int delta) {
    mercy = max(0, min(mercy_goal, mercy + delta));
}

void Monster::reset() {
    curr_health = max_health;
    mercy = 0;
}

void Player::augmenter_attaque(int montant) {
    attack += montant;
}
