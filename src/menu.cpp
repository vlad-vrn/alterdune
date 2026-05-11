#include "menu.hpp"

static int roll_damage(int max_dmg) {
    if (max_dmg <= 0) return 0;
    return rand() % (max_dmg + 1);
}

// Menu

void Menu::show_inventory(Player& player) {
    auto& inv = player.get_inventory();
    cout << "\n--- Inventory ---\n";
    if (inv.empty()) {
        cout << "Your bag is empty.\n";
        return;
    }
    for (int i = 0; i < (int)inv.size(); i++) {
        auto& item = inv[i];
        cout << i + 1 << ". " << item.get_name()
             << " - heals " << item.get_value() << " HP"
             << " - x" << item.get_quantity() << "\n";
    }
}

void Menu::show_stats(Player& player) {
    GameStats& stats = player.get_stats();
    cout << "\n--- Character Stats ---\n";
    cout << "Name   : " << player.get_name() << "\n";
    cout << "HP     : " << player.get_curr_health() << "/" << player.get_max_health() << "\n";
    cout << "Level  : " << player.get_lv() << "\n";
    cout << "Kills  : " << stats.killed_count() << "\n";
    cout << "Spared : " << stats.spared_count() << "\n";
    cout << "Wins   : " << stats.victory_count() << "/10\n";
}

void Menu::show_bestiary(GameStats& stats) {
    cout << "\n--- Bestiary ---\n";
    const auto& killed = stats.get_killed();
    const auto& spared = stats.get_spared();
    if (killed.empty() && spared.empty()) {
        cout << "You haven't defeated any monsters yet.\n";
        return;
    }
    for (Monster* m : killed) {
        cout << "[KILLED] " << m->get_name()
             << " (" << m->get_category() << ")"
             << "  HP:" << m->get_max_health()
             << "  ATK:" << m->get_attack()
             << "  DEF:" << m->get_defense() << "\n";
    }
    for (Monster* m : spared) {
        cout << "[SPARED] " << m->get_name()
             << " (" << m->get_category() << ")"
             << "  HP:" << m->get_max_health()
             << "  ATK:" << m->get_attack()
             << "  DEF:" << m->get_defense() << "\n";
    }
}

// BattleMenu

BattleMenu::BattleMenu(Monster* enemy) : enemy(enemy) {}

void BattleMenu::fight(Player& player) {
    (void)player;
    int dmg = roll_damage(enemy->get_max_health());
    if (dmg == 0) {
        cout << "You swing but miss!\n";
    } else {
        enemy->receive_attack(dmg);
        cout << "You deal " << dmg << " damage to " << enemy->get_name() << "!\n";
    }
}

void BattleMenu::act_menu(Player& player, const map<string, Act>& catalogue) {
    (void)player;
    const auto& act_ids = enemy->get_act_ids();
    int count = min(enemy->get_act_count(), (int)act_ids.size());

    cout << "\n--- ACT ---\n";
    for (int i = 0; i < count; i++) {
        auto it = catalogue.find(act_ids[i]);
        if (it != catalogue.end()) {
            cout << i + 1 << ". " << it->second.get_name() << "\n";
        }
    }
    cout << "0. Back\n> ";

    int choice;
    cin >> choice;
    if (choice <= 0 || choice > count) return;

    int idx = choice - 1;
    auto it = catalogue.find(act_ids[idx]);
    if (it == catalogue.end()) return;

    const Act& act = it->second;
    cout << act.get_display_text() << "\n";
    enemy->modify_mercy(act.get_mercy_impact());

    if (act.get_mercy_impact() > 0) {
        cout << enemy->get_name() << "'s mercy increased! ("
             << enemy->get_mercy() << "/" << enemy->get_mercy_goal() << ")\n";
    } else if (act.get_mercy_impact() < 0) {
        cout << enemy->get_name() << " looks more angry. ("
             << enemy->get_mercy() << "/" << enemy->get_mercy_goal() << ")\n";
    }
}

bool BattleMenu::item_menu(Player& player) {
    auto& inv = player.get_inventory();

    bool has_items = false;
    for (auto& item : inv) {
        if (item.get_quantity() > 0) { has_items = true; break; }
    }
    if (!has_items) {
        cout << "You have no items left!\n";
        return false;
    }

    cout << "\n--- Items ---\n";
    for (int i = 0; i < (int)inv.size(); i++) {
        if (inv[i].get_quantity() > 0) {
            cout << i + 1 << ". " << inv[i].get_name()
                 << " - heals " << inv[i].get_value() << " HP"
                 << " - x" << inv[i].get_quantity() << "\n";
        }
    }
    cout << "0. Back\n> ";

    int choice;
    cin >> choice;
    if (choice <= 0 || choice > (int)inv.size()) return false;
    if (inv[choice - 1].get_quantity() <= 0) {
        cout << "You have none of those left!\n";
        return false;
    }

    int before = player.get_curr_health();
    player.use_item(choice - 1);
    int healed = player.get_curr_health() - before;
    cout << "You used " << inv[choice - 1].get_name() << " and recovered " << healed << " HP!\n";
    return true;
}

bool BattleMenu::try_mercy() {
    if (enemy->is_sparable()) {
        cout << enemy->get_name() << " accepts your mercy. You spare them.\n";
        return true;
    }
    cout << enemy->get_name() << " won't accept your mercy yet. ("
         << enemy->get_mercy() << "/" << enemy->get_mercy_goal() << ")\n";
    return false;
}

void BattleMenu::monster_attacks(Player& player) {
    int dmg = roll_damage(enemy->get_attack());
    if (dmg == 0) {
        cout << enemy->get_name() << " attacks but misses!\n";
    } else {
        player.receive_attack(dmg);
        cout << enemy->get_name() << " deals " << dmg << " damage to you! ("
             << player.get_curr_health() << "/" << player.get_max_health() << " HP left)\n";
    }
}

BattleOutcome BattleMenu::run(Player& player, const map<string, Act>& catalogue) {
    cout << "\n* " << enemy->get_name() << " appears! *\n";

    while (enemy->is_alive() && player.is_alive()) {
        cout << "\n[" << player.get_name() << "]  HP: "
             << player.get_curr_health() << "/" << player.get_max_health() << "\n";
        cout << "[" << enemy->get_name() << "]  HP: "
             << enemy->get_curr_health() << "/" << enemy->get_max_health()
             << "   Mercy: " << enemy->get_mercy() << "/" << enemy->get_mercy_goal() << "\n";
        cout << "\n  FIGHT (1)   ACT (2)   ITEM (3)   MERCY (4)\n> ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            fight(player);
            if (!enemy->is_alive()) {
                cout << enemy->get_name() << " was defeated!\n";
                return BattleOutcome::WIN_KILLED;
            }
        } else if (choice == 2) {
            act_menu(player, catalogue);
        } else if (choice == 3) {
            if (!item_menu(player)) continue;
        } else if (choice == 4) {
            if (try_mercy()) return BattleOutcome::WIN_SPARED;
        } else {
            cout << "Invalid choice.\n";
            continue;
        }

        monster_attacks(player);
        if (!player.is_alive()) {
            cout << "\nYou were defeated...\n";
            return BattleOutcome::DEFEAT;
        }
    }

    return player.is_alive() ? BattleOutcome::WIN_KILLED : BattleOutcome::DEFEAT;
}
