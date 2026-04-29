#include "classes.hpp"
#include "menu.hpp"
#include "reader.hpp"

static map<string, Act> build_catalogue() {
    return {
        {"JOKE",       Act("JOKE",       "Joke",       "You tell a terrible pun. The monster groans in agony.", 20)},
        {"COMPLIMENT", Act("COMPLIMENT", "Compliment", "You say the monster looks great today. It blushes a little.", 15)},
        {"INSULT",     Act("INSULT",     "Insult",     "You call the monster something unspeakable. It looks furious!", -20)},
        {"THREATEN",   Act("THREATEN",   "Threaten",   "You make a menacing pose. The monster cracks its knuckles.", -15)},
        {"COMFORT",    Act("COMFORT",    "Comfort",    "You gently pat the air near the monster. It seems... comforted?", 25)},
        {"MUSIC",      Act("MUSIC",      "Music",      "You hum a little tune. The monster starts dancing awkwardly.", 20)},
        {"ENCOURAGE",  Act("ENCOURAGE",  "Encourage",  "You cheer the monster on. It's confused but genuinely touched.", 15)},
        {"REASON",     Act("REASON",     "Reason",     "You explain that violence is pointless. The monster thinks about it.", 30)},
        {"FLATTER",    Act("FLATTER",    "Flatter",    "You say the monster has beautiful eyes. It stares at you blankly.", 10)},
        {"SPARE",      Act("SPARE",      "Spare",      "You hold out your hand in peace. The monster hesitates.", 35)},
    };
}

struct MonsterBlueprint {
    string category, name;
    int hp, atk, def, mercy_goal;
    vector<string> act_ids;
};

static vector<MonsterBlueprint> load_monster_blueprints(const string& filepath) {
    vector<MonsterBlueprint> blueprints;
    auto data = read_csv(filepath);
    if (data.empty()) {
        cerr << "Error: could not load monsters from " << filepath << "\n";
        exit(1);
    }
    for (auto& row : data) {
        if (row.size() < 8) {
            cerr << "Warning: malformed monster row, skipping.\n";
            continue;
        }
        MonsterBlueprint bp;
        bp.category = row[0];
        bp.name     = row[1];
        try {
            bp.hp         = stoi(row[2]);
            bp.atk        = stoi(row[3]);
            bp.def        = stoi(row[4]);
            bp.mercy_goal = stoi(row[5]);
        } catch (...) {
            cerr << "Warning: malformed stats for monster \"" << row[1] << "\", skipping.\n";
            continue;
        }
        for (int i = 6; i < (int)row.size(); i++) {
            if (row[i] != "-") bp.act_ids.push_back(row[i]);
        }
        blueprints.push_back(bp);
    }
    return blueprints;
}

static Monster* spawn_monster(const MonsterBlueprint& bp) {
    if (bp.category == "NORMAL")
        return new NormalMonster(bp.name, bp.hp, bp.atk, bp.def, bp.mercy_goal, bp.act_ids);
    if (bp.category == "MINIBOSS")
        return new MinibossMonster(bp.name, bp.hp, bp.atk, bp.def, bp.mercy_goal, bp.act_ids);
    return new BossMonster(bp.name, bp.hp, bp.atk, bp.def, bp.mercy_goal, bp.act_ids);
}

static void load_items(Player& player, const string& filepath) {
    auto data = read_csv(filepath);
    if (data.empty()) {
        cerr << "Error: could not load items from " << filepath << "\n";
        exit(1);
    }
    for (auto& row : data) {
        if (row.size() < 4) {
            cerr << "Warning: malformed item row, skipping.\n";
            continue;
        }
        try {
            player.add_item(Item(row[0], row[1], stoi(row[2]), stoi(row[3])));
        } catch (...) {
            cerr << "Warning: malformed item data, skipping.\n";
        }
    }
}

static void print_start_summary(Player& player) {
    cout << "\nName : " << player.get_name() << "\n";
    cout << "HP   : " << player.get_curr_health() << "/" << player.get_max_health() << "\n";
    cout << "Items:\n";
    for (auto& item : player.get_inventory()) {
        cout << "  - " << item.get_name() << " x" << item.get_quantity() << "\n";
    }
}

int main() {
    cout << "Welcome to ALTERDUNE\n\n";
    cout << "Enter your character's name: ";
    string player_name;
    getline(cin, player_name);
    if (player_name.empty()) player_name = "Chara";

    Player* player = new Player(player_name);

    load_items(*player, "data/items.csv");
    auto blueprints = load_monster_blueprints("data/monsters.csv");

    if (blueprints.empty()) {
        cerr << "Error: no monsters loaded. Exiting.\n";
        delete player;
        return 1;
    }

    print_start_summary(*player);

    map<string, Act> catalogue = build_catalogue();

    mt19937 rng(random_device{}());
    uniform_int_distribution<int> pick(0, (int)blueprints.size() - 1);

    // monsters created during combat — kept alive for GameStats pointers
    vector<Monster*> combat_monsters;

    Menu main_menu;
    bool running = true;
    bool defeated = false;

    while (running && player->get_stats().victory_count() < 10) {
        cout << "\n=== MAIN MENU ===  (wins: " << player->get_stats().victory_count() << "/10)\n";
        cout << "1. Bestiary\n";
        cout << "2. Start a fight\n";
        cout << "3. Character stats\n";
        cout << "4. Items\n";
        cout << "5. Quit\n";
        cout << "> ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            main_menu.show_bestiary(player->get_stats());

        } else if (choice == 2) {
            Monster* m = spawn_monster(blueprints[pick(rng)]);
            combat_monsters.push_back(m);

            BattleMenu battle(m);
            BattleOutcome result = battle.run(*player, catalogue);

            if (result == BattleOutcome::WIN_KILLED) {
                player->get_stats().add_killed(m);
                cout << "\nVictory! (" << player->get_stats().victory_count() << "/10 wins)\n";
            } else if (result == BattleOutcome::WIN_SPARED) {
                player->get_stats().add_spared(m);
                cout << "\nVictory! (" << player->get_stats().victory_count() << "/10 wins)\n";
            } else {
                defeated = true;
                running  = false;
            }

        } else if (choice == 3) {
            main_menu.show_stats(*player);

        } else if (choice == 4) {
            main_menu.show_inventory(*player);
            cout << "Use an item? Enter number (0 to cancel): ";
            int idx;
            cin >> idx;
            if (idx > 0) player->use_item(idx - 1);

        } else if (choice == 5) {
            running = false;

        } else {
            cout << "Invalid choice.\n";
        }
    }

    // Endings
    if (player->get_stats().victory_count() >= 10) {
        cout << "\n=== GAME COMPLETE ===\n";
        switch (player->get_stats().get_ending()) {
            case Ending::GENOCIDE:
                cout << "GENOCIDE ENDING\n";
                cout << "You slaughtered everything in your path. The dust settles.\n";
                cout << "There is nothing left.\n";
                break;
            case Ending::PACIFIST:
                cout << "PACIFIST ENDING\n";
                cout << "You spared every monster you encountered. The world is at peace.\n";
                cout << "Everyone gets a happy ending.\n";
                break;
            case Ending::NEUTRAL:
                cout << "NEUTRAL ENDING\n";
                cout << "You fought some and spared others.\n";
                cout << "The world remains... uncertain.\n";
                break;
        }
    } else if (defeated) {
        cout << "\n=== GAME OVER ===\n";
        cout << "You fell after " << player->get_stats().victory_count() << " victories.\n";
    } else {
        cout << "\nSee you next time.\n";
    }

    for (Monster* m : combat_monsters) delete m;
    delete player;
    return 0;
}
