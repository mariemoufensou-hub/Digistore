#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* =========================================================
 *  Structures de données
 * ========================================================= */

typedef struct Produit {
    char nom[20];
    char reference[20];  /* identifiant unique du produit */
    int  quantite;        /* stock disponible              */
    float prix;
    int  vendu;           /* nb d'unités vendues au total  */
    struct Produit *suivant; /* maillon suivant de la liste  */
} Produit;

typedef struct Avis {
    char reference[20];  /* référence du produit concerné */
    char commentaire[100];
    int  etoiles;        /* note entre 0 et 5             */
    struct Avis *suivant;
} Avis;

typedef struct PanierItem {
    char reference[20];  /* référence du produit          */
    int  quantite;
    struct PanierItem *suivant;
} PanierItem;

static Produit    *listeProduits = NULL;
static Avis       *listeAvis     = NULL;
static PanierItem *listePanier   = NULL;

/* saisie */
int   saisirEntier(void);
float saisirFloat(void);

Produit    *rechercherParRef(const char *ref);
void        ajouterEnFinProduit(Produit *n);
void        ajouterEnFinAvis(Avis *n);
void        ajouterEnFinPanier(PanierItem *n);

void sauvegarderProduits(void);
void chargerProduits(void);
void sauvegarderAvis(void);
void chargerAvis(void);

void ajouterProduit(void);
void supprimerProduit(void);
void afficherProduits(void);
void rechercherProduit(void);
void vendreProduit(void);

void ajouterAuPanier(void);
void afficherPanier(void);
void validerPanier(void);

void ajouterAvis(void);
void afficherAvis(void);
void statistiques(void);
void afficherChiffreAffaires(void);

int  loginAdmin(void);
void menuAdmin(void);
void menuClient(void);

void libererMemoire(void);


int saisirEntier(void) {
    int x;
    while (scanf("%d", &x) != 1) {
        printf("Erreur, entrez un entier : ");
        scanf("%*s");
    }
    return x;
}

float saisirFloat(void) {
    float x;
    while (scanf("%f", &x) != 1) {
        printf("Erreur, entrez un nombre : ");
        scanf("%*s");
    }
    return x;
}


/* Recherche un produit */
Produit *rechercherParRef(const char *ref) {
    Produit *p = listeProduits;
    for (listeProduits; p != NULL; p = p->suivant) {
        if (strcmp(p->reference, ref) == 0)
            return p;
    }
    return NULL;
}

/* Ajoute un nœud en fin de liste de produits */
void ajouterEnFinProduit(Produit *n) {
    if (listeProduits == NULL) {
        listeProduits = n;
        return;
    }
    Produit *tmp = listeProduits;
    while (tmp->suivant != NULL) tmp = tmp->suivant;
    tmp->suivant = n;
}

/* Ajoute un nœud en fin de liste d'avis */
void ajouterEnFinAvis(Avis *n) {
    if (listeAvis == NULL) {
        listeAvis = n;
        return;
    }
    Avis *tmp = listeAvis;
    while (tmp->suivant != NULL) tmp = tmp->suivant;
    tmp->suivant = n;
}

/* Ajoute un nœud en fin du panier */
void ajouterEnFinPanier(PanierItem *n) {
    if (listePanier == NULL) {
        listePanier = n;
        return;
    }
    PanierItem *tmp = listePanier;
    while (tmp->suivant != NULL) tmp = tmp->suivant;
    tmp->suivant = n;
}

void sauvegarderProduits(void) {
    FILE *f = fopen("produits.txt", "w");
    if (!f) { perror("Impossible d'ouvrir produits.txt"); return; }
    Produit *p = listeProduits;
    for ( listeProduits; p != NULL; p = p->suivant) {
        fprintf(f, "%s %s %d %.2f %d\n",
                p->nom, p->reference, p->quantite, p->prix, p->vendu);
    }
    fclose(f);
}

void chargerProduits(void) {
    FILE *f = fopen("produits.txt", "r");
    if (!f) return; /* fichier absent au premier lancement, c'est normal */

    char  nom[20], ref[20];
    int   qte, vendu;
    float prix;

    while (fscanf(f, "%19s %19s %d %f %d", nom, ref, &qte, &prix, &vendu) == 5) {
        Produit *n = (Produit *)malloc(sizeof(Produit));
        if (!n) { fclose(f); return; }

        strcpy(n->nom,       nom);
        strcpy(n->reference, ref);
        n->quantite = qte;
        n->prix     = prix;
        n->vendu    = vendu;
        n->suivant  = NULL;
        ajouterEnFinProduit(n);
    }
    fclose(f);
}


void sauvegarderAvis(void) {
    FILE *f = fopen("avis.txt", "w");
    if (!f) { perror("Impossible d'ouvrir avis.txt"); return; }
    Avis *a = listeAvis;
    for ( listeAvis; a != NULL; a = a->suivant) {
        fprintf(f, "%s %d |%s|\n", a->reference, a->etoiles, a->commentaire);
    }
    fclose(f);
}

void chargerAvis(void) {
    FILE *f = fopen("avis.txt", "r");
    if (!f) return;

    char ref[20], commentaire[100];
    int  etoiles;

    while (fscanf(f, "%19s %d |%99[^|]|", ref, &etoiles, commentaire) == 3) {
        Avis *n = (Avis *)malloc(sizeof(Avis));
        if (!n) { fclose(f); return; }

        strcpy(n->reference,   ref);
        strcpy(n->commentaire, commentaire);
        n->etoiles = etoiles;
        n->suivant = NULL;
        ajouterEnFinAvis(n);
    }
    fclose(f);
}

/*Gestion des produits*/

void ajouterProduit(void) {
    Produit *n = (Produit *)malloc(sizeof(Produit));
    if (!n) { printf("Memoire insuffisante\n"); return; }

    printf("Nom       : "); scanf("%19s", n->nom);
    printf("Reference : "); scanf("%19s", n->reference);

    /* vérifier que la référence est unique */
    if (rechercherParRef(n->reference) != NULL) {
        printf("Erreur : cette reference existe déjà.\n");
        free(n);
        return;
    }

    printf("Quantite  : "); n->quantite = saisirEntier();
    printf("Prix      : "); n->prix     = saisirFloat();
    n->vendu   = 0;
    n->suivant = NULL;

    ajouterEnFinProduit(n);
    sauvegarderProduits();
    printf("Produit ajoute.\n");
}

void supprimerProduit(void) {
    if (listeProduits == NULL) { printf("Aucun produit.\n"); return; }

    afficherProduits();
    char ref[20];
    printf("Reference du produit à supprimer : ");
    scanf("%19s", ref);

    Produit *courant  = listeProduits;
    Produit *precedent = NULL;

    while (courant != NULL) {
        if (strcmp(courant->reference, ref) == 0) {
            if (precedent == NULL)
                listeProduits = courant->suivant;
            else
                precedent->suivant = courant->suivant;

            free(courant);
            sauvegarderProduits();
            printf("Produit supprime.\n");
            return;
        }
        precedent = courant;
        courant   = courant->suivant;
    }
    printf("Reference introuvable.\n");
}

void modifierProduit(void) {
    char ref[20];

    printf("Reference du produit a modifier : ");
    scanf("%19s", ref);

    Produit *p = rechercherParRef(ref);

    if (p == NULL) {
        printf("Produit introuvable.\n");
        return;
    }

    printf("\n--- Produit trouve ---\n");
    printf("Nom actuel      : %s\n", p->nom);
    printf("Quantite actuelle: %d\n", p->quantite);
    printf("Prix actuel      : %.2f\n", p->prix);

    printf("\n--- Nouvelle valeur ---\n");

    printf("Nouveau nom : ");
    scanf("%19s", p->nom);

    printf("Nouvelle quantite : ");
    int qte = saisirEntier();
    if (qte < 0) {
        printf("Quantite invalide.\n");
        return;
    }
    p->quantite = qte;

    printf("Nouveau prix : ");
    float prix = saisirFloat();
    if (prix < 0) {
        printf("Prix invalide.\n");
        return;
    }
    p->prix = prix;

    sauvegarderProduits();

    printf("Produit modifie avec succes.\n");
}

void afficherProduits(void) {
    if (listeProduits == NULL) { printf("Aucun produit disponible.\n"); return; }
    printf("\n%-20s %-12s %8s %10s\n", "Nom", "Référence", "Stock", "Prix");
    printf("%-20s %-12s %8s %10s\n",
           "--------------------", "------------", "-------", "---------");
    Produit *p = listeProduits;
    for (listeProduits; p != NULL; p = p->suivant) {
        printf("%-20s %-12s %8d %10.2f\n",
               p->nom, p->reference, p->quantite, p->prix);
    }
}

void rechercherProduit(void) {
    char nom[20];
    printf("Nom du produit à chercher : ");
    scanf("%19s", nom);

    int trouve = 0;
    Produit *p = listeProduits;
    for (listeProduits; p != NULL; p = p->suivant) {
        if (strcmp(p->nom, nom) == 0) {
            printf("\nNom       : %s\n", p->nom);
            printf("Reference : %s\n",  p->reference);
            printf("Stock     : %d\n",  p->quantite);
            printf("Prix      : %.2f\n", p->prix);
            trouve = 1;
        }
    }
    if (!trouve) printf("Produit non trouvé.\n");
}

void vendreProduit(void) {
    afficherProduits();
    char ref[20];
    printf("Reference du produit : ");
    scanf("%19s", ref);

    Produit *p = rechercherParRef(ref);
    if (p == NULL) { printf("Reference invalide.\n"); return; }

    printf("Quantite : ");
    int qte = saisirEntier();

    if (qte <= 0) { printf("Quantité invalide.\n"); return; }

    if (qte > p->quantite) {
        printf("Stock insuffisant (%d disponibles).\n", p->quantite);
        return;
    }

    p->quantite -= qte;
    p->vendu    += qte;
    float total  = qte * p->prix;

    printf("\n----- RÉCAPITULATIF -----\n");
    printf("Produit          : %s\n",   p->nom);
    printf("Quantité achetee : %d\n",   qte);
    printf("Prix unitaire    : %.2f\n", p->prix);
    printf("Total            : %.2f\n", total);
    printf("Stock restant    : %d\n",   p->quantite);
    sauvegarderProduits();
}

/* =========================================================
 *  Panier
 * ========================================================= */

void ajouterAuPanier(void) {
    afficherProduits();
    char ref[20];
    printf("Reference du produit : ");
    scanf("%19s", ref);

    Produit *p = rechercherParRef(ref);
    if (p == NULL) { printf("Reference invalide.\n"); return; }

    printf("Quantite : ");
    int qte = saisirEntier();

    if (qte <= 0 || qte > p->quantite) {
        printf("Quantite invalide ou stock insuffisant (%d disponibles).\n", p->quantite);
        return;
    }

    PanierItem *item = (PanierItem *)malloc(sizeof(PanierItem));
    if (!item) { printf("Memoire insuffisante.\n"); return; }

    strcpy(item->reference, ref);
    item->quantite = qte;
    item->suivant  = NULL;
    ajouterEnFinPanier(item);
    printf("Ajoute au panier.\n");
}

void afficherPanier(void) {
    if (listePanier == NULL) { printf("Panier vide.\n"); return; }

    float total = 0;
    printf("\n--- Panier ---\n");
    PanierItem *it = listePanier;
    for (listePanier; it != NULL; it = it->suivant) {
        Produit *p = rechercherParRef(it->reference);
        if (p != NULL) {
            float sous = it->quantite * p->prix;
            printf("%-20s x%d  %.2f\n", p->nom, it->quantite, sous);
            total += sous;
        }
    }
    printf("Total estime: %.2f\n", total);
}

void validerPanier(void) {
    if (listePanier == NULL) {
        printf("Panier vide.\n");
        return;
    }

    /* 1. Vérification du stock AVANT validation */
    PanierItem *it = listePanier;

    for (it = listePanier; it != NULL; it = it->suivant) {
        Produit *p = rechercherParRef(it->reference);

        if (p == NULL) {
            printf("Erreur : produit '%s' introuvable.\n", it->reference);
            return;
        }

        if (it->quantite > p->quantite) {
            printf("Stock insuffisant pour %s (demande: %d, disponible: %d)\n",
                   p->nom, it->quantite, p->quantite);
            return;
        }
    }

    /* 2. Validation + mise à jour */
    float total = 0;

    printf("\n===== FACTURE =====\n");

    for (it = listePanier; it != NULL; it = it->suivant) {
        Produit *p = rechercherParRef(it->reference);

        p->quantite -= it->quantite;
        p->vendu    += it->quantite;

        float sous = it->quantite * p->prix;
        total += sous;

        printf("%-20s | %d x %.2f = %.2f\n",
               p->nom, it->quantite, p->prix, sous);
    }

    printf("-------------------\n");
    printf("TOTAL : %.2f\n", total);

    /* 3. Vider le panier */
    PanierItem *tmp;
    while (listePanier != NULL) {
        tmp = listePanier;
        listePanier = listePanier->suivant;
        free(tmp);
    }

    sauvegarderProduits();
    printf("Commande validee !\n");
}
/* =========================================================
 *  Avis clients
 * ========================================================= */

void ajouterAvis(void) {
    afficherProduits();
    char ref[20];
    printf("Reference du produit : ");
    scanf("%19s", ref);

    if (rechercherParRef(ref) == NULL) { printf("Reference invalide.\n"); return; }

    printf("Note (0 à 5) : ");
    int note = saisirEntier();
    if (note < 0 || note > 5) { printf("Note invalide.\n"); return; }

    Avis *n = (Avis *)malloc(sizeof(Avis));
    if (!n) { printf("Memoire insuffisante.\n"); return; }

    strcpy(n->reference, ref);
    n->etoiles = note;
    printf("Commentaire : ");
    scanf(" %99[^\n]", n->commentaire);
    n->suivant = NULL;

    ajouterEnFinAvis(n);
    sauvegarderAvis();
    printf("Avis ajoute.\n");
}

void afficherAvis(void) {
    if (listeAvis == NULL) { printf("Aucun avis.\n"); return; }

    printf("\n--- Avis clients ---\n");
    Avis *a = listeAvis;
    for (listeAvis; a != NULL; a = a->suivant) {
        Produit *p = rechercherParRef(a->reference);
        const char *nom = (p != NULL) ? p->nom : a->reference;
        printf("%-20s | %d/5 | %s\n", nom, a->etoiles, a->commentaire);
    }
}

/*Statistiques et chiffre d'affaires*/

void afficherChiffreAffaires(void) {
    if (listeProduits == NULL) { printf("Aucun produit.\n"); return; }

    float total = 0;
    printf("\n=== Chiffre d'affaires ===\n");
    Produit *p = listeProduits;
    for (listeProduits; p != NULL; p = p->suivant) {
        float ca = p->prix * p->vendu;
        printf("%-20s | Ventes : %d | CA : %.2f\n", p->nom, p->vendu, ca);
        total += ca;
    }
    printf("-------------------------------\n");
    printf("Total : %.2f\n", total);
}

void statistiques(void) {
    if (listeProduits == NULL) {
        printf("Aucun produit.\n");
        return;
    }

    /* PRODUITS  */
    Produit *plusVendu  = listeProduits;
    Produit *moinsVendu = listeProduits;
    Produit *p = listeProduits->suivant;
    for (listeProduits->suivant; p != NULL; p = p->suivant) {
        if (p->vendu > plusVendu->vendu)
            plusVendu = p;

        if (p->vendu < moinsVendu->vendu)
            moinsVendu = p;
    }

    /* NOTES */
    float totalNotes = 0;
    int nbNotes = 0;

    Avis *a = listeAvis;

    /* meilleur / pire avis */
    Avis *meilleurAvis = listeAvis;
    Avis *pireAvis = listeAvis;

    for (a = listeAvis; a != NULL; a = a->suivant) {
        totalNotes += a->etoiles;
        nbNotes++;

        if (meilleurAvis == NULL || a->etoiles > meilleurAvis->etoiles)
            meilleurAvis = a;

        if (pireAvis == NULL || a->etoiles < pireAvis->etoiles)
            pireAvis = a;
    }

    float moyenne = (nbNotes > 0) ? totalNotes / nbNotes : 0.0f;

    printf("\n===== STATISTIQUES =====\n");

    printf("Plus vendu      : %s (%d ventes)\n",
           plusVendu->nom, plusVendu->vendu);

    printf("Moins vendu     : %s (%d ventes)\n",
           moinsVendu->nom, moinsVendu->vendu);


    if (nbNotes > 0) {
        Produit *pBest = rechercherParRef(meilleurAvis->reference);
        Produit *pWorst = rechercherParRef(pireAvis->reference);

        printf("Meilleur note   : %s (%d/5)\n",
               pBest ? pBest->nom : meilleurAvis->reference,
               meilleurAvis->etoiles);

        printf("Moins bien note : %s (%d/5)\n",
               pWorst ? pWorst->nom : pireAvis->reference,
               pireAvis->etoiles);
        printf("Moyenne notes   : %.2f / 5 (%d avis)\n",
            moyenne, nbNotes);
    }

    /* =======================
       MESSAGE GLOBAL
    ======================= */
    if (nbNotes == 0)
        printf("INFO : Aucun avis pour l'instant.\n");
    else if (moyenne >= 3)
        printf("INFO : Clients globalement satisfaits.\n");
    else
        printf("ALERTE : Insatisfaction des clients !\n");
}

/*Authentification admin*/

int loginAdmin(void) {
    char username[20], password[20];

    printf("\n=== Connexion Admin ===\n");
    printf("Nom d'utilisateur : "); scanf("%19s", username);
    printf("Mot de passe : "); scanf("%19s", password);
    if (strcmp(username, "admin") == 0 && strcmp(password, "1234") == 0) {
        printf("Connexion reussie !\n");
        return 1;
    }
    printf("Identifiants incorrects.\n");
    return 0;
}

/*Menus*/

void menuAdmin(void) {
    int choix;
    do {
        printf("\n--- ADMIN ---\n");
        printf("1. Ajouter un produit\n");
        printf("2. Modifier un produit\n");
        printf("3. Supprimer un produit\n");
        printf("4. Afficher les produits\n");
        printf("5. Voir les avis\n");
        printf("6. Statistiques generales\n");
        printf("7. Chiffre d'affaires\n");
        printf("0. Retour\n> ");
        choix = saisirEntier();
        switch (choix) {
            case 1: ajouterProduit();        break;
            case 2: modifierProduit(); break;
            case 3: supprimerProduit();      break;
            case 4: afficherProduits();      break;
            case 5: afficherAvis();          break;
            case 6: statistiques();          break;
            case 7: afficherChiffreAffaires(); break;
            case 0: break;
            default: printf("Option invalide.\n");
        }
    } while (choix != 0);
}

void menuClient(void) {
    int choix;
    do {
        printf("\n--- CLIENT ---\n");
        printf("1. Rechercher un produit\n");
        printf("2. Voir les produits\n");
        printf("3. Ajouter au panier\n");
        printf("4. Voir le panier\n");
        printf("5. Valider la commande\n");
        printf("6. Donner un avis\n");
        printf("0. Retour\n> ");
        choix = saisirEntier();
        switch (choix) {
            case 1: rechercherProduit(); break;
            case 2: afficherProduits();  break;
            case 3: ajouterAuPanier();   break;
            case 4: afficherPanier();    break;
            case 5: validerPanier();     break;
            case 6: ajouterAvis();       break;
            case 0: break;
            default: printf("Option invalide.\n");
        }
    } while (choix != 0);
}

/*  Libération mémoire */

void libererMemoire(void) {
    Produit *p = listeProduits;
    while (p) { Produit    *t = p->suivant; free(p); p = t; }

    Avis *a = listeAvis;
    while (a) { Avis       *t = a->suivant; free(a); a = t; }

    PanierItem *i = listePanier;
    while (i) { PanierItem *t = i->suivant; free(i); i = t; }
}

int main(void) {
    chargerProduits();
    chargerAvis();

    int choix;
    do {
        printf("\n=== DIGISTORE ===\n");
        printf("1. Espace Admin\n");
        printf("2. Espace Client\n");
        printf("0. Quitter\n> ");
        choix = saisirEntier();
        switch (choix) {
            case 1:
                if (loginAdmin()) menuAdmin();
                break;
            case 2:
                menuClient();
                break;
            case 0:
                break;
            default:
                printf("Option invalide.\n");
        }
    } while (choix != 0);

    libererMemoire();
    return 0;
}
