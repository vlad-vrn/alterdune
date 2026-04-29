#pragma once
#include "classes.hpp"

enum class BattleOutcome { WIN_KILLED, WIN_SPARED, DEFEAT };

class Menu {
public:
    virtual ~Menu() {}
    void show_inventory(Player& player);
    void show_stats(Player& player);
    void show_bestiary(GameStats& stats);
};

class BattleMenu : public Menu {
    Monster* enemy;
public:
    BattleMenu(Monster* enemy);
    BattleOutcome run(Player& player, const map<string, Act>& catalogue);
private:
    void fight(Player& player);
    void act_menu(Player& player, const map<string, Act>& catalogue);
    bool item_menu(Player& player);
    bool try_mercy();
    void monster_attacks(Player& player);
};
