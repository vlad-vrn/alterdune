#include "config.hpp"
#include "classes.hpp"

void battle(){
  Player* Frisk = new Player();
  Monster* Dummy = new Monster(10, "Dummy", 1, {});
  Monster* MadDummy = new Monster(15, "MadDummy", 5, {});
  Encounter* Battle = new Encounter(Frisk, {Dummy, MadDummy});
  Battle->player_turn();
  delete Frisk;
  delete Dummy;
  delete MadDummy;
  delete Battle;
}


void test_fight(){
  Item* ButtPie = new Item("ButtPie");
  Player* Frisk = new Player();
  cout << Frisk->get_curr_health() << endl;
  Monster* Napstablook = new Monster();
  Frisk->receive_attack(Napstablook);
  Frisk->attack_monster(Napstablook);
  delete Napstablook;
  delete Frisk;
  delete ButtPie;
}

int main(){
  battle();
  return 0;
}



