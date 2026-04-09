#pragma once
#include "config.hpp"

class Monster;

class Item {
  string name;
  int type;
  int value;

public:
  Item(string item_name) : name(item_name) {}
};

class Character {
protected:
  int max_health;
  int curr_health;
  int attack;
  int defense;
  string name;

public:
  Character(int hp, int atk, int def, string n)
      : max_health(hp), curr_health(hp), attack(atk), defense(def), name(n) {}
  virtual ~Character() {}
  void receive_attack(int);
  int get_curr_health() { return curr_health; };
  int get_max_health() { return max_health; };
  string get_name() { return name; };
};

class Player : public Character {
  int lv;
  vector<Item> items;

public:
  Player(int max_hp = 20, int love = 1, int atk = 5, int def = 5,
         string human_name = "Chara", vector<Item> held_items = {})
      : Character(max_hp, atk, def, human_name), lv(love), items(held_items) {}
  void receive_attack(
      const Monster
          *monster); // Je sais pas si ca doit etre une methode de Monster ou
                     // une methode de Player, en vrai plutot Monstre comme ca
                     // il peut choisir quelle attque
  void attack_monster(Monster *monster);
};

class Monster : public Character {
  string intro_;
  string *dialogues;

public:
  Monster(int max_hp = 5, string monster_name = "Napstablook", int atk = 5,
          int def = 5, string *dials = {})
      : Character(max_hp, atk, def, monster_name), dialogues(dials) {}
  string get_name() const { return name; }
  int get_attack() const { return attack; }
  int get_hp() const { return curr_health; }
  void receive_attack(int damages);
};

class Encounter {
  Player *player;
  vector<Monster *> monster_list;

public:
  Encounter(Player *human, vector<Monster *> monst_list)
      : player(human), monster_list(monst_list) {}
  void player_turn();
  void monsters_turn();
};
