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

// Lit le CSV et crée directement les objets Monster correspondants
static vector<Monster*> charger_monstres(const string& filepath) {
    vector<Monster*> monstres;
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
        string categorie = row[0];
        string nom       = row[1];
        int hp, atk, def, mercy_goal;
        try {
            hp         = stoi(row[2]);
            atk        = stoi(row[3]);
            def        = stoi(row[4]);
            mercy_goal = stoi(row[5]);
        } catch (...) {
            cerr << "Warning: malformed stats for monster \"" << row[1] << "\", skipping.\n";
            continue;
        }
        vector<string> act_ids;
        for (int i = 6; i < (int)row.size(); i++) {
            if (row[i] != "-") act_ids.push_back(row[i]);
        }
        if (categorie == "NORMAL")
            monstres.push_back(new NormalMonster(nom, hp, atk, def, mercy_goal, act_ids));
        else if (categorie == "MINIBOSS")
            monstres.push_back(new MinibossMonster(nom, hp, atk, def, mercy_goal, act_ids));
        else
            monstres.push_back(new BossMonster(nom, hp, atk, def, mercy_goal, act_ids));
    }
    return monstres;
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
    srand(time(0));

    cout << "Welcome to ALTERDUNE\n\n";
    cout << "Enter your character's name: ";
    string player_name;
    getline(cin, player_name);
    if (player_name.empty()) player_name = "Chara";

    Player* player = new Player(player_name);

    load_items(*player, "data/items.csv");
    vector<Monster*> tous_monstres = charger_monstres("data/monsters.csv");

    if (tous_monstres.empty()) {
        cerr << "Error: no monsters loaded. Exiting.\n";
        delete player;
        return 1;
    }

    print_start_summary(*player);

    map<string, Act> catalogue = build_catalogue();

    // Séparer les monstres normaux/miniboss des boss
    vector<Monster*> monstres_normaux, monstres_boss;
    for (Monster* m : tous_monstres) {
        if (m->get_category() == "BOSS") monstres_boss.push_back(m);
        else                             monstres_normaux.push_back(m);
    }

    Menu main_menu;
    bool running  = true;
    bool defeated = false;

    while (running && player->get_stats().victory_count() < 1) {
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
            // Rencontre 5 → premier boss, rencontre 10 → deuxième boss
            int num_rencontre = player->get_stats().victory_count() + 1;
            Monster* m;
            if ((num_rencontre == 5 || num_rencontre == 10) && !monstres_boss.empty()) {
                int idx = (num_rencontre == 5) ? 0 : min(1, (int)monstres_boss.size() - 1);
                m = monstres_boss[idx];
            } else {
                m = monstres_normaux[rand() % monstres_normaux.size()];
            }
            m->reset();

            BattleMenu battle(m);
            BattleOutcome result = battle.run(*player, catalogue);

            if (result == BattleOutcome::WIN_KILLED) {
                player->get_stats().add_killed(m);
                player->augmenter_attaque(1);
                cout << "\nVictory! Your attack increased to " << player->get_attack()
                     << "! (" << player->get_stats().victory_count() << "/10 wins)\n";
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
    if (player->get_stats().victory_count() >= 1) {
        cout << "\n=== GAME COMPLETE ===\n";
        switch (player->get_stats().get_ending()) {
            case Ending::GENOCIDE:
                cout << "GENOCIDE ENDING\n";
                cout << "Greetings. I am " << player->get_name() << "..\n";
                cout << "The demon that comes whem people call its name.\n";
                cout << "Let us rease this pointless world, and move on to the next.\n";
                cout << "9999999999999999999999999999999999999999999999.\n9999999999999999999999999999999999999999999999.\n9999999999999999999999999999999999999999999999.\n";
                break;
            case Ending::PACIFIST:
                cout << "PACIFIST ENDING\n";
                cout << "You were able to reach the surface.\n";
                cout << "\"" << player->get_name() << "\"...\n";
                cout << "You came from this world, right ?\n";
                cout << "If you really do not have any other place to go...\n";
                cout << "I will do my best to take care of you, for as long as you need\n";
                break;
            case Ending::NEUTRAL:
                cout << "NEUTRAL ENDING\n";
                cout << "Is killing things really necessary ?.\n";
                cout << "I...\n";
                cout << "I honestly don't know anymore\n";
                break;
        }
    } else if (defeated) {
        cout << "\n=== GAME OVER ===\n";
        cout << "You cannot give up just yet...\n";
    } else {
        cout << "\nYou are filled with determination\n";
    }

    for (Monster* m : tous_monstres) delete m;
    delete player;
    return 0;
}
