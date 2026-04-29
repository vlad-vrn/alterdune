#pragma once
#include "config.hpp"

class Monster; // forward declaration for GameStats

class Act {
    string id;
    string name;
    string display_text;
    int mercy_impact;
public:
    Act() : id(""), name(""), display_text(""), mercy_impact(0) {}
    Act(string id, string name, string display_text, int mercy_impact);
    string get_id()           const { return id; }
    string get_name()         const { return name; }
    string get_display_text() const { return display_text; }
    int    get_mercy_impact() const { return mercy_impact; }
};

class Item {
    string name;
    string type;
    int value;
    int quantity;
public:
    Item(string name, string type, int value, int quantity);
    string get_name()     const { return name; }
    string get_type()     const { return type; }
    int    get_value()    const { return value; }
    int    get_quantity() const { return quantity; }
    void use();
};

enum class Ending { GENOCIDE, NEUTRAL, PACIFIST };

class GameStats {
    vector<Monster*> spared;
    vector<Monster*> killed;
public:
    void add_spared(Monster* m);
    void add_killed(Monster* m);
    int    victory_count() const;
    int    spared_count()  const { return (int)spared.size(); }
    int    killed_count()  const { return (int)killed.size(); }
    Ending get_ending()    const;
    const vector<Monster*>& get_spared() const { return spared; }
    const vector<Monster*>& get_killed() const { return killed; }
};

// Abstract base class
class Character {
protected:
    string name;
    int max_health;
    int curr_health;
    int attack;
    int defense;
public:
    Character(string name, int hp, int atk, int def);
    virtual ~Character() {}
    virtual void receive_attack(int damages) = 0;
    string get_name()        const { return name; }
    int    get_curr_health() const { return curr_health; }
    int    get_max_health()  const { return max_health; }
    int    get_attack()      const { return attack; }
    int    get_defense()     const { return defense; }
    bool   is_alive()        const { return curr_health > 0; }
};

class Player : public Character {
    int lv;
    vector<Item> inventory;
    GameStats stats;
public:
    Player(string name = "Chara", int hp = 20, int lv = 1, int atk = 5, int def = 5);
    void receive_attack(int damages) override;
    void use_item(int index);
    void add_item(const Item& item);
    GameStats&    get_stats()     { return stats; }
    vector<Item>& get_inventory() { return inventory; }
    int get_lv() const { return lv; }
};

// Abstract monster base
class Monster : public Character {
protected:
    int mercy;
    int mercy_goal;
    vector<string> act_ids;
public:
    Monster(string name, int hp, int atk, int def, int mercy_goal, vector<string> act_ids);
    virtual ~Monster() {}
    void receive_attack(int damages) override;
    void modify_mercy(int delta);
    bool is_sparable()              const { return mercy >= mercy_goal; }
    virtual int    get_act_count()  const = 0;
    virtual string get_category()   const = 0;
    int                   get_mercy()      const { return mercy; }
    int                   get_mercy_goal() const { return mercy_goal; }
    const vector<string>& get_act_ids()    const { return act_ids; }
};

class NormalMonster : public Monster {
public:
    NormalMonster(string name, int hp, int atk, int def, int mercy_goal, vector<string> act_ids)
        : Monster(name, hp, atk, def, mercy_goal, act_ids) {}
    int    get_act_count() const override { return 2; }
    string get_category()  const override { return "NORMAL"; }
};

class MinibossMonster : public Monster {
public:
    MinibossMonster(string name, int hp, int atk, int def, int mercy_goal, vector<string> act_ids)
        : Monster(name, hp, atk, def, mercy_goal, act_ids) {}
    int    get_act_count() const override { return 3; }
    string get_category()  const override { return "MINIBOSS"; }
};

class BossMonster : public Monster {
public:
    BossMonster(string name, int hp, int atk, int def, int mercy_goal, vector<string> act_ids)
        : Monster(name, hp, atk, def, mercy_goal, act_ids) {}
    int    get_act_count() const override { return 4; }
    string get_category()  const override { return "BOSS"; }
};
