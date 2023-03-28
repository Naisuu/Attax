#include <stdio.h>
#include <stdlib.h>
#include <MLV/MLV_all.h>
#include <time.h>

#define TAILLE_PLATEAU 7
#define TAILLE_MAX_NOM 20
#define TAILLE_CASE 50
#define MARGE_X 100
#define MARGE_Y 100

typedef struct
{
  char nom[TAILLE_MAX_NOM];
  char symbol;
  int score;
} Joueur;

typedef struct
{
  char plateau[TAILLE_PLATEAU + 2][TAILLE_PLATEAU + 2];
  Joueur *joueurs[2];
} Plateau;

void initPlateau(Plateau *p, Joueur *j1, Joueur *j2)
{
  // Initialise le plateau en plaçant des '*' sur les bordures
  // 'x' ou 'o' sur chaque extrémités ainsi que '.' pour le reste
  // et insère les 2 joueurs dans p.

  int i, j;
  for (i = 0; i < TAILLE_PLATEAU + 2; i++)
  {
    for (j = 0; j < TAILLE_PLATEAU + 2; j++)
    {
      if (i == 0 || i == TAILLE_PLATEAU + 1 || j == 0 || j == TAILLE_PLATEAU + 1)
      {
        p->plateau[i][j] = '*';

        // L’affichage ASCII d’un tableau contient un espace invisible en fin de chaque ligne.

        if (j == TAILLE_PLATEAU + 1)
          strcpy(&p->plateau[i][j], "* ");
      }
      else if ((i == 1 && j == 1) || (i == TAILLE_PLATEAU && j == TAILLE_PLATEAU))
        p->plateau[i][j] = 'x';

      else if ((i == 1 && j == TAILLE_PLATEAU) || (i == TAILLE_PLATEAU && j == 1))
        p->plateau[i][j] = 'o';

      else {p->plateau[i][j] = '.';}
    }
    p->joueurs[0] = j1;
    p->joueurs[1] = j2;
  }
}

void initJoueur(Joueur *p, Joueur *j, int humanPC)
{
  // Initialise les joueurs J1 et J2 en demandant aux utilisateurs 
  // d'entrer leur nom et rempli les attributs symbol et score, dans
  // le cas où humanPC est à 1, J2 est automatiquement rempli.

  printf("Quel est le nom du premier joueur (symbol o) :");
  scanf(" %s", p->nom);
  p->symbol = 'o';
  p->score = 2;
  j->symbol = 'x';
  j->score = 2;
  if (humanPC == 0)
  {
    printf("Quel est le nom du deuxieme joueur (symbol x) :");
    scanf(" %s", j->nom);
  }
  if (humanPC == 1)
  {
    strcpy(j->nom, "Ordinateur");
  }
}

void affichePlateau(const Plateau *p)
{
  // Cette fonction permettra d’afficher le plateau
  // en version ASCII après son initialisation.

  for (int i = 0; i < TAILLE_PLATEAU + 2; i++)
  {
    for (int j = 0; j < TAILLE_PLATEAU + 2; j++)
    {
      printf("%c ", p->plateau[i][j]);
    }
    printf("\n");
  }
}

void affichePlateau2D(const Plateau *p)
{
  // Cette fonction permettra d’afficher le plateau
  // en version graphique après son initialisation.

  int ax, ay, bx, by;
  for (int i = 1; i < TAILLE_PLATEAU + 1; i++)
  {
    for (int j = 1; j < TAILLE_PLATEAU + 1; j++)
    {
      ax = j * TAILLE_CASE + MARGE_X;
      ay = i * TAILLE_CASE + MARGE_Y;
      bx = ax + TAILLE_CASE;
      by = ay + TAILLE_CASE;

      // Dessine chaque case du plateau ainsi les potentiels pions.

      MLV_draw_rectangle(j * TAILLE_CASE + MARGE_X, i * TAILLE_CASE + MARGE_Y, TAILLE_CASE, TAILLE_CASE, MLV_COLOR_WHITE);
      if (p->plateau[i][j] == p->joueurs[0]->symbol)
        MLV_draw_filled_circle(ax + (TAILLE_CASE / 2), ay + (TAILLE_CASE / 2), 10, MLV_COLOR_RED);
      if (p->plateau[i][j] == p->joueurs[1]->symbol)
        MLV_draw_filled_circle(ax + (TAILLE_CASE / 2), ay + (TAILLE_CASE / 2), 10, MLV_COLOR_CYAN);
    }
  }
}

int caseLibre(const Plateau *p, int x, int y)
{
  // Verifie si la coordonnée de la case est comprise
  // dans les dimensions du plateau et qu'elle soit vide.

  char c = p->plateau[y][x];
  if (((x >= 1 && x <= TAILLE_PLATEAU) && (y >= 1 && y <= TAILLE_PLATEAU)) && (c == '.'))
    return 1;
  else
    return 0;
}

int caseVoisine(Plateau *p, Joueur *joueur, int x, int y)
{
  // Verifie si la case est placable donc que les cases voisines
  // de la notre sont dotés d'au moins une case ennemi.

  // Initialisation du symbole ennemi.

  char symbolOppose;
  int returnVal = 0;
  if (joueur->symbol == 'o')
    symbolOppose = 'x';
  else
  {
    symbolOppose = 'o';
  }

  // Si une case ennemi est voisine de la notre et que
  // notre case est vide alors la case ennemi devient la notre,
  // le score est également mis à jour.

  for (int i = -1; i < 2; i++)
  { 
    for (int j = -1; j < 2; j++)
    { 
      if ((p->plateau[y + i][x + j] == symbolOppose) && (p->plateau[y][x] == '.'))
      {
        p->plateau[y + i][x + j] = joueur->symbol;
        joueur->score += 1;
        if (p->joueurs[0] == joueur)
        {
          p->joueurs[1]->score -= 1;
        }
        else
        {
          p->joueurs[0]->score -= 1;
        }
        returnVal = 1;
      }
    }
  }
  return returnVal;
}

void placePions(Plateau *p, Joueur *joueur, int humanPC)
{
  // Demande aux utilisateurs d'entrer les coordonnées d'une case
  // tant que cette case ne reste pas libre ou que la case
  // n'est voisine d'aucune case ennemi, si le J2 est l'ordinateur
  // les coordonnées sont choisis aléatoirement.

  int x, y;
  do
  {
    if (humanPC == 0 || (humanPC == 1 && joueur->symbol == 'o'))
    {
      printf("%s (%c), veuillez saisir des coordonnees entre (1 et %d) :\n", joueur->nom, joueur->symbol,TAILLE_PLATEAU);
      scanf(" %d", &y);
      scanf(" %d", &x);
    }
    else
    {
      x = rand() % 8;
      y = rand() % 8;
    }

  } while ((caseLibre(p, x, y) == 0) || (caseVoisine(p, joueur, x, y) == 0)); // 2 conditions obligatoirement validés pour sortir

  p->plateau[y][x] = joueur->symbol;
  joueur->score += 1;
}

int verifVictoire(const Plateau *p, int asciiGraph, int jval)
{
  // Verifie s'il y a un gagnant sur le plateau et adapte
  // l'affichage en fonction de asciiGraph.

  // un des joueurs à un score de 0 ?
  if (p->joueurs[0]->score == 0)
  {
    printf("Bravo %s, vous avez gagné %d à %d.\n", p->joueurs[1]->nom, p->joueurs[1]->score, p->joueurs[0]->score);
    if (asciiGraph == 1)
    {
      MLV_draw_adapted_text_box(200, 550, "Le gagnant est : ", 0, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_LEFT);
      MLV_draw_adapted_text_box(350, 550, p->joueurs[1]->nom, 0, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_LEFT);
      MLV_actualise_window();
    }
    return 1;
  }
  else if (p->joueurs[1]->score == 0)
  {
    printf("Bravo %s, vous avez gagné %d à %d.\n", p->joueurs[0]->nom, p->joueurs[0]->score, p->joueurs[1]->score);
    if (asciiGraph == 1)
    {
      MLV_draw_adapted_text_box(200, 550, "Le gagnant est : ", 0, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_LEFT);
      MLV_draw_adapted_text_box(350, 550, p->joueurs[0]->nom, 0, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_LEFT);
      MLV_actualise_window();
    }
    return 1;
  }

  // Compte le nombre de case disponible sur le plateau

  int nbCaseDispo = 0;
  int boucle = 0;
  int i, j,coorCase_i,coorCase_j;
  char symbolOppose;

  for (i = 1; i < TAILLE_PLATEAU + 1; i++)
  {
    for (j = 1; j < TAILLE_PLATEAU + 1; j++)
      {
        if (caseLibre(p, i, j) == 1)
        {
          //On cherche une case disponible pour le placement de jval
          if (p->joueurs[jval]->symbol == 'o')
            symbolOppose = 'x';
          else
          {
            symbolOppose = 'o';
          }
          for (int a = -1; a < 2; a++)
          {
            for (int b = -1; b < 2; b++)
            { 
              if ((((i + a) >= 1) && ((i + a) <= TAILLE_PLATEAU)) && (((j + b) >= 1) && (j + b) <= TAILLE_PLATEAU))
              {
                if (p->plateau[i + a][j + b] == symbolOppose)
                {
                  return 0;
                }
              }
            }
          }
        }
      }
  }

  // Si jeu est rempli ou jval ne peux plus jouer
  // on nomme les résultats, le gagnant ou égalité.

  if (p->joueurs[0]->score > p->joueurs[1]->score)
  {
    printf("Bravo %s, vous avez gagné %d à %d.\n", p->joueurs[0]->nom, p->joueurs[0]->score, p->joueurs[1]->score);
    if (asciiGraph == 1)
    {
      MLV_draw_adapted_text_box(200, 550, "Le gagnant est : ", 0, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_LEFT);
      MLV_draw_adapted_text_box(350, 550, p->joueurs[0]->nom, 0, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_LEFT);
      MLV_actualise_window();
    }
    return 1;
  }
  else if (p->joueurs[0]->score < p->joueurs[1]->score)
  {
    printf("Bravo %s, vous avez gagné %d à %d.\n", p->joueurs[1]->nom, p->joueurs[1]->score, p->joueurs[0]->score);
    if (asciiGraph == 1)
    {
      MLV_draw_adapted_text_box(200, 550, "Le gagnant est : ", 0, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_LEFT);
      MLV_draw_adapted_text_box(350, 550, p->joueurs[1]->nom, 0, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_LEFT);
      MLV_actualise_window();
    }
    return 1;
  }
  else
  {
    printf("Egalité %d à %d.\n", p->joueurs[0]->score, p->joueurs[1]->score);
    if (asciiGraph == 1)
    {
      MLV_draw_adapted_text_box(300, 550, "Egalité ! ", 0, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_LEFT);
      MLV_actualise_window();
    }
    return 1;
  }
}

int options(int argc, char *argv[], int *asciiGraph, int *humanPC)
{
  // Gere  les options a, g, h et o et attribue les valeurs
  // à asciiGraph (0 si ASCII et 1 pour Graph)
  // à humanPC (0 si J1J2 et 1 si ordinateur)

  if (argc == 3)
  { 
    for (int i = 0; i < 2; i++)
    {
      if (((strcmp(argv[1 + i], "-a") == 0) || (strcmp(argv[1 + i], "-g") == 0)) && // -a/-g | -h/-o
          ((strcmp(argv[2 - i], "-h") == 0) || (strcmp(argv[2 - i], "-o") == 0)))
      {
        if (strcmp(argv[1 + i], "-g") == 0)
        {
          *asciiGraph = 1;
        }
        else
        {
          *asciiGraph = 0;
        }
        if (strcmp(argv[2 - i], "-o") == 0)
        {
          *humanPC = 1;
          break;
        }
        else
        {
          *humanPC = 0;
          break;
        }
      }
    }
    if (*humanPC < 0 || *asciiGraph < 0)
    {
      ;
      printf("Erreur: ./ataxx.out [-a/-g | -h/-o]\n");
      return 0;
    }
  }
  else
  {
    printf("Erreur: ./ataxx.out [-a/-g | -h/-o]\n");
    return 0;
  }
  return 1;
}

void placePions2D(Plateau *p, Joueur *j, int humanPC)
{
  // Attend un clic du joueur sur une case valide comme
  // la fonction placePions(..) dans le cas où le joueur
  // serait l'ordinateur placePions(..) est alors appellée.

  if ((humanPC == 0) || ((humanPC == 1) && (j->symbol == 'o')))
  {
    int stop = 0;
    do
    {
      int clicX, clicY, ax, ay, bx, by;
      MLV_wait_mouse(&clicX, &clicY);
      for (int colonne = 0; colonne < TAILLE_PLATEAU + 2 && stop == 0; colonne++)
      {
        for (int ligne = 0; ligne < TAILLE_PLATEAU + 2 && stop == 0; ligne++)
        {
          ax = ligne * TAILLE_CASE + MARGE_X;
          ay = colonne * TAILLE_CASE + MARGE_Y;
          bx = ax + TAILLE_CASE;
          by = ay + TAILLE_CASE;

          if (((ax <= clicX) && (clicX <= bx)) && ((ay <= clicY) && (clicY <= by)))
          { // verification dimension d'une case
            if ((caseLibre(p, ligne, colonne) == 1) && (caseVoisine(p, j, ligne, colonne) == 1))
            { // verifie si ligne colonne correspondant au clic est OK
              p->plateau[colonne][ligne] = j->symbol;
              j->score += 1;
              stop = 1;
            }
          }
        }
      }
    } while (stop == 0);
  }
  else
  {
    placePions(p, j, humanPC);
  }
}

void jeuAscii(Plateau *p, int asciiGraph, int humanPC)
{
  // Scénario d'une partie sur un plateau ASCII

  int j = 0;
  affichePlateau(p);
  while (verifVictoire(p, asciiGraph, j) == 0)
  { // tant qu'aucun joueur n'a gagné
    placePions(p, p->joueurs[j], humanPC);
    affichePlateau(p);
    printf("Score actuel: %s(%c) %d - %s(%c) %d\n\n", p->joueurs[0]->nom, p->joueurs[0]->symbol, p->joueurs[0]->score, p->joueurs[1]->nom, p->joueurs[1]->symbol, p->joueurs[1]->score);
    j += 1;
    if (j > 1)
    {
      j = 0;
    }
  }
}
void jeuGraphique(Plateau *p, int asciiGraph, int humanPC)
{
  // Scénario d'une partie sur un plateau graphique

  MLV_create_window("Projet Ataxx", "ataxx", 800, 800);
  affichePlateau2D(p);
  MLV_actualise_window();
  int j = 0;
  while (verifVictoire(p, asciiGraph, j) == 0)
  { // tant qu'aucun joueur n'a gagné
    MLV_draw_adapted_text_box(0, 0, p->joueurs[j]->nom, 0, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_LEFT);
    MLV_actualise_window();
    placePions2D(p, p->joueurs[j], humanPC);
    affichePlateau2D(p);
    MLV_draw_adapted_text_box(0, 0, "                                           ", 0, MLV_COLOR_BLACK, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_LEFT);
    MLV_actualise_window();
    j += 1;
    if (j > 1)
    {
      j = 0;
    }
  }
  MLV_wait_seconds(100);
  MLV_free_window();
}

int main(int argc, char *argv[])
{
  // Options de la partie
  
  int asciiGraph = -1;
  int humanPC = -1;
  if (options(argc, argv, &asciiGraph, &humanPC) == 0)
  {
    return 1;
  }

  // initialisation plateau et joueurs

  Plateau jeuPlateau;
  Joueur j1;
  Joueur j2;
  srand(time(NULL));
  initJoueur(&j1, &j2, humanPC);
  initPlateau(&jeuPlateau, &j1, &j2);

  // dépendemment des options lance le jeu 

  if (asciiGraph == 0)
  {
    jeuAscii(&jeuPlateau, asciiGraph, humanPC);
  }
  else
  {
    jeuGraphique(&jeuPlateau, asciiGraph, humanPC);
  }
  return 0;
}
