#pragma once
#include "config.hpp"

class Monster;

class Item {
  string name;

public:
  Item(string item_name) : name(item_name) {}
};

class Player {
  int max_health;
  int curr_health;
  int lv;
  int attack;
  string name;
  vector<Item> items;

public:
  Player(int max_hp = 20, int love = 1, int atk = 5,
         string human_name = "Chara", vector<Item> held_items = {})
      : max_health(max_hp), curr_health(max_hp), lv(love), attack(atk),
        name(human_name), items(held_items) {}
  int get_curr_health() const { return curr_health; }
  void receive_attack(
      const Monster
          *monster); // Je sais pas si ca doit etre une methode de Monster ou
                     // une methode de Player, en vrai plutot Monstre comme ca
                     // il peut choisir quelle attque
  void attack_monster(Monster *monster);
};

class Monster {
  int curr_health;
  string name;
  int attack;
  string intro_;
  string *dialogues;

public:
  Monster(int max_hp = 5, string monster_name = "Napstablook", int atk = 5,
          string *dials = {})
      : curr_health(max_hp), name(monster_name), attack(atk), dialogues(dials) {
  }
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
