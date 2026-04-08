#include "classes.hpp"

void Player::receive_attack(const Monster* monster){
  int new_hp = curr_health - monster->get_attack();
  cout << monster->get_name() << " attacked you !" << endl;
  cout << "You have now " << new_hp << " HP !" << endl;
  curr_health = new_hp;
}


void Monster::receive_attack(int damages){
  curr_health = curr_health - damages;
}

void Player::attack_monster(Monster* monster){
  cout << "You are attacking " << monster->get_name() << " !" << endl;
  monster->receive_attack(attack);
  cout << "He has now " << monster->get_hp() << " HP !" << endl;
}

void Encounter::player_turn(){
  cout << "It's your turn !" << endl;
  cout << "In front of you are : " << endl;
  for(size_t i = 0 ; i < monster_list.size(); i++){
    cout << monster_list[i]->get_name() << endl;
  }
}
