#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>

using namespace std;

char PetNames[7] = {'R', 'A', 'J', 'V', 'B', 'C', '\0'};
char UnCapitalPetNames[7] = {'r', 'a', 'j', 'v', 'b', 'c', '\0'};
int gameTime = 60; // Will loop from 1000 to 0, once it hits zero it will 'restart' the day
int randarr[155];
int randarr2[221]; // to emulate randomness 3 variables are needed for 'garbage' that comes out from a system
int randIndex;

int wrapIdx(int i, int mod) { // keeps array indices safely in bounds (positive modulo)
  i %= mod;
  if (i < 0) i += mod;
  return i;
}

bool Failure;
bool PlayerLeftStore; // set by scenes::Introduction() when the player declines to adopt a pet
int days = 0;
int vists = 0;

void You_broke_This() {
  while (true) {
    cout << "Lobster does not support a method to fix wrong input, "
    << "shouldve been more careful bucko, now you're stuck here."
    << endl;

    cin.get();
  }
}

void Non_app_choice() {
  cout << endl << endl << "Not an Option, please chose again" << endl << endl;
}

char char_choose() {
  char char_choice;
  cout << endl << "> ";
  cin >> char_choice;
  cout << endl;

  if (cin.eof()) { // input stream exhausted, exit cleanly instead of recursing forever
    cout << endl << "No more input available, exiting." << endl;
    std::exit(0);
  }

  if (cin.fail()) {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    Non_app_choice();
    return char_choose();
  }

  if (char_choice == '-') { /// henceforth rather than causing complications this dash will be used for inappliable space and choices
    Non_app_choice();
    return char_choose();
  }

  return char_choice;
}

int num_choose() {
  int num_choice;

  cout << endl << "> ";
  cin >> num_choice;
  cout << endl;

  if (cin.eof()) { // input stream exhausted, exit cleanly instead of looping forever
    cout << endl << "No more input available, exiting." << endl;
    exit(0);
  }

  if (cin.fail()) {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    You_broke_This();
  }

  return num_choice;
}

class Player;

class pet {
  private:
    Player* owner;
    bool dead;
    int difficulty = 1; // equivalent to star
    int saturation; // all stats 1-100
    int hydration;
    int energy;
    int health;
    int mental_health;
    int condition; // Conidition will have 5 states,0 sleeping,1 awake,2 Happy,3 enraged,4 sad
    int BedQuality = 0;
    int TimeCounter = 0;

    void ConditionDescription() {
      randIndex += 1;
      if (condition == 0) {
        if ((randarr[wrapIdx(randIndex,155)] % 2) == 1) {cout << endl << endl << "They are sleeping soundly";}
        else {cout << endl << endl << "light breathing comes from their chest";}
      }
      if (condition == 1) {
        if ((randarr[wrapIdx(randIndex,155)] % 2) == 1) {cout << endl << endl << "They are staring up at you questioningly";}
        else {cout << endl << endl << "They are going around aimlessly";}
      }
      if (condition == 2) {
        if ((randarr[wrapIdx(randIndex,155)] % 2) == 1) {cout << endl << endl << "They excitedly bounce around";}
        else {cout << endl << endl << "They shake their whole body like a dance";}
      }
      if (condition == 3) {
        if ((randarr[wrapIdx(randIndex,155)] % 2) == 1) {cout << endl << endl << "Only fury and descruction burn in their eyes.";}
        else {cout << endl << endl << "Something bad might happen to you if you dont do something..";}
      }
      if (condition == 4) {
        if ((randarr[wrapIdx(randIndex,155)] % 2) == 1) {cout << endl << endl << "They weep around in a corner.";}
        else {cout << endl << endl << "They hang their head in sorrow.";}
      }
    }

  public:
    // Each pet will henceorth be refered to by ID #.

    void SetOwner(Player* p)
    {
      owner = p;
    }

    void SetDifficulty(int level) {
      if (level < 1) {
        difficulty = 1;
      }
      else if (level > 10) {
        difficulty = 10;
      }
      else {
        difficulty = level;
      }
    }

    int GetDifficulty() {
      return difficulty;
    }

    void Kill() {dead = true;} // debug helper, publicly exposed

    bool Death() {
      return health <= 0;
    }

    bool sleep()
    {
      if(condition != 0)
      {
        condition = 0;
        return true;
      }
      return false;
    }

    void TimePassing(int TimePassed) {
      saturation -= difficulty * (TimePassed / 5);
      hydration -= difficulty * (TimePassed / 10);
      energy -= difficulty * (TimePassed / 5);
      mental_health -= difficulty * (TimePassed / 10);

      if (saturation < 20 || hydration < 20) {
        health -= difficulty * 2;
      }

      if (mental_health < 20) {
        health -= difficulty;
        condition = 4;
      } else if (mental_health > 80)
        condition = 2;

      // Prevent health from going below zero
      if (health < 0) {
        health = 0;
      }

      if (Death()) {
        dead = true;
      }

      randIndex++;
    }

    void Reset() {
      if (dead != true) {
        energy = 100;
        condition = 1;
      }
    }

    void PetInfo() {
    }

    int RandIsValidHigh(int array) { // for high values like 90
      bool valid = true;
      array = array % 155;
      if (array < 0) array += 155;
      while (valid) {
        if ((randarr[array]+20/difficulty) > 100 - (difficulty*10)) {
          valid = false;
          int result = randarr[array]-20/difficulty;
          return (result < 1) ? 1 : result; // stats are documented as 1-100, so clamp the upper end
        }
        else {
          array = (array + 1) % 155;
        }
      }
      return 50; // fallback, unreachable in practice
    }

    int RandIsValidLow(int array) { // for low values like 20 or smth
      bool valid = true;
      array = array % 155;
      if (array < 0) array += 155;
      while (valid) {
        if (randarr[array]-20/difficulty < 100 - (difficulty*10)) {
          valid = false;
          int result = randarr[array] - 20/difficulty;
          return (result < 1) ? 1 : result;
        }
        else {
          array = (array + 1) % 155;
          cout << array;
        }
      }
      return 50; // fallback, unreachable in practice
    }

    void initialize(int level) {
      SetDifficulty(level);

      saturation = RandIsValidHigh(difficulty+1);
      energy = RandIsValidHigh(difficulty+61);
      health = RandIsValidHigh(difficulty+32);
      hydration = RandIsValidHigh(difficulty+15);
      mental_health = RandIsValidHigh(difficulty+22);

      if (difficulty == 10) {
        condition = 3;
      }
      else {
        condition = 1; // awake by default; only the difficulty==10 special case overrides this
      }

      dead = false;
    }

    bool Report_Characteristics() {
      if (dead == true) {
        cout << endl << "dead." << endl;
        Failure = true;
        return false;
      }
      cout << "Rated a " << difficulty << " star pet." << endl
        << "Saturation: " << saturation << endl
        << "Hydration: " << hydration << endl
        << "Energy: " << energy << endl
        << "Health: " << health << endl
        << "Mental Health: " << mental_health << endl;
      ConditionDescription(); // was never being called anywhere - mood flavor text was dead code
      cout << endl << endl;
      return true;
    }

    bool FeedPet(); //  The following are all bool to deterime whenever they are sucessfull

    bool HydratePet();

    bool PlayPet() {
      if (dead == true) {Failure = true; return false;}
      if (energy > 5*difficulty and condition != 0) {
        mental_health += 20/difficulty;
        energy -= 5*difficulty;
        if (mental_health > 100) {mental_health = 100;}
        if (condition == 4) {condition = 3;}
        cout << endl << endl << "You play with your pet and enjoy some valuable leasuire time." << endl << endl;
      }
      else if (condition == 4) {cout << endl << endl << "Pet is enraged with you! Cannot complete action" << endl << endl;}
      else if (energy < 5*difficulty) {cout << endl << endl << "Requires more energy" << endl;}
      else if (condition == 0) {cout << endl << endl << "They're sleeping idiot." << endl << endl;}
      else {cout << "something is dearly wrong, youve made a wrong turn player!";}
      return true;
    }

    bool RestPet() {
      if (dead == true) {Failure = true; return false;}
      if (condition != 0) {
        condition = 0;
        energy += 100 - 2*difficulty;
        if (energy > 100 + BedQuality)
        {
          energy = 100 + BedQuality;
        }
      }
      else {cout << endl << endl << "Already sleeping dingus" << endl << endl;}
      return true;
    }

    bool GiveEnergyDrink(); // a quick pick-me-up: smaller boost than a full Rest, but doesn't put the pet to sleep

    bool PetAction() {
      if (dead == true) {Failure = true; return false;}
      while (true) {
        cout << endl << endl << "Choose an action to do to your pet. '0' for exit" << endl << endl
          << "Options Feed Pet (1), Hydrate Pet (2), Play Pet(3), Rest Pet(4), Energy Drink(5).";
        int PetActionTemp = num_choose();
        if (PetActionTemp == 99) {return false;}
        if (PetActionTemp == 1) {return FeedPet();}
        else if (PetActionTemp == 2) {return HydratePet();}
        else if (PetActionTemp == 3) {return PlayPet();}
        else if (PetActionTemp == 4) {return RestPet();}
        else if (PetActionTemp == 5) {return GiveEnergyDrink();}
        else if (PetActionTemp == 0) {return false;}
        else {Non_app_choice();}
      }
    }
};
pet pets[7];
/* From the First pet onwards it wil be logged. if one has only one pet they cant acess the others as pet "-" is not a
valid pet entry. CurrentPets[i] will return a character such as 'r', hence when pets[i] is accessed it will access the first
pet obtained. we can just detail the pet the moment one gets it.

*/

class Player {
public:
  int money = 20;
  int PetFood[3] = {0, 0, 0};
  int water = 0;
  int EnergyDrinks = 0;

  char CurrentPets[7] = {'-', '-', '-', '-', '-', '-', '\0'};
  int TotalPetNumber = 0;

  void PetFoodReport() {
    cout << PetFood[0] << " "
    << PetFood[1] << " "
    << PetFood[2];
  }

  void StatReport() {
    cout << "You currently have $" << money << endl;
    cout << "Pet Food stock: ";
    PetFoodReport();
    cout << endl;
    cout << "Water: " << water << endl;
    cout << "Energy Drinks: " << EnergyDrinks << endl << endl;
  }
};
Player player;

bool pet::FeedPet() {
  if (dead == true) {Failure = true; return false;}
  cout << endl << "Which type of pet food? 1 2 3?";
  int FeedPetTemp = num_choose();
  if (FeedPetTemp == 99) {return true;}
  if (FeedPetTemp >= 1 and FeedPetTemp <= 3 and owner->PetFood[FeedPetTemp-1] > 0) {
    owner->PetFood[FeedPetTemp-1] -= 1;
    saturation += (20/difficulty)*FeedPetTemp*FeedPetTemp;
    if (saturation > 100) {saturation = 100;}
    cout << endl << "Pet Food Stock now: ";
    owner->PetFoodReport();
  }
  else {cout << "No pet food left or not an option, go buy some brokie" << endl << endl;}
  return true;
}

bool pet::HydratePet() {
  if (dead == true) {Failure = true; return false;}
  if (owner->water > 2*difficulty) {
    cout << endl << "Pouring water... Taking a sip... Done!" << endl << endl;
    owner->water -= 2*difficulty;
    hydration += 60/difficulty;
    if (hydration > 100) {hydration = 100;}
  }
  else {
    cout << endl << endl << "Pay bills. Brokie";
  }
  return true;
}

bool pet::GiveEnergyDrink() {
  if (dead == true) {Failure = true; return false;}
  if (condition == 0) {
    cout << endl << endl << "They're fast asleep, better let them rest." << endl << endl;
  }
  else if (owner->EnergyDrinks > 0) {
    owner->EnergyDrinks -= 1;
    energy += 60/difficulty;
    if (energy > 100) {energy = 100;}
    cout << endl << endl << "You crack open an energy drink and your pet chugs it down, perking right up!" << endl << endl;
  }
  else {
    cout << endl << endl << "No energy drinks left, go buy some." << endl << endl;
  }
  return true;
}

void PetFoodReport() {
  cout << player.PetFood[0] << " " << player.PetFood[1] << " " << player.PetFood[2];
}

void StatReport() {
  cout << "You currently have $" << player.money << endl << "Pet Food stock: ";
  PetFoodReport();
  cout << endl << "Water: " << player.water << endl << "Energy Drinks: " << player.EnergyDrinks << endl << endl;
}

class scenes {
  private: // for actionable things within the public
    // 0 = success, 1 = wrong case, 2 = not a valid pet letter at all
    int PetIsInStore(char PetInitial) { // selecting a pet
      for (int i = 0; i < 6; i++) {
        if (PetNames[i] == PetInitial) {
          player.CurrentPets[i] = PetNames[i];
          if (i == 5)
            pets[i].SetDifficulty(10);
          else
            pets[i].SetDifficulty(i + 1);
          cout << endl << endl << "Please wait for 'randomization' of pet" << player.CurrentPets << endl << endl;
          pets[i].SetOwner(&player);
          pets[i].initialize(i == 5 ? 10 : i + 1);
          PetNames[i] = '-';
          UnCapitalPetNames[i] = '-';
          return 0;
        }
        else if (UnCapitalPetNames[i] == PetInitial) {
          cout << endl << endl << "The owner replies: You know its disrespectful to not capitalize something's name right? Lets try that again";
          return 1;
        }
      }
      return 2;
    }

    bool WorkScene() {
      randIndex += randarr[wrapIdx(randIndex-23,155)]-randarr2[wrapIdx(randIndex+23,221)];
      if ((randIndex*randIndex)-12 <= 26) {
        cout << "Day is never finished," << endl <<
          "Master got me working" << endl <<
          "Someday Massa set me free..." << endl << endl <<
          "money + $100";
        player.money += 100;
      }
      else if ((randIndex+2)%5 == 0 && (randIndex+2) > 14) {
        cout << "You're pretty sure your coworker" << endl <<
          "is flirting with you." << endl <<
          "Unfortunately you have no game." << endl <<
          "she gives you $50 adding to your paycheck" << endl << endl << "+$50" << endl;
        player.money += 50;
      }
      else if (randIndex%2 == 0) {
        cout << "You work and work, for such a small amount," << endl <<
          "you curse this economy but can't really do anything" << endl << endl <<
          "+$10";
      }
      else if (randIndex%2 != 0) {
        cout << endl << "'What would happen if my boss died?'" << endl
          << "Along with other intrustive thoughts" << endl <<
          "cross your mind, though its probably best" << endl <<
          "that you never act on it" << endl << endl <<
          "+$10";
        player.money += 10;
      }
      return true;
    }

    bool OutsideScene() {
      randIndex -= randarr2[wrapIdx(randIndex+1,221)];
      if ((randIndex*randIndex)-12 <= 16) {
        cout << "You could've sworn you saw the" << endl <<
          "Magician from the store walk by," << endl <<
          "but upon confronting him, it turned" << endl <<
          "out to just be a werid guy." << endl << endl <<
          "He seems high... He gives you a 8 pack of alchol" << endl <<
          "Seems to be water that looks like alchol though" << endl <<
          "He must really be high..." << endl << endl <<
          "+ 8 water" << endl << endl;
        player.water += 8;
      }
      else if ((randIndex+2)%5 == 0) {
        cout << "You meet a dog, it puts out its paw." << endl <<
          "You hold your hand out and..." << endl <<
          "?! The dog gives you 50$??" << endl <<
          "You attempt to interrogate the dog," << endl <<
          "but it provides no answer and the" << endl <<
          "owner soon arrives." << endl << endl << "+50$?" << endl << endl;
        player.money += 50;
      }
      else if (randIndex%2 == 0) {
        cout << "You look at the people walking by, they're all strangers." << endl << endl;
      }
      else if (randIndex%2 != 0) {
        cout << "You go outside and taste the fresh air, its nice." << endl << endl;
      }

      cout << "After you head to the plaza, you question where to go." << endl << endl;
      bool OutsideVar = true;
      while (OutsideVar) {
        cout << endl << endl << "Options: (0) exit plaza (1) The Casino. (2) Alice's Allusions. (3) Look for a different job." << endl;
        int OutsideChoice = num_choose();
        if (OutsideChoice == 99) {OutsideVar = false;}
        if (OutsideChoice == 1) {
          cout << endl << endl << "You arrive at the entrance, the bouncer requests a membership card." << endl <<
            "He also states that it would be way to complicated to" << endl <<
            "add gambling and also its not that type of game." << endl << endl <<
            "whatever that means..";
        }
        else if (OutsideChoice == 2) {
          cout << endl << endl << "And then just like a certain game, you encounter a lady" << endl <<
            "working the counter and hit it off with her" << endl <<
            "though unlike the game, this path slowly turns into" << endl <<
            "a dating sim that has 15 endings." << endl << endl <<
            "Yeah not happening, just go back inside your house.";
        }
        else if (OutsideChoice == 3) {
          cout << endl << endl << "In this economy??" << endl;
        }
        else if (OutsideChoice == 0) {OutsideVar = false;}
      }
      return true;
    }

    bool StoreScene() {
      bool store = true;
      while (store) {
        cout << endl << endl << "You arrive in a store, theres some stuff to buy, not much though. You suppose thats what poverty is like." << endl
          << "Theres multiple isles and options, 0 for exit, 1: low quality Pet Food (10$), 2: Pet Food (20$), 3: Expensive Pet Food (50$) 4: 12 Water bottles (10$), 5: An inflation-driven energy drink (35$) " << endl << endl;
        int StoreActionTemp = num_choose();
        if (StoreActionTemp == 1) {
          if (player.money >= 10) {player.PetFood[0] += 1; player.money -= 10;}
          else {cout << endl << "Brokie" << endl;}
        }
        else if (StoreActionTemp == 2) {
          if (player.money >= 20) {player.PetFood[1] += 1; player.money -= 20;}
          else {cout << endl << "Brokie" << endl;}
        }
        else if (StoreActionTemp == 3) {
          if (player.money >= 50) {player.PetFood[2] += 1; player.money -= 50;}
          else {cout << endl << "Brokie" << endl;}
        }
        else if (StoreActionTemp == 4) {
          if (player.money >= 10) {player.water += 12; player.money -= 10;}
          else {cout << endl << "Brokie" << endl;}
        }
        else if (StoreActionTemp == 5) {
          if (player.money >= 35) {player.EnergyDrinks += 1; player.money -= 35;}
          else {cout << endl << "Brokie" << endl;}
        }
        else if (StoreActionTemp == 0) {
          store = false;
        }
        else {Non_app_choice();}
      }
      return true;
    }

  public:
    bool TimeOut(int TimeSpent) {
      gameTime -= TimeSpent;
      if (gameTime < 0) {
        cout << "You feel sleepy so you hit the hay, heading back to your house and going to sleep." << endl << endl << "... zzz ...";
        for (int i = 0; i < 6; i++) {
          if (player.CurrentPets[i] != '-') {
            pets[i].TimePassing(TimeSpent);
            pets[i].Reset();
          }
        }
        cout << endl << endl << "You wake up to a new day.";
        days += 1;
        gameTime = 60;
        return true;
      }
      else {
        for (int i = 0; i < 6; i++) {
          if (player.CurrentPets[i] != '-') {
            pets[i].TimePassing(TimeSpent);
          }
        }
        cout << endl << TimeSpent << " units o' time has been spent, remaining: " << gameTime << endl;
        return false;
      }
    }

    bool restart() {
      cout << endl << endl << "Restart? Y/N" << endl << "> ";
      char NoYes; // I know this is conusing but this is a separate function that asseratains if you want to restart not a choice confrimation
      cin >> NoYes;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      if (cin.eof()) {cout << endl << "No more input available, exiting." << endl; std::exit(0);}
      if (NoYes == 'y' or NoYes == 'Y') {return true;}
      else if (NoYes == 'n' or NoYes == 'N') {return false;}
      else {
        Non_app_choice();
        return restart();
      }
    }

    bool confirmation() {
      char YesNo;
      cout << endl << endl << "Are you certain this is your choice?" << endl << "Y/N?" << endl << "> ";
      cin >> YesNo;
      if (cin.eof()) {cout << endl << "No more input available, exiting." << endl; std::exit(0);}
      if (YesNo == 'y' or YesNo == 'Y') {
        return true;
      }
      else if (YesNo == 'n' or YesNo == 'N') {
        return false;
      }
      else {
        Non_app_choice();
        return confirmation();
      }
    }

    void Introduction() {
      bool introVar = true;
      bool WantsPet = true; // tracks whether the player chose to adopt or walked away
      while (introVar) {
        int num = num_choose();
        if (num == 1) {
          cout << endl << endl << "Ralph, a large blue dog. 1 Star" << endl <<
            "Alice, a little green bird. 2 Star" << endl <<
            "Jeff, a land shark. 3 Star" << endl <<
            "VERK, the destoryer of worlds. 4 Star" << endl <<
            "Bob, a human..?? 5 Star" << endl <<
            "Charlie, a black infant dragon with a blue nose. ??? star" << endl << endl <<
            "Make a choice with the first letter of the pet's name" << endl;
          introVar = false;
        }
        else if (num == 2 or num == 99) {
          cout << endl << endl << "You turn around, and decide that pets aren't for you. Eventually you pass by the store once more, or at least where it used to be..." << endl <<
            " The store is no longer there, but what remains is a empty lot of grass, a notice is posted to the door" << endl << endl <<
            " This store can no longer sustain the animals and pets within, it is with a heavy heart that I have to inform prospective owners that all the pets..." << endl <<
            " ... Have gone to a better place, I hope there they can find peace..." << endl << endl << endl
            << "GAME OVER";
          introVar = false;
          WantsPet = false;
        }
        else {
          Non_app_choice();
        }
      }
      PlayerLeftStore = !WantsPet; // let main() know whether to skip the rest of this round
    }

    void S2() { //Scene is already declared in scenes, S2 stands for scene but its kinda redundant lol!
      bool S2Var = true;
      while (S2Var) {
        char temp = char_choose();
        int result = PetIsInStore(temp);
        if (result == 0) {
          player.TotalPetNumber++;
          cout << "The man at the counter unlocks the chamber with your pet, he places it on the floor and it walks toward you." << endl
            << "You leave the store, walking out to the front with your pet." << endl
            << "You turn back around, and see the store gone. An ordinary empty building stands in its place" << endl << endl
            << "your Current Roster is now: " << player.TotalPetNumber << " Containing Current Pets:" << player.CurrentPets;
          S2Var = false;
        }
        else if (result == 2) {
          // pick just one of the two "invalid" messages at random, not both
          if (randarr[wrapIdx(randIndex,155)] % 2 == 0) {Non_app_choice();}
          else {cout << endl << "Not applicable, choose differently";}
        }
        // result == 1 (wrong case) already prints its own message above, nothing more to add here
      }
    }

    bool PetRosterInfo() {
      if (Failure) {
        return false;
      }

      cout << endl << endl
      << "Current pet roster is as following:"
      << player.CurrentPets
      << endl << endl
      << "select a pet with its initial or '/' to back out.";

      char PetRosterInfoTemp = char_choose();

      if (PetRosterInfoTemp == '/') {
        return false;
      }

      for (int i = 0; i < 6; i++) {
        if (player.CurrentPets[i] == PetRosterInfoTemp) {

          pets[i].Report_Characteristics();

          if (!pets[i].PetAction()) {
            return PetRosterInfo();
          }

          return true;
        }
      }

      Non_app_choice();
      return PetRosterInfo();
    }

    void Visitation() {
      cout << endl << endl << endl << "Its been a while since you recieved your pet." << endl <<
        "it so happens that you walk by the same place again," << endl <<
        "and like magic, the store is there already." << endl <<
        "You see the desperate pets in the windows, clutching" << endl <<
        "your own, you push the door open." << endl <<
        "The owner looks up happily to see you." << endl <<
        "The store seems much less dreary and spooky than" << endl <<
        "the last time, its not like you even" << endl <<
        "gave him any money, so how..?" << endl << endl <<
        "   'Heh heh, thanks to you things have been" << endl <<
        "   looking up for all these fellas! Thank you." << endl <<
        "   really! I fixed up the shop a little, its" << endl <<
        "   nicer now, no? Anyways did you want to adopt" << endl <<
        "   another one? You did so well with this, im" << endl <<
        "   sure it will be the same! So wont you take" << endl <<
        "   another one?'";
      bool vistLoop = true;
      while (vistLoop) {
        cout << endl << endl << "(1) Yeah, sure... (2) Nah I'm good";
        int num2 = num_choose();
        if (num2 == 1) {
          cout << endl << endl << "Ralph, a large blue dog. 1 Star" << endl <<
            "Alice, a little green bird. 2 Star" << endl <<
            "Jeff, a land shark. 3 Star" << endl <<
            "VERK, the destoryer of worlds. 4 Star" << endl <<
            "Bob, a human..?? 5 Star" << endl <<
            "Charlie, a black infant dragon with a blue nose. ??? star" << endl << endl <<
            "Make a choice with the first letter of the pet's name" << endl;
          vistLoop = false;
          S2();
        }
        else if (num2 == 2 or num2 == 99) {
          Failure = true;
          cout << endl << endl << "..." << endl <<
            "The magician pauses, his smile falters for a second." << endl <<
            "But he regains composure, and says" << endl << endl <<
            "    'Thats fine... I mean... you are sure right?" << endl <<
            "     well, regaudless.. you arent exactly" << endl <<
            "     what I hoped for... Well fine then" << endl <<
            "     , just go home.'" << endl;
          cout << endl << endl << "And so like that you left the store, heading home." << endl <<
            "Similar to the first time, the store dissappears." << endl <<
            "You pick up your pace to your house...";
          vistLoop = false;
        }
        else {Non_app_choice();}
      }
    }

    void Consequences() {
      cout << endl << endl << endl << endl << endl << endl
        << "                    ..?" << endl << endl << endl <<
        "    You hear a knock at your door." << endl << endl <<
        "a familiar voice rings out beyond the door. " << endl <<
        "              Its him." << endl <<
        "            The Magician" << endl << endl <<
        "But its different this time, his voice" << endl <<
        "is pratically fried and grumbling" << endl <<
        "like the deepest abyss. Its dripping" << endl <<
        "              with malice..." << endl << endl << endl <<
        "'When we first met, I thought you" << endl <<
        "would do a diligent job..." << endl <<
        "But now..? Look at what you've done" << endl <<
        "and to think I wouldn't notice..?" << endl <<
        "Well... Does that matter now?'" << endl << endl << endl <<
        "Slowly... Your front door handle creaks";
      cout << endl <<
        "The door handle groans as the force" << endl <<
        "of the Magician's grip decimates it" << endl <<
        "The hindges on the door snap." << endl <<
        "A foot comes through the crack in " << endl <<
        "the door, and the figure its attached to..." << endl <<
        "is the Magician, The keeper of the pets," << endl <<
        "            A great unknown." << endl <<
        "You step back, but by then the Magician " << endl <<
        "has already fully stepped into the house." << endl << endl <<
        "He looks up at you, his hair swaying" << endl <<
        "at a shoulder's length despite the " << endl <<
        "lacking draft in the room. His hair" << endl <<
        "sways to reveal two crimson red eyes." << endl <<
        "He takes off his Magician's hat to his" << endl <<
        "chest, and bows. A deep bow. A somber bow." << endl <<
        "         A bow full of resentment" << endl << endl <<
        "And his hair darkens to an unnatural void." << endl <<
        "         Colors shift to swirls," << endl <<
        "         As the world whirls," << endl <<
        "         White gets whiter," << endl <<
        "         Black gets blacker," << endl <<
        "And the world before you gets engulfed" << endl <<
        "in red. Crimson red. Just for a second," << endl <<
        "and a moment. It flashes until everything" << endl <<
        "         ..." << endl <<
        "is over..." << endl << endl << endl << endl << endl <<
        "GAME OVER, You could not bear the weight " << endl <<
        "of your responsibility.";
    }

    void Home() { // a gameplay loop lasts or 1 day.
      bool GameplayLoop = true;
      while (GameplayLoop) {
        if (Failure) {GameplayLoop = false; Consequences(); break;}
        if (days/10 == 1 && vists == 0) {Visitation(); vists += 1;}
        cout << "What would you like to do today?" << endl << endl << "Options: Select pet (1), Go to the store (2), Go outside (3), Work (4), Stat Report (5)";
        int HomeChoice = num_choose();
        if (HomeChoice == 1) {
          if (TimeOut(2)) {}
          else if (PetRosterInfo()) {}
        }
        else if (HomeChoice == 2) {
          if (TimeOut(10)) {}
          else if (StoreScene()) {}
        }
        else if (HomeChoice == 3) {
          if (TimeOut(20)) {}
          else if (OutsideScene()) {}
        }
        else if (HomeChoice == 4) {
          if (TimeOut(40)) {}
          else if (WorkScene()) {}
        }
        else if (HomeChoice == 5) {StatReport();}
        else if (HomeChoice == 67) {pets[0].Kill();}
        else if (HomeChoice == 99) {GameplayLoop = false;}
        else {Non_app_choice();}
      }
    }
}; // for the scenes class

void ResetGame() {
  player.TotalPetNumber = 0;

  char NewPetNames[7] = {'R','A','J','V','B','C','\0'};
  char NewLowerNames[7] = {'r','a','j','v','b','c','\0'};

  for (int i = 0; i < 6; i++) {
    player.CurrentPets[i] = '-';
    PetNames[i] = NewPetNames[i];
    UnCapitalPetNames[i] = NewLowerNames[i];
  }

  player.CurrentPets[6] = '\0';
  PetNames[6] = '\0';
  UnCapitalPetNames[6] = '\0';

  player.money = 20;

  for (int i = 0; i < 3; i++)
    player.PetFood[i] = 0;

  player.PetFood[0] = 2;
  player.water = 4;
  player.EnergyDrinks = 0;

  days = 0;
  vists = 0;
  gameTime = 60;

  Failure = false;
}

int main() {
  srand((unsigned int)std::time(nullptr));
  for (int i = 0; i < 155; i++) {randarr[i] = std::rand() % 100;}
  for (int i = 0; i < 221; i++) {randarr2[i] = std::rand() % 100;}
  bool playing = true;
  while (playing) {
    ResetGame();
    cout << "You arrive one day to a mysterious new pet store," << endl
      << "it's sign slightly faded despite you never seeing it before." << endl
      << "Walking through the front door you are fascinated by the almost magical inside," << endl
      << "exotic pets and animals are in glass containers and custom enviroments." << endl
      << "The owner is a man wearing a magician's outfit. He speaks eloquently," << endl << endl <<
      "Aha! A new guest, come take a look at my animals! One of them can be yours, you don't have to pay either!" << endl <<
      "ah... but if something happens to one of my precious animals... Well I'm sure that won't happen!" << endl <<
      "Choose a creature, the higher the star the higher the difficulty of care!'" << endl << endl <<
      "choose an option" << endl <<
      "1 - Look at the pets list" << endl <<
      "2 - leave the store because you can't handle responsibility";
    scenes instance;
    instance.Introduction();
    if (!PlayerLeftStore) {
      instance.S2();
      cout << endl << endl << "CONGRATS, you now have a new pet." << endl << endl << "You arrive home and greet your pet. " << endl
        << endl << "You currently have $" << player.money << endl << "Pet Food stock: ";
      PetFoodReport();
      cout << endl << "Water: " << player.water << endl << "Energy Drinks: " << player.EnergyDrinks << endl << endl;
      instance.Home();
    }
    if (!instance.restart()) {
      playing = false;
      cout << endl << "See you next time!";
    }
  }
}
