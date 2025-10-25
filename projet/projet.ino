#include <Preferences.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "magasin.h"
#include "jeuPomme.h"
#include "acceuilJeux.h"
#include "jeuAventure.h"
#include "globalVariable.h"
#include "Monstre.h"
#include "imagesChambre.h"
#include "imagesCuisine.h"
#include "ressources.h"
#include "dormirFichier.h"
#include "soigner.h"
#include "laver.h"

Monstre truc;
Monstre monstre;
Monstre leFantome;
Monstre dryade;
Preferences preferences;

Adafruit_SSD1306 oled(128, 64, &Wire, -1);

void setup() {
  // Configuration initiale du programme. Cette fonction est exécutée une fois au démarrage.
  Serial.begin(9600);  // Initialise la communication série à 9600 baud

  pinMode(boutonMilieu, INPUT_PULLUP);  // Configure le bouton milieu comme une entrée avec résistance pull-up interne
  pinMode(boutonGauche, INPUT_PULLUP);  // Configure le bouton gauche
  pinMode(boutonDroite, INPUT_PULLUP);  // Configure le bouton droit

  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialise l'écran OLED avec l'adresse I2C 0x3C
  preferences.begin("my-app", false);      // Commence la gestion des préférences sous le nom "my-app"

  oled.setTextColor(WHITE);

  // Initialisation des images et des attaques pour les monstres du mini jeu
  dryade.image = champi;
  leFantome.image = fantome;
  dryade.attaque = 20;
  leFantome.attaque = 15;

  // Position de ligne pour les choix de selection
  posLigneChoixAvant = 3;
  posLigneChoixArriere = 43;

  imageVente = burger;  // Initialisation de l'image de vente

  debutBoucle = 1;

  delay(1000);  // Pause d'une seconde

  oled.clearDisplay();  // Efface l'écran OLED
  oled.display();       // Met à jour l'affichage de l'écran OLED
}

void loop() {
  // Boucle principale du programme, exécutée en continu

  // Gestion du temps et récupération du temps écoulé depuis l'arrêt de l'appareil
  if (debutBoucle == 1) {
    debutBoucle = 2;
    recuperation();  // Récupération des données sauvegardées
    if (tempsTamaFinBoucle > 1000) {
      recuperationStat();  // Récupération des statistiques si le temps écoulé est supérieur à 1s
    }
  }

  // Gestion du temps du Tamagotchi
  if (secondesTama >= 60) {
    tempsDebutTama = finBoucleapresSauvegarde + millis();  // Réinitialisation du temps de début
    minutesTama += 1;                                      // Incrémentation des minutes
  }
  secondesTama = (tempsTamaFinBoucle - tempsDebutTama) / 1000;  // Calcul des secondes écoulées

  menu();  // Affichage et gestion du menu principal

  // Mise à jour du temps en fin de boucle
  tempsTamaFinBoucle = finBoucleapresSauvegarde + millis();
  finCicle();  // Mise à jour des besoins à la fin du cycle

  mettreStatsOk();  // Ajustement des statistiques pour qu'elles restent dans les limites acceptables

  oled.display();  // Met à jour l'affichage de l'écran OLED

  // Sauvegarde des données importantes pour les récupérer lors du redémarrage
  sauvegarde();
}


void recuperation() {
  tempsTamaFinBoucle = preferences.getFloat("tempsFin", 0);
  finBoucleapresSauvegarde = tempsTamaFinBoucle;
  secondesTama = preferences.getInt("sec", 0);
  minutesTama = preferences.getInt("min", 0);
  tempsDebutTama = preferences.getFloat("debut", 0);
  nbPotionVie = preferences.getInt("pv", 0);
  nbPotionAtk = preferences.getInt("pa", 0);
  nbPommeGarde = preferences.getInt("pom", 0);
  nbBrocoli = preferences.getInt("bro", 0);
  nbBurger = preferences.getInt("bur", 0);
  nbGateau = preferences.getInt("gat", 0);
  nbCafe = preferences.getInt("caf", 0);
  nbCocktail = preferences.getInt("coc", 0);
}

void sauvegarde() {
  preferences.putInt("sec", secondesTama);
  preferences.putInt("min", minutesTama);
  preferences.putFloat("tempsFin", tempsTamaFinBoucle);
  preferences.putFloat("debut", tempsDebutTama);
  preferences.putInt("pv", nbPotionVie);
  preferences.putInt("pa", nbPotionAtk);
  preferences.putInt("bro", nbPommeGarde);
  preferences.putInt("bur", nbBurger);
  preferences.putInt("gat", nbGateau);
  preferences.putInt("caf", nbCafe);
  preferences.putInt("coc", nbCocktail);
  preferences.putInt("aff", truc.affection);
  preferences.putInt("age", truc.age);
  preferences.putInt("fat", truc.fatigue);
  preferences.putInt("fun", truc.fun);
  preferences.putInt("hun", truc.hunger);
  preferences.putInt("pro", truc.proprete);
  preferences.putInt("hea", truc.health);
  preferences.putInt("gol", truc.gold);
}
void recuperationStat() {
  truc.affection = preferences.getInt("aff", 0);
  truc.age = preferences.getInt("age", 0);
  truc.fatigue = preferences.getInt("fat", 0);
  truc.fun = preferences.getInt("fun", 0);
  truc.hunger = preferences.getInt("hun", 0);
  truc.proprete = preferences.getInt("pro", 0);
  truc.health = preferences.getInt("hea", 0);
  truc.gold = preferences.getInt("gol", 0);
}

void menu() {
  // Efface l'écran OLED
  oled.clearDisplay();

  // Dessine le fond du menu
  oled.drawBitmap(0, 15, fondMenu, 128, 49, WHITE);

  // Dessine l'image du monstre 'truc' sur l'écran OLED
  oled.drawBitmap(42, 20, truc.image, truc.largeur, truc.hauteur, WHITE);

  // Dessine la barre de sélection en haut de l'écran
  oled.drawBitmap(0, 0, choixBarre, 128, 15, WHITE);

  // Dessine l'icône de l'humeur actuelle du monstre
  oled.drawBitmap(25, 1, choixHumeur, 15, 14, WHITE);

  // Vérifie si le bouton gauche est pressé
  if (digitalRead(boutonGauche) == 0) {
    // Change l'onglet sélectionné vers la gauche
    if (onglet == 1) {
      onglet = 6;
    } else {
      onglet -= 1;
    }
    delay(150);  // Petit délai pour éviter les rebonds
  }

  // Vérifie si le bouton droit est pressé
  if (digitalRead(boutonDroite) == 0) {
    // Change l'onglet sélectionné vers la droite
    if (onglet == 6) {
      onglet = 1;
    } else {
      onglet += 1;
    }
    delay(150);  // Petit délai pour éviter les rebonds
  }

  // Sélectionne et affiche les icônes et options correspondantes à chaque onglet
  if (onglet == 1) {
    choixBarre = barreProfil;
    oled.drawBitmap(105, 1, soin, 15, 14, WHITE);
    if (digitalRead(boutonMilieu) == 0) {
      delay(300);
      onglet = 1;
      fenetreStat();  // Affiche la fenêtre des statistiques
      onglet = 1;
    }
  }

  if (onglet == 2) {
    choixBarre = barreDouche;
    oled.drawBitmap(105, 1, soin, 15, 14, WHITE);
    if (digitalRead(boutonMilieu) == 0) {
      laver();  // Lance la fonction pour laver le monstre
    }
  }

  if (onglet == 3) {
    choixBarre = barreSortir;
    oled.drawBitmap(105, 1, soin, 15, 14, WHITE);
    if (digitalRead(boutonMilieu) == 0) {
      delay(300);
      onglet = 1;
      devantMaison();  // Affiche l'interface devant la maison
      doitFermer = false;
      onglet = 3;
    }
  }

  if (onglet == 4) {
    choixBarre = barreManger;
    oled.drawBitmap(105, 1, soin, 15, 14, WHITE);
    if (digitalRead(boutonMilieu) == 0) {
      delay(300);
      onglet = 1;
      manger();  // Lance la fonction pour nourrir le monstre
      onglet = 4;
    }
  }

  if (onglet == 5) {
    choixBarre = barreLit;
    oled.drawBitmap(105, 1, soin, 15, 14, WHITE);
    if (digitalRead(boutonMilieu) == 0) {
      dormir();  // Lance la fonction pour faire dormir le monstre
    }
  }

  if (onglet == 6) {
    choixBarre = barreTache;
    oled.drawBitmap(105, 1, soinSelect, 15, 14, WHITE);
    if (digitalRead(boutonMilieu) == 0) {
      soigner();  // Lance la fonction pour soigner le monstre
    }
  }

  // Met à jour l'icône de l'humeur en fonction de l'affection du monstre
  if (truc.affection <= 25) {
    choixHumeur = colere;
  } else if (truc.affection <= 50) {
    choixHumeur = triste;
  } else if (truc.affection <= 75) {
    choixHumeur = bof;
  } else {
    choixHumeur = joie;
  }

  // Affiche les mises à jour sur l'écran OLED
  oled.display();
}

void mettreStatsOk() {

  // Vérifie et ajuste la fatigue
  if (truc.fatigue > 100) {
    truc.fatigue = 100;  // Limite la fatigue à 100 si elle dépasse
  }
  if (truc.fatigue < 0) {
    truc.fatigue = 0;  // Empêche la fatigue d'être inférieure à 0
  }

  // Vérifie et ajuste la faim
  if (truc.hunger > 100) {
    truc.hunger = 100;
  }
  if (truc.hunger < 0) {
    truc.hunger = 0;
  }

  // Vérifie et ajuste l'amusement
  if (truc.fun > 100) {
    truc.fun = 100;
  }
  if (truc.fun < 0) {
    truc.fun = 0;
  }

  // Vérifie et ajuste l'affection
  if (truc.affection > 100) {
    truc.affection = 100;
  }
  if (truc.affection < 0) {
    truc.affection = 0;
  }

  // Vérifie et ajuste la vie
  if (truc.health > 100) {
    truc.health = 100;
  }
  if (truc.health < 0) {
    truc.health = 0;
  }

  // Vérifie et ajuste la propreté
  if (truc.proprete > 100) {
    truc.proprete = 100;
  }
  if (truc.proprete < 0) {
    truc.proprete = 0;
  }
  if (truc.health <= 0) {
    mourrir();
  }
}

void finCicle() {
  // Vérifie si le nombre de minutes est pair
  if ((minutesTama % 2) == 0) {
    // Vérifie si la minute en cours est différente du nombre de minutes écoulées depuis le démarrage du jeu
    if (minuteEnCicle != minutesTama) {
      // Vérifie les conditions de santé liées à la fatigue, la faim et la propreté
      if (truc.fatigue == 100 || truc.hunger == 100 || truc.proprete == 0) {
        truc.health -= 10;
      }
      // Réduit l'affection si la propreté est inférieure à 50
      if (truc.proprete < 50) {
        truc.affection -= 10;
      }
      // Met à jour la minute en cours
      minuteEnCicle = minutesTama;
      // Modifie les statistiques du monstre
      truc.hunger += 10;
      truc.fatigue += 10;
      truc.fun -= 15;
      truc.proprete -= 10;
      truc.affection -= 15;
    }
  }
  // if (minutesTama % 59) == 0)
  // {
  //     if (minuteEnCicle != minutesTama) {
  //       truc.age += 1;
  //       minuteEnCicle = minutesTama;
  //     }
  //   }
}

// Fin fonctions écran principale//

// Fonctions fenêtre stats //
void fenetreStat() {
  do {
    oled.clearDisplay();  // Efface l'écran OLED

    // Gestion de la navigation entre les pages avec les boutons gauche et droite
    if (digitalRead(boutonDroite) == 0) {
      delay(150);
      if (onglet == 3) {
        onglet = 1;  // Si onglet est à 3, revenir à 1
      } else {
        onglet += 1;  // Sinon passer à l'onglet suivant
      }
    }
    if (digitalRead(boutonGauche) == 0) {
      delay(150);
      if (onglet == 1) {
        onglet = 3;  // Si onglet est à 1, passer à 3
      } else {
        onglet -= 1;  // Sinon revenir à l'onglet précédent
      }
    }

    // Affichage des informations sur les deux premières pages
    if (onglet == 1 || onglet == 2) {
      oled.setCursor(4, 4);
      oled.println(truc.nom);  // Affiche le nom du monstre

      oled.setCursor(85, 4);
      oled.print(truc.age);
      oled.println(" ans");  // Affiche l'âge du monstre

      oled.drawLine(0, 15, 128, 15, WHITE);  // Dessine une ligne horizontale pour séparer

      // Première page : Affichage de la vie, de l'affection et de la faim
      if (onglet == 1) {
        oled.setCursor(0, 20);
        oled.print("vie");
        oled.drawRect(62, 22, (int)map(truc.health, 0, 100, 0, 46), 2, WHITE);  // Barre de vie

        oled.drawRect(60, 20, 50, 6, WHITE);  // Cadre pour la barre de vie

        oled.setCursor(0, 36);
        oled.print("affection");
        oled.drawRect(62, 38, (int)map(truc.affection, 0, 100, 0, 46), 2, WHITE);  // Barre d'affection
        oled.drawRect(60, 36, 50, 6, WHITE);                                       // Cadre pour la barre d'affection

        oled.setCursor(0, 52);
        oled.print("faim");
        oled.drawRect(62, 54, (int)map(truc.hunger, 0, 100, 0, 46), 2, WHITE);  // Barre de faim
        oled.drawRect(60, 52, 50, 6, WHITE);                                    // Cadre pour la barre de faim
      }

      // Deuxième page : Affichage de l'amusement, de la fatigue et de la propreté
      if (onglet == 2) {
        oled.setCursor(0, 20);
        oled.print("fun");
        oled.drawRect(62, 22, (int)map(truc.fun, 0, 100, 0, 46), 2, WHITE);  // Barre de fun
        oled.drawRect(60, 20, 50, 6, WHITE);                                 // Cadre pour la barre de fun

        oled.setCursor(0, 36);
        oled.print("fatigue");
        oled.drawRect(62, 38, (int)map(truc.fatigue, 0, 100, 0, 46), 2, WHITE);  // Barre de fatigue
        oled.drawRect(60, 36, 50, 6, WHITE);                                     // Cadre pour la barre de fatigue

        oled.setCursor(0, 52);
        oled.print("proprete");
        oled.drawRect(62, 54, (int)map(truc.proprete, 0, 100, 0, 46), 2, WHITE);  // Barre de propreté
        oled.drawRect(60, 52, 50, 6, WHITE);                                      // Cadre pour la barre de propreté
      }
    }

    // Troisième page : Affichage du temps de jeu
    if (onglet == 3) {
      oled.setCursor(4, 4);
      oled.println("temps en jeu : ");
      oled.setCursor(0, 20);
      oled.print((int)minutesTama / 60);  // Heures jouées
      oled.println(" heures ");

      oled.setCursor(0, 36);
      oled.print(minutesTama % 60);  // Minutes jouées
      oled.println(" minutes ");

      oled.setCursor(0, 52);
      oled.print((int)secondesTama);  // Secondes jouées
      oled.println(" secondes ");

      oled.drawLine(0, 15, 128, 15, WHITE);  // Ligne horizontale pour séparer
    }

    oled.display();                          // Met à jour l'affichage sur l'écran OLED
  } while (digitalRead(boutonMilieu) == 1);  // Continue tant que le bouton milieu est pressé
  delay(300);                                // Délai après la sortie de la boucle pour éviter les rebonds de bouton
}


void devantMaison() {
  do {
    oled.clearDisplay();

    // Gestion de la navigation entre les pages avec les boutons gauche et droite
    if (digitalRead(boutonGauche) == 0 && onglet > 1) {
      onglet -= 1;
      delay(150);
    }
    if (digitalRead(boutonDroite) == 0 && onglet < 4) {
      onglet += 1;
      delay(150);
    }

    // Gestion de chaque onglet

    if (onglet == 1) {
      // Affichage pour le choix d'aller dans le jardin et lancé un jeu
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(0, 16, acceuilJardin, 128, 49, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        chercherPomme();  // Appel de la fonction chercherPomme
        if (nbPomme == 15) {
          pommesRecuperees();  // Appel de la fonction chercherPomme qui gère les récompense une fois le jeu fini
        }
        onglet = 1;
        doitFermer = false;
      }
    }
    if (onglet == 2) {
      // Affichage pour le choix d'aller dans la forêt et lancé un jeu
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(0, 16, acceuilForet, 128, 49, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        onglet = 1;
        aventure();
        truc.attaque = 35;
        choixTypeMonstre = false;
        onglet = 1;
        doitFermer = false;
      }
    }
    if (onglet == 3) {
      // Affichage pour le choix d'aller en ville et aller au magasin
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(0, 16, acceuilVille, 128, 49, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        onglet = 1;
        magasin();
        onglet = 1;
        doitFermer = false;
      }
    }
    if (onglet == 4) {
      // Affichage pour le choix de retourner au menu
      oled.drawBitmap(116, 2, fermerSelect, 11, 11, WHITE);
      oled.drawBitmap(0, 16, acceuil, 128, 49, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        doitFermer = true;
        delay(300);
      }
    }

    if (mort == true) {
      doitFermer = true;
    }
    oled.display();
  } while (doitFermer == false);
}

void aventure() {
  posLigneChoixAvant = 3;
  posLigneChoixArriere = 43;

  do {
    // Si le choix du type de monstre n'a pas encore été fait
    if (choixTypeMonstre == false) {
      typeMonstre = random(1, 3);  // Sélection aléatoire du type de monstre

      // Attribution du monstre en fonction du type choisi
      if (typeMonstre == 1) {
        monstre = leFantome;
      }
      if (typeMonstre == 2) {
        monstre = dryade;
      }
      choixTypeMonstre = true;  // Le choix du monstre est maintenant fait
    }

    oled.clearDisplay();

    // Affichage des options pour quitter ou combattre
    oled.setCursor(3, 0);
    oled.print("quitter");
    oled.setCursor(73, 0);
    oled.print("combattre");

    // Dessine la ligne de sélection entre les options
    oled.drawLine(posLigneChoixAvant, 10, posLigneChoixArriere, 10, WHITE);

    // Affiche l'image du monstre
    oled.drawBitmap(100, 64 - 19, monstre.image, 25, 19, WHITE);

    // Dessine une ligne de séparation en haut de l'écran
    oled.drawLine(0, 15, 128, 15, WHITE);

    // Affiche le fond de la forêt
    oled.drawBitmap(0, 16, fondForet, 128, 49, WHITE);

    // Gestion des actions lorsque les boutons sont pressés
    if (digitalRead(boutonGauche) == 0) {
      delay(150);
      onglet = 1;  // choix de l'onglet pour revenir au menu principal
    }
    if (digitalRead(boutonDroite) == 0) {
      delay(150);
      onglet = 2;  // choix de l'onglet pour aller à l'écran de combat
    }

    // Action si l'option "quitter" est sélectionnée
    if (onglet == 1) {
      posLigneChoixAvant = 3;
      posLigneChoixArriere = 43;

      // Action si le bouton du milieu est pressé
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        onglet = 1;         // Revenir au menu principal
        doitFermer = true;  // Fermer la fenêtre d'aventure
      }
    }

    // Action si l'option "combattre" est sélectionnée
    if (onglet == 2) {
      posLigneChoixAvant = 73;
      posLigneChoixArriere = 127;

      // Action si le bouton du milieu est pressé
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        onglet = 1;
        combat();       // Fonction pour commencer le combat
        recompenses();  // Fonction pour distribuer les récompenses après le combat
        onglet = 2;
        delay(300);
        doitFermer = false;  // Ne pas fermer automatiquement la fenêtre d'aventure
        enCombat = true;     // un nouveau combat pourra être lancé
        tour = true;         // lorsqu'un combat recommencera, le joueur commencera a jouer
      }
    }

    if (mort == true) {
      doitFermer = true;
    }

    oled.display();
  } while (doitFermer == false);

  delay(300);
}


void combat() {
  oled.clearDisplay();

  while (enCombat == true) {
    oled.clearDisplay();

    // Affichage de la barre de santé du monstre et de son image
    oled.drawRect(92, 36, map(monstre.health, 0, 100, 0, 32), 2, WHITE);
    oled.drawBitmap(100, 64 - 19, monstre.image, 25, 19, WHITE);

    // Affichage du fond de l'écran de combat
    oled.drawBitmap(0, 16, choix, 128, 49, WHITE);

    // Affichage des informations du joueur (vie et attaque)
    oled.setCursor(0, 0);
    oled.print("vie :  ");
    oled.print(truc.health);
    oled.print(" | atk : ");
    oled.print(truc.attaque);


    // Tour du monstre
    if (tour == false) {
      oled.clearDisplay();

      chance = random(1, 11);  // Génération d'un nombre aléatoire pour déterminer si le monstre loupe son attaque
      oled.drawBitmap(100, 64 - 19, monstre.image, 25, 19, WHITE);
      oled.drawBitmap(0, 16, choix, 128, 49, WHITE);  //affiche le choix entre fuir attaquer et prendre potion en couleur inversée
      oled.drawRect(92, 36, map(monstre.health, 0, 100, 0, 32), 2, WHITE);
      oled.setCursor(0, 0);

      // Si le monstre rate son attaque
      if (chance == 10) {
        oled.println("Loupe");
      }

      else {
        truc.health = truc.health - monstre.attaque;  // Réduit la santé du joueur de l'attaque du monstre
        oled.print(monstre.attaque);
        oled.println(" degats recus");
      }

      oled.display();
      delay(1000);  // Délai pour lire les informations

      // Si la santé du joueur atteint 0 ou moins, le combat s'arrête
      if (truc.health <= 0) {
        enCombat = false;
        mort = true;
      }

      tour = true;  // Passe au tour du joueur
    }


    // Tour du joueur
    if (tour == true && enCombat == true) {

      // Change l'onglet sélectionné vers le bas quand le bouton de droite est pressé
      if (digitalRead(boutonDroite) == 0 && onglet < 3) {
        onglet += 1;
        delay(150);
      }

      // Change l'onglet sélectionné vers le haut quand le bouton de gauche est pressé
      if (digitalRead(boutonGauche) == 0 && onglet > 1) {
        onglet -= 1;
        delay(150);
      }

      // Attaque sélectionnée
      if (onglet == 1) {
        choix = choixAttaque;

        // Si le bouton du milieu est pressé
        if (digitalRead(boutonMilieu) == 0) {
          oled.clearDisplay();
          oled.drawBitmap(100, 64 - 19, monstre.image, 25, 19, WHITE);
          oled.drawBitmap(0, 16, choix, 128, 49, WHITE);
          oled.drawRect(92, 36, map(monstre.health, 0, 100, 0, 32), 2, WHITE);
          monstre.health = monstre.health - truc.attaque;  // Réduit la santé du monstre de l'attaque du joueur
          oled.setCursor(0, 0);
          oled.print(truc.attaque);
          oled.println(" degats infliges");
          oled.display();  // Met à jour l'affichage
          delay(1000);     // Délai pour lire les informations

          // Si le monstre n'a plus de santé, le joueur gagne le combat
          if (monstre.health <= 0) {
            enCombat = false;
            gagner = true;
            monstre.health = 100;  // Réinitialise la santé du monstre
          }
          tour = false;  // Passe au tour du monstre
        }
      }

      // Utilisation de potions
      if (onglet == 2) {
        choix = choixPotions;


        if (digitalRead(boutonMilieu) == 0) {
          delay(300);
          onglet = 1;
          prendrePotion();  // Fonction pour utiliser une potion
          if (choixPotion != choixRetour) {
            tour = false;  // Passe au tour du monstre si une potion est utilisée
          }
        }
      }

      // Fuite du combat
      if (onglet == 3) {
        choix = choixFuir;

        if (digitalRead(boutonMilieu) == 0) {
          enCombat = false;  // Met fin au combat
          onglet = 1;
          gagner = false;  // Le joueur perd le combat et n'aura pas de récompense
        }
      }

      oled.display();
    }
  }
}


void prendrePotion() {
  while (quitterMenuPotion == false) {
    oled.clearDisplay();  // Efface l'écran OLED à chaque itération de la boucle

    // Affichage du nombre de potions d'attaque et de vie
    oled.setCursor(54, 29);
    oled.print(nbPotionAtk);
    oled.setCursor(54, 40);
    oled.print(nbPotionVie);

    // Affichage de l'image du monstre et de sa barre de santé
    oled.drawBitmap(100, 64 - 19, monstre.image, 25, 19, WHITE);
    oled.drawRect(92, 36, map(monstre.health, 0, 100, 0, 32), 2, WHITE);

    // Affichage du fond de l'écran de sélection des potions
    oled.drawBitmap(0, 16, choixPotion, 128, 49, WHITE);

    // Sélection de l'option de potion avec les boutons gauche et droite
    if (digitalRead(boutonDroite) == 0 && onglet < 3) {
      onglet += 1;
      delay(150);
    }
    if (digitalRead(boutonGauche) == 0 && onglet > 1) {
      onglet -= 1;
      delay(150);
    }

    // Utilisation de la potion d'attaque
    if (onglet == 1) {
      choixPotion = choixAtk;
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        if (nbPotionAtk > 0) {
          truc.attaque += 10;  // Augmente l'attaque du joueur de 10 points
          oled.setCursor(3, 3);
          oled.print("+ 10 pts ATK");
          nbPotionAtk -= 1;          // Diminue le nombre de potions d'attaque disponibles
          quitterMenuPotion = true;  // Quitte le menu des potions
          oled.display();
          delay(1000);
        } else {
          oled.setCursor(3, 3);
          oled.print("Pas de potion d'Attaque");
          oled.display();
          delay(1000);
        }
      }
    }

    // Utilisation de la potion de vie
    if (onglet == 2) {
      choixPotion = choixVie;
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        if (nbPotionVie > 0) {
          if (truc.health == 100) {
            oled.setCursor(3, 3);
            oled.print("Votre vie est déjà complète !");
            oled.display();
            delay(1000);
          } else {
            truc.health += 60;  // Augmente la santé du joueur de 60 points
            oled.setCursor(3, 3);
            oled.print("+ 60 pts de Vie");
            nbPotionVie -= 1;          // Diminue le nombre de potions de vie disponibles
            quitterMenuPotion = true;  // Quitte le menu des potions
            if (truc.health > 100) {
              truc.health = 100;  // Limite la santé à 100 points maximum
            }
            oled.display();
            delay(1000);
          }
        } else {
          oled.setCursor(3, 3);
          oled.print("Pas de potion de Vie");
          oled.display();
          delay(1000);
        }
      }
    }

    // Retour au menu précédent
    if (onglet == 3) {
      choixPotion = choixRetour;
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        quitterMenuPotion = true;  // Quitte le menu des potions
      }
    }

    oled.display();  // Met à jour l'affichage à chaque itération de la boucle
  }

  quitterMenuPotion = false;  // Réinitialise la variable de sortie du menu
  onglet = 1;
  delay(300);
}


void recompenses() {
  if (truc.health > 0) {
    chance = random(0, 3);

    // Ajout d'une potion de vie si la chance est égale à 1
    if (chance == 1) {
      nbPotionVie += 1;
    }

    // Ajout d'une potion d'attaque si la chance est égale à 2
    if (chance == 2) {
      nbPotionAtk += 1;
    }

    // Ajout du butin (gold) au joueur
    truc.gold += monstre.loot;

    // Augmentation des attributs du joueur
    truc.fun += 20;
    truc.fatigue += 15;
    truc.affection += 30;
    truc.proprete -= 20;
    truc.hunger += 15;

    // Affichage des récompenses sur l'écran OLED
    oled.clearDisplay();
    while (digitalRead(boutonMilieu) == 1) {
      oled.setCursor(0, 0);
      oled.print("Recuperation du butin");
      oled.setCursor(0, 30);
      oled.print("+ ");
      oled.print(monstre.loot);
      oled.print(" gold");

      // Affichage de la potion obtenue selon la chance
      oled.setCursor(0, 40);
      if (chance == 1) {
        oled.print("+ Potion de vie");
      } else if (chance == 2) {
        oled.print("+ Potion d'attaque");
      }

      // Affichage du bouton "quitter" en bas à droite
      oled.setCursor(85, 55);
      oled.print("Quitter");

      oled.display();
    }
  }
}


void chercherPomme() {
  do {
    // Affichage du nombre de pommes récupérées sur l'écran OLED
    oled.setCursor(0, 0);
    oled.print(nbPomme);
    oled.println("/15 Pommes");

    // Dessin de l'interface et des éléments visuels sur l'écran OLED
    oled.drawLine(0, 15, 128, 15, WHITE);
    oled.drawBitmap(0, 16, fond, 128, 49, WHITE);
    oled.drawBitmap(posPanier, 64 - 16, panier, 23, 9, WHITE);
    oled.drawBitmap(posXpomme, posYpomme, pomme, 11, 11, WHITE);
    oled.drawBitmap(posXpomme2, posYpomme2, pomme, 11, 11, WHITE);

    // Déplacement du panier à gauche si bouton gauche pressé
    if (digitalRead(boutonGauche) == 0) {
      posPanier = 30;
    }

    // Déplacement du panier à droite si bouton droite pressé
    if (digitalRead(boutonDroite) == 0) {
      posPanier = 76;
    }

    // Pause du jeu si bouton central pressé
    if (digitalRead(boutonMilieu) == 0) {
      fairePause = true;
      delay(300);
      pauseJeu();
      fairePause = false;
    }

    // Déplacement aléatoire de la première pomme et gestion de la récolte
    if (posYpomme < 64 - 16) {
      if (posYpomme == -20) {
        posYpomme = 30;
      } else {
        posYpomme += 1;
      }
    } else {
      posYpomme = random(-100, -20);
    }

    // Déplacement aléatoire de la deuxième pomme et gestion de la récolte
    if (posYpomme2 < 64 - 16) {
      if (posYpomme2 == -20) {
        posYpomme2 = 30;
      } else {
        posYpomme2 += 1;
      }
    } else {
      posYpomme2 = random(-100, -20);
    }

    // Vérification et gestion de la récolte des pommes en fonction de leur position et du panier
    if ((posYpomme == 64 - 16 && posPanier == 30) || (posYpomme2 == 64 - 16 && posPanier == 76)) {
      nbPomme += 1;
    }

    // Vérification et gestion du retrait des pommes si elles sont déposées dans le panier opposé
    if ((posYpomme == 64 - 16 && posPanier == 76) || (posYpomme2 == 64 - 16 && posPanier == 30)) {
      if (nbPomme > 0) {
        nbPomme -= 1;
      }
    }

    // Condition de fermeture de la boucle si 15 pommes ont été collectées
    if (nbPomme == 15) {
      doitFermer = true;
    }

    oled.display();
    oled.clearDisplay();
  } while (doitFermer == false);

  doitFermer = false;  // Réinitialisation de la variable de fermeture
  reprendre = false;   // Réinitialisation de la variable de reprise
}

void pauseJeu() {
  onglet = 1;
  posLigneChoixAvant = 3;
  posLigneChoixArriere = 43;

  do {
    oled.clearDisplay();
    // Affichage de l'écran de pause avec options "Reprendre" et "Quitter"
    oled.setCursor(50, 4);
    oled.print("PAUSE");
    oled.setCursor(3, 50);
    oled.print("Reprendre");
    oled.setCursor(85, 50);
    oled.print("Quitter");
    oled.drawLine(posLigneChoixAvant, 60, posLigneChoixArriere, 60, WHITE);

    // Gestion des boutons gauche et droit pour la sélection des options
    if (digitalRead(boutonGauche) == 0) {
      onglet = 1;
    }
    if (digitalRead(boutonDroite) == 0) {
      onglet = 2;
    }

    // Option "Reprendre"
    if (onglet == 1) {
      posLigneChoixAvant = 3;
      posLigneChoixArriere = 55;
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        reprendre = true;
        fairePause = false;
      }
    }

    // Option "Quitter"
    if (onglet == 2) {
      posLigneChoixAvant = 85;
      posLigneChoixArriere = 126;
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        doitFermer = true;
        fairePause = false;
      }
    }

    oled.display();
  } while (fairePause == true);  // Boucle jusqu'à ce que fairePause soit false

  onglet = 1;
}

void pommesRecuperees() {
  pommesVendues = random(0, 16);      // Nombre de pommes vendues au hasard
  prix = 1;                           // Prix fixe par pomme
  nbPomme -= pommesVendues;           // Mise à jour du nombre de pommes restantes
  truc.gold += prix * pommesVendues;  // Ajout du gain en or
  truc.fun += 15;                     // Augmentation du plaisir
  truc.fatigue += 7;                  // Augmentation de la fatigue
  truc.affection += 15;               // Augmentation de l'affection
  truc.proprete -= 10;                // Diminution de la propreté
  truc.hunger += 5;                   // Augmentation de la faim

  // Affichage des résultats et attente de l'appui du bouton du milieu pour continuer
  while (digitalRead(boutonMilieu) == 1) {
    oled.clearDisplay();
    oled.setCursor(3, 0);
    oled.println("Vous avez recuperees toute les pommes !");
    oled.setCursor(3, 20);
    oled.print(pommesVendues);
    oled.println(" pommes vendues.");
    oled.print("+ ");
    oled.print(prix * pommesVendues);
    oled.println(" gold");
    oled.print(nbPomme);
    oled.println(" pomme(s) recuperee(s).");
    oled.setCursor(60, 55);
    oled.print("continuer");
    oled.drawLine(60, 63, 128, 63, WHITE);
    oled.display();  // Met à jour l'affichage OLED
  }

  // Mise à jour du nombre de pommes gardées et réinitialisation du nombre de pommes
  nbPommeGarde += nbPomme;
  nbPomme = 0;
  delay(300);  // Pause pour éviter les appuis multiples accidentels
}

void magasin() {
  posLigneChoixAvant = 3;
  posLigneChoixArriere = 43;
  do {
    oled.clearDisplay();

    // Affichage des options de menu
    oled.setCursor(3, 0);
    oled.print("quitter");
    oled.setCursor(86, 0);
    oled.print("rentrer");
    oled.drawLine(posLigneChoixAvant, 10, posLigneChoixArriere, 10, WHITE);
    oled.drawLine(0, 15, 128, 15, WHITE);
    oled.drawBitmap(0, 16, imageMagasin, 128, 49, WHITE);

    // Gestion des boutons gauche et droite
    if (digitalRead(boutonGauche) == 0) {
      delay(150);
      posLigneChoixAvant = 3;
      posLigneChoixArriere = 43;
      onglet = 1;
    }
    if (digitalRead(boutonDroite) == 0) {
      delay(150);
      posLigneChoixAvant = 85;
      posLigneChoixArriere = 128;
      onglet = 2;
    }

    // Gestion de la sélection avec le bouton milieu
    if (onglet == 1 && digitalRead(boutonMilieu) == 0) {
      delay(300);
      doitFermer = true;  // Quitter le magasin
    }

    if (onglet == 2 && digitalRead(boutonMilieu) == 0) {
      delay(300);
      onglet = 1;
      interieur();  // Entrer dans le magasin
      onglet = 2;
      doitFermer = false;
    }

    oled.display();  // Mise à jour de l'affichage OLED
  } while (doitFermer == false);
  delay(300);  // Pause pour éviter les appuis multiples accidentels
}

void interieur() {
  do {
    oled.clearDisplay();

    // Gestion des boutons gauche et droite pour naviguer dans les onglets
    if (digitalRead(boutonGauche) == 0 && onglet > 1) {
      onglet -= 1;
      delay(150);
    }
    if (digitalRead(boutonDroite) == 0 && onglet < 3) {
      onglet += 1;
      delay(150);
    }

    // Affichage et actions pour chaque onglet
    if (onglet == 1) {
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(0, 16, choixNourriture, 128, 49, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        achatFood();  // Acheter de la nourriture
        onglet = 1;
        doitFermer = false;
      }
    }

    if (onglet == 2) {
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(0, 16, choixAchatPotions, 128, 49, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        delay(300);
        achatPotions();  // Acheter des potions
        onglet = 1;
        doitFermer = false;
      }
    }

    if (onglet == 3) {
      oled.drawBitmap(116, 2, fermerSelect, 11, 11, WHITE);
      oled.drawBitmap(0, 16, choixAchat, 128, 49, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        doitFermer = true;  // Quitter le menu intérieur du magasin
        delay(300);
      }
    }

    oled.display();  // Mise à jour de l'affichage OLED
  } while (doitFermer == false);
}


void achatFood() {
  do {
    // Effacer l'affichage OLED
    oled.clearDisplay();

    // Dessiner une ligne horizontale sur l'affichage OLED
    oled.drawLine(0, 15, 128, 15, WHITE);

    // Gestion des boutons pour naviguer dans les onglets
    if (digitalRead(boutonGauche) == 0) {
      if (onglet == 1) {
        onglet = 4;
      } else {
        onglet -= 1;
      }
      delay(150);
    }
    if (digitalRead(boutonDroite) == 0) {
      if (onglet == 4) {
        onglet = 1;
      } else {
        onglet += 1;
      }
      delay(150);
    }

    // Gestion de l'onglet 2 : deffilement vers la droite, changement du produit afficher
    if (onglet == 2) {
      oled.setCursor(3, 2);
      oled.print("acheter");
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droiteSelect, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gauche, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        if (choixVente == 1) {
          choixVente = 5;
        } else {
          choixVente -= 1;
        }
        delay(150);
      }
    }

    // Gestion de l'onglet 3 : deffilement vers la gauche, changement du produit afficher
    if (onglet == 3) {
      oled.setCursor(3, 2);
      oled.print("acheter");
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droite, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gaucheSelect, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        if (choixVente == 5) {
          choixVente = 1;
        } else {
          choixVente += 1;
        }
        delay(150);
      }
    }

    // Gestion de l'onglet 1 : Achat de l'article sélectionné
    if (onglet == 1) {
      oled.setCursor(3, 2);
      oled.print("acheter");
      oled.drawLine(3, 11, 43, 11, WHITE);
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droiteSelect, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gaucheSelect, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        // Vérifier si le joueur a assez d'argent pour acheter l'article
        if (truc.gold < prix) {
          delay(150);
          oled.setCursor(3, 40);
          oled.print("pas assez de sous");
          oled.display();
          delay(2000);
        } else {
          // Déduire le prix de l'article du solde du joueur
          truc.gold -= prix;
          delay(300);
          // Ajouter l'article acheté à l'inventaire du joueur
          if (choixVente == 1) {
            nbBurger += 1;
          }
          if (choixVente == 2) {
            nbBrocoli += 1;
          }
          if (choixVente == 3) {
            nbGateau += 1;
          }
          if (choixVente == 4) {
            nbCafe += 1;
          }
          if (choixVente == 5) {
            nbCocktail += 1;
          }
        }
      }
    }

    // Gestion de l'onglet 4 : Fermeture du menu d'achat
    if (onglet == 4) {
      oled.setCursor(3, 2);
      oled.print("acheter");
      oled.drawBitmap(116, 2, fermerSelect, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droiteSelect, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gaucheSelect, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        doitFermer = true;
        delay(300);
      }
    }

    // Mise à jour des informations de l'article en fonction du produit affciher
    if (choixVente == 1) {
      imageVente = burger;
      prix = 25;
      nomProduit = "burger";
    }
    if (choixVente == 2) {
      imageVente = broccoli;
      prix = 10;
      nomProduit = "brocoli";
    }
    if (choixVente == 3) {
      imageVente = gateau;
      prix = 30;
      nomProduit = "gateau";
    }
    if (choixVente == 4) {
      imageVente = cafe;
      prix = 5;
      nomProduit = "cafe";
    }
    if (choixVente == 5) {
      imageVente = cocktail;
      prix = 15;
      nomProduit = "cocktail";
    }

    // Affichage des informations de l'article sur l'écran OLED
    oled.setCursor(3, 20);
    oled.print(nomProduit);
    oled.drawBitmap(128 - 46, 18, imageVente, 46, 46, WHITE);
    oled.setCursor(3, 40);
    oled.print("prix:");
    oled.print(prix);
    oled.print(" pcs");
    oled.setCursor(2, 55);
    oled.print("fonds:");
    oled.print(truc.gold);
    oled.print(" pcs");
    oled.display();

  } while (doitFermer == false);

  // Réinitialiser les variables pour la prochaine utilisation
  doitFermer = false;
  onglet = 1;
  choixVente = 1;
}


void achatPotions() {
  do {
    // Effacer l'affichage OLED
    oled.clearDisplay();

    // Dessiner une ligne horizontale sur l'affichage OLED
    oled.drawLine(0, 15, 128, 15, WHITE);

    // Gestion des boutons pour naviguer dans les onglets
    if (digitalRead(boutonGauche) == 0) {
      if (onglet == 1) {
        onglet = 4;
      } else {
        onglet -= 1;
      }
      delay(150);  // Délai pour éviter les répétitions rapides de lecture de bouton
    }
    if (digitalRead(boutonDroite) == 0) {
      if (onglet == 4) {
        onglet = 1;
      } else {
        onglet += 1;
      }
      delay(150);  // Délai pour éviter les répétitions rapides de lecture de bouton
    }

    // Gestion de l'onglet 2 : deffilement vers la droite, changement du produit afficher
    if (onglet == 2) {
      oled.setCursor(3, 2);
      oled.print("acheter");
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droiteSelect, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gauche, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        if (choixVente == 1) {
          choixVente = 2;
        } else {
          choixVente -= 1;
        }
        delay(150);  // Délai pour éviter les répétitions rapides de lecture de bouton
      }
    }

    // Gestion de l'onglet 3 : deffilement vers la gauche, changement du produit afficher
    if (onglet == 3) {
      oled.setCursor(3, 2);
      oled.print("acheter");
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droite, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gaucheSelect, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        if (choixVente == 2) {
          choixVente = 1;
        } else {
          choixVente += 1;
        }
        delay(150);  // Délai pour éviter les rebonds
      }
    }

    // Gestion de l'onglet 1 : Achat de l'article sélectionné
    if (onglet == 1) {
      oled.setCursor(3, 2);
      oled.print("acheter");
      oled.drawLine(3, 11, 43, 11, WHITE);
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droiteSelect, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gaucheSelect, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        // Vérifier si le joueur a assez d'argent pour acheter l'article
        if (truc.gold < prix) {
          delay(150);
          oled.setCursor(3, 40);
          oled.print("pas assez de sous");
          oled.display();
          delay(2000);
        } else {
          // Déduire le prix de l'article du solde du joueur
          truc.gold -= prix;
          delay(300);
          // Ajouter l'article acheté à l'inventaire du joueur
          if (choixVente == 1) {
            nbPotionVie += 1;
          }
          if (choixVente == 2) {
            nbPotionAtk += 1;
          }
        }
      }
    }

    // Gestion de l'onglet 4 : Fermeture du menu d'achat
    if (onglet == 4) {
      oled.setCursor(3, 2);
      oled.print("acheter");
      oled.drawBitmap(116, 2, fermerSelect, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droiteSelect, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gaucheSelect, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        doitFermer = true;
        delay(300);
      }
    }

    // Mise à jour des informations de l'article en fonction du choix de vente
    if (choixVente == 1) {
      imageVente = potionVie;
      prix = 25;
      nomProduit = "potion Vie";
    }
    if (choixVente == 2) {
      imageVente = potionAtk;
      prix = 25;
      nomProduit = "potion Atk";
    }

    // Affichage des informations de l'article sur l'écran OLED
    oled.setCursor(3, 20);
    oled.print(nomProduit);
    oled.drawBitmap(128 - 46, 18, imageVente, 46, 46, WHITE);
    oled.setCursor(3, 40);
    oled.print("prix:");
    oled.print(prix);
    oled.print(" pcs");
    oled.setCursor(2, 55);
    oled.print("fonds:");
    oled.print(truc.gold);
    oled.print(" pcs");
    oled.display();

  } while (doitFermer == false);

  // Réinitialiser les variables pour la prochaine utilisation
  choixVente = 1;
  doitFermer = false;
  onglet = 1;
}

//manger//

void manger() {
  do {

    oled.clearDisplay();

    // Dessine une ligne horizontale sur l'affichage OLED
    oled.drawLine(0, 15, 128, 15, WHITE);

    // Gestion des boutons pour naviguer dans les onglets
    if (digitalRead(boutonGauche) == 0) {
      if (onglet == 1) {
        onglet = 4;
      } else {
        onglet -= 1;
      }
      delay(150);  // Délai pour éviter les répétitions rapides de lecture de bouton
    }
    if (digitalRead(boutonDroite) == 0) {
      if (onglet == 4) {
        onglet = 1;
      } else {
        onglet += 1;
      }
      delay(150);  // Délai pour éviter les répétitions rapides de lecture de bouton
    }

    // Gestion de l'onglet 2 : changement de nourriture
    if (onglet == 2) {
      oled.setCursor(3, 2);
      oled.print("manger");
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droiteSelect, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gauche, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        if (choixMiam == 1) {
          choixMiam = 6;
        } else {
          choixMiam -= 1;
        }
        delay(150);  // Délai pour éviter les répétitions rapides de lecture de bouton
      }
    }

    // Gestion de l'onglet 3 : changement de nourriture
    if (onglet == 3) {
      oled.setCursor(3, 2);
      oled.print("manger");
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droite, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gaucheSelect, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        if (choixMiam == 6) {
          choixMiam = 1;
        } else {
          choixMiam += 1;
        }
        delay(150);  // Délai pour éviter les répétitions rapides de lecture de bouton
      }
    }

    // Gestion de l'onglet 1 : Consommation de nourriture
    if (onglet == 1) {
      oled.setCursor(3, 2);
      oled.print("manger");
      oled.drawLine(3, 11, 35, 11, WHITE);
      oled.drawBitmap(116, 2, fermer, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droiteSelect, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gaucheSelect, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        // Vérifie si le Tamagotchi a suffisamment faim pour manger
        if (truc.hunger <= 0) {
          delay(150);
          oled.setCursor(3, 40);
          oled.print("pas assez faim");
          oled.display();
          delay(2000);
        } else {
          // Vérifie si il y a de la nourriture en réserve
          if (quantite == 0) {
            delay(150);
            oled.setCursor(3, 40);
            oled.print("Pas de reserve");
            oled.display();
            delay(2000);
          } else {
            // Diminue la faim du Tamagotchi et la quantité de nourriture
            truc.hunger -= food;
            quantite -= 1;
            delay(300);
            // Décrémenter la quantité de l'article sélectionné
            if (choixMiam == 1) {
              nbBurger -= 1;
            }
            if (choixMiam == 2) {
              nbBrocoli -= 1;
            }
            if (choixMiam == 3) {
              nbGateau -= 1;
            }
            if (choixMiam == 4) {
              nbCafe -= 1;
            }
            if (choixMiam == 5) {
              nbCocktail -= 1;
            }
            if (choixMiam == 6) {
              nbPommeGarde -= 1;
            }
          }
        }
      }
    }

    // Gestion de l'onglet 4 : Fermeture du menu de nourriture et retour au menu
    if (onglet == 4) {
      oled.setCursor(3, 2);
      oled.print("manger");
      oled.drawBitmap(116, 2, fermerSelect, 11, 11, WHITE);
      oled.drawBitmap(104, 2, droiteSelect, 11, 11, WHITE);
      oled.drawBitmap(90, 2, gaucheSelect, 11, 11, WHITE);
      if (digitalRead(boutonMilieu) == 0) {
        doitFermer = true;
        delay(300);
      }
    }

    // Mise à jour des informations de nourriture en fonction du choix
    if (choixMiam == 1) {
      imageMiam = burgerCuisine;
      food = 60;
      quantite = nbBurger;
    }
    if (choixMiam == 2) {
      imageMiam = brocoliCuisine;
      food = 30;
      quantite = nbBrocoli;
    }
    if (choixMiam == 3) {
      imageMiam = gateauCuisine;
      food = 45;
      quantite = nbGateau;
    }
    if (choixMiam == 4) {
      imageMiam = cafeCuisine;
      food = 15;
      quantite = nbCafe;
    }
    if (choixMiam == 5) {
      imageMiam = cocktailCuisine;
      food = 15;
      quantite = nbCocktail;
    }
    if (choixMiam == 6) {
      imageMiam = pommeCuisine;
      food = 2;
      quantite = nbPommeGarde;
    }

    // Affiche l'image et la quantité de nourriture sur l'écran OLED
    oled.drawBitmap(0, 0, imageMiam, 128, 64, WHITE);
    oled.fillRect(90, 16, 40, 15, BLACK);
    oled.setCursor(50, 3);
    oled.print("x ");
    oled.print(quantite);

    oled.setCursor(91, 18);
    oled.print(food);
    oled.print(" nrt");
    oled.display();

  } while (doitFermer == false);

  // Réinitialise l'état de fermeture pour la prochaine utilisation
  doitFermer = false;
}



void dormir() {
  delay(300);
  tempsAvantDodo = millis();
  secondesDodo = 0;
  minutesDodo = 0;
  do {
    oled.clearDisplay();
    tempsFinDodo = millis();
    secondesDodo = (tempsFinDodo - tempsAvantDodo) / 1000;
    if (secondesDodo >= 60) {
      tempsAvantDodo = millis();
      minutesDodo += 1;
    }
    if (secondesDodo % 5 == 0) {
      if (secondesDodo != secondesDodoEnCours) {
        truc.fatigue -= 1;
        secondesDodoEnCours = secondesDodo;
      }
    }

    if (truc.fatigue <= 0) {
      truc.fatigue = 0;
    }
    oled.setCursor(3, 17);
    oled.print("fatigue : ");
    oled.print(truc.fatigue);
    oled.setCursor(3, 40);
    oled.print(minutesDodo);
    oled.print(" min ");
    oled.print(secondesDodo);
    oled.print(" sec ");
    oled.display();
  } while (digitalRead(boutonMilieu) == 1);
  delay(150);
}

void laver() {
  delay(300);  // Délai initial pour démarrer le processus de soin
  truc.affection += 20;
  truc.proprete = 100;
  // Boucle pour afficher une animation de 9 images
  for (int i = 0; i < 10; i++) {
    delay(60);
    oled.clearDisplay();
    oled.drawBitmap(0, 16, allArray[i], 128, 128, WHITE);  // Affiche l'image de l'animation
    oled.display();
  }
  oled.setCursor(3, 50);
  oled.print("Il est tout propre!");
  oled.display();
  delay(1000);

  oled.display();  // Met à jour l'affichage OLED une dernière fois pour s'assurer que tout est correctement affiché
}

void soigner() {
  delay(300);  // Délai initial pour démarrer le processus de soin
  truc.affection += 20;

  // Boucle pour afficher une animation de 14 images
  for (int i = 0; i < 15; i++) {
    delay(60);
    oled.clearDisplay();
    oled.drawBitmap(0, 16, epd_bitmap_allArray[i], 128, 128, WHITE);  // Affiche l'image de l'animation
    oled.display();
    Serial.print("Tout ce passe bien ici, image : ");
    Serial.println(i);
  }
  Serial.println("Viens de quitter l'animation incroyable");

  // Boucle pour augmenter la santé du Tamagotchi jusqu'à 100
  while (truc.health < 100) {
    Serial.println("Ok dans le soignement");
    oled.clearDisplay();
    oled.drawBitmap(0, 16, epd_bitmap_allArray[14], 128, 128, WHITE);
    oled.setCursor(3, 3);
    oled.print("vie");

    // Dessine une barre de vie en fonction de la santé actuelle du Tamagotchi
    oled.drawRect(42, 5, (int)map(truc.health, 0, 100, 0, 46), 2, WHITE);
    oled.drawRect(40, 3, 50, 6, WHITE);

    truc.health += 1;  // Augmente la santé du Tamagotchi de 1 point
    oled.display();
    delay(50);
  }

  oled.display();  // Met à jour l'affichage OLED une dernière fois pour s'assurer que tout est correctement affiché
}

void mourrir() {
  mort = false;
  oled.clearDisplay();
  if (truc.health <= 0) {
    oled.setCursor(40, 35);
    oled.print("MORT!");
    oled.display();
    delay(3000);
    oled.print("On recommence"); 
    oled.display(); 
    delay(2000); 
    preferences.clear();
    esp_restart(); 
  }
}