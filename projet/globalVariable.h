#ifndef GLOBALVARIABLE_H_INCLUDED
#define GLOBALVARIABLE_H_INCLUDED
int boutonMilieu = 17;
int boutonGauche = 18;
int boutonDroite = 4;
int onglet = 1;
bool doitFermer = false;


bool choix_monstre = false;
bool placement_fleche = false;
bool statOk = true;

// On assossie le numéro de nos pins connecter a nos bouton aux noms de nos boutons


//Mes variable qui vont me permttre de calculer le temps qui passe
int minuteEnCicle = 67;
float tempsTamaFinBoucle;
int secondesTama;
int minutesTama;
float tempsDebutTama;
float finBoucleapresSauvegarde;
int debutBoucle;

int positionXfleche = 4;  //position de la fleche pour la bouger au menu

unsigned char fermer[] PROGMEM = {
  0x1f, 0x00, 0x20, 0x80, 0x40, 0x40, 0x91, 0x20, 0x8a, 0x20, 0x84, 0x20, 0x8a, 0x20, 0x91, 0x20,
  0x40, 0x40, 0x20, 0x80, 0x1f, 0x00
};
// 'fermer', 11x11px
unsigned char fermerSelect[] PROGMEM = {
  0x00, 0x00, 0x1f, 0x00, 0x3f, 0x80, 0x6e, 0xc0, 0x75, 0xc0, 0x7b, 0xc0, 0x75, 0xc0, 0x6e, 0xc0,
  0x3f, 0x80, 0x1f, 0x00, 0x00, 0x00

};



#endif