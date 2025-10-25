#ifndef MONSTRE_H_INCLUDED
#define MONSTRE_H_INCLUDED
#include "imagesChambre.h"
typedef struct Monstre Monstre;
struct Monstre {
  int hunger = 0;

  int fatigue = 0;

  int fun = 100;

  int health = 100;

  int proprete = 100;

  int affection = 100;

  char *nom = "Demon";

  int age = 0;

  char *type;

  int attaque = 35;

  int hauteur = 40;
  int largeur = 44;
  unsigned char *image = testMonstre1;
  int loot = 30;
  int gold = 0;
};




#endif
