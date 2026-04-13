#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Les structures

struct produit {
    char nom[20];
    char reference[20];
    int quantite;
    float prix;
    int vendu;
};

struct avis {
    int produitIndex;
    char commentaire[100];
    int etoiles;
};

struct panierArticle {
    int produitIndex;
    int quantite;
};
struct produit produits[1000];// tableau des produits
struct avis avisList[1000]; // tableau des avis
struct panierArticle panier[1000];// tableau du panier

// Les variables
int nbPanier = 0;
int nbProduits = 0;
int nbAvis = 0;

// validation
// Cette fonction permet de saisir un entier valide :
int saisirEntier() {
    int x;
    while(scanf("%d", &x) != 1) {
        printf("Erreur, entrer un entier : ");
        scanf("%*s");
    }
    return x;
}
// Cette fonction permet de saisir un nombre décimal valide :
float saisirFloat() {
    float x;
    while(scanf("%f", &x) != 1) {
        printf("Erreur, entrer un nombre : ");
        scanf("%*s");
    }
    return x;
}

// Les produits

// sauvegarde produits dans fichier
void sauvegarderProduits() {
    FILE *f = fopen("produits.txt", "w");
    int i = 0;
    for(i; i < nbProduits; i++) {
        fprintf(f, "%s %s %d %.2f %d\n",
            produits[i].nom,
            produits[i].reference,
            produits[i].quantite,
            produits[i].prix,
            produits[i].vendu);
    }
    fclose(f);
}

// charger produits depuis fichier
void chargerProduits() {
    FILE *f = fopen("produits.txt", "r");
    if(!f) return;

    while(fscanf(f, "%s %s %d %f %d",
        produits[nbProduits].nom,
        produits[nbProduits].reference,
        &produits[nbProduits].quantite,
        &produits[nbProduits].prix,
        &produits[nbProduits].vendu) == 5)
    {
        nbProduits++;
    }
    fclose(f);
}
// ajouter un produit
void ajouterProduit() {
    if(nbProduits >=1000) return;

    printf("Nom: "); scanf("%s", produits[nbProduits].nom);
    printf("Reference: "); scanf("%s", produits[nbProduits].reference);
    printf("Quantite: "); produits[nbProduits].quantite = saisirEntier();
    printf("Prix: "); produits[nbProduits].prix = saisirFloat();
    produits[nbProduits].vendu = 0;

    nbProduits++;
    sauvegarderProduits();
}
// afficher produits
void afficherProduits() {
    int i = 0;
    for( i; i < nbProduits; i++) {
        printf("%d - %s | %s | Qt:%d | Prix:%.2f\n",
            i,
            produits[i].nom,
            produits[i].reference,
            produits[i].quantite,
            produits[i].prix);
    }
}

// Vente

// vente directe
void vendreProduit() {
    int index, qte;

    afficherProduits();

    printf("Index produit: ");
    index = saisirEntier();
// vérification index
    if(index < 0 || index >= nbProduits) {
        printf("Index invalide\n");
        return;
    }

    printf("Quantite: ");
    qte = saisirEntier();
// vérifier stock
    if(qte <= produits[index].quantite) {
        produits[index].quantite -= qte;// diminuer stock
        produits[index].vendu += qte; // augmenter ventes

        float total = qte * produits[index].prix;

        printf("Vente OK\n");
        printf("----- RECAP COMMANDE -----\n");
        printf("Produit : %s\n", produits[index].nom);
        printf("Quantite achetee : %d\n", qte);
        printf("Prix unitaire : %.2f\n", produits[index].prix);
        printf("Total a payer : %.2f\n", total);
        printf("Stock restant : %d\n", produits[index].quantite);
        sauvegarderProduits();
    } else {
        printf("Stock insuffisant\n");
    }
}
// ajouter au panier
void ajouterAuPanier() {
    int index, qte;

    afficherProduits();

    printf("Index produit: ");
    index = saisirEntier();

    if(index < 0 || index >= nbProduits) {
        printf("Index invalide\n");
        return;
    }

    printf("Quantite: ");
    qte = saisirEntier();

    if(qte <= 0 || qte > produits[index].quantite) {
        printf("Stock insuffisant\n");
        return;
    }

    panier[nbPanier].produitIndex = index;
    panier[nbPanier].quantite = qte;
    nbPanier++;
    printf("Ajoute au panier\n");
}


// validation du panier
void validerPanier() {
    if(nbPanier == 0) {
        printf("Panier vide\n");
        return;
    }

    float total = 0;

    printf("\n===== FACTURE =====\n");

    int i = 0;
    for(i = 0; i < nbPanier; i++) {

        int p = panier[i].produitIndex;
        int q = panier[i].quantite;

        // mise à jour STOCK
        produits[p].quantite -= q;
        produits[p].vendu += q;

        float prix = q * produits[p].prix;
        total += prix;

        printf("%s | %d x %.2f = %.2f\n",
            produits[p].nom,
            q,
            produits[p].prix,
            prix);
    }

    printf("-------------------\n");
    printf("TOTAL : %.2f\n", total);

    nbPanier = 0;

    sauvegarderProduits();

    printf("Commande validee ! + stock mis a jour\n");
}
// Les avis

// sauvegarder avis
void sauvegarderAvis() {
    FILE *f = fopen("avis.txt", "w");
    if(!f) return;

    int i;
    for(i = 0; i < nbAvis; i++) {
        fprintf(f, "%d %d |%s|\n",
            avisList[i].produitIndex,
            avisList[i].etoiles,
            avisList[i].commentaire);
    }

    fclose(f);
}

// charger avis
void chargerAvis() {
    FILE *f = fopen("avis.txt", "r");
    if(!f) return;

    nbAvis = 0;

    while(fscanf(f, "%d %d |%[^|]|",
        &avisList[nbAvis].produitIndex,
        &avisList[nbAvis].etoiles,
        avisList[nbAvis].commentaire) == 3)
    {
        nbAvis++;
    }

    fclose(f);
}

// ajouter avis
void ajouterAvis() {
    if(nbAvis >=1000) return;

    int index;

    afficherProduits();

    printf("Index produit: ");
    index = saisirEntier();

    if(index < 0 || index >= nbProduits) {
        printf("Index invalide\n");
        return;
    }

    int note;
    printf("Note (1-5): ");
    note = saisirEntier();

    if(note < 1 || note > 5) {
        printf("Erreur: note entre 1 et 5\n");
        return;
    }

    avisList[nbAvis].produitIndex = index;
    avisList[nbAvis].etoiles = note;

    printf("Commentaire: ");
    scanf(" %99[^\n]", avisList[nbAvis].commentaire);

    nbAvis++;
    sauvegarderAvis();

    printf("Avis ajouté\n");
}

// Les statistiques

// afficher tous les avis
void afficherAvis() {
    int i = 0;
    for( i; i < nbAvis; i++) {
        int p = avisList[i].produitIndex;

        printf("Produit: %s | %d etoiles | %s\n",
            produits[p].nom,
            avisList[i].etoiles,
            avisList[i].commentaire);
    }
}

// meilleur produit (a partir du note moyenne)
void meilleurProduitNote() {
    float somme[1000] = {0};
    int count[1000] = {0};

    // calcul du note moyenne
    int i;
    for(i = 0; i < nbAvis; i++) {
        int p = avisList[i].produitIndex;
        somme[p] += avisList[i].etoiles;
        count[p]++;
    }

    int best = -1;
    float bestMoy = -1; // IMPORTANT

    // recherche du meilleure note moyenne
    for(i = 0; i < nbProduits; i++) {
        if(count[i] > 0) {
            float moy = somme[i] / count[i];

            if(best == -1 || moy > bestMoy) {
                bestMoy = moy;
                best = i;
            }
        }
    }

    // affichage
    if(best == -1) {
        printf("Aucun avis disponible\n");
    } else {
        printf("Meilleur produit: %s (%.2f etoiles)\n",
            produits[best].nom,
            bestMoy);
    }
}
// recherche du produit le plus vendu
void produitPlusVendu() {
    if(nbProduits == 0) {
        printf("Aucun produit\n");
        return;
    }

    int max = 0;
    int i = 1;
    for(i ; i < nbProduits; i++) {
        if(produits[i].vendu > produits[max].vendu) {
            max = i;
        }
    }

    if(produits[max].vendu == 0) {
        printf("Aucune vente effectuee\n");
    } else {
        printf("Produit le plus vendu: %s (%d ventes)\n",
            produits[max].nom,
            produits[max].vendu);
    }
}
// Les menus

void menuAdmin() {
    int choix;

    do {
        printf("\n--- ADMIN ---\n");
        printf("1. Ajouter produit\n");
        printf("2. Afficher produits\n");
        printf("3. Voir avis\n");
        printf("4. Produit mieux note\n");
        printf("5. Produit le plus vendu\n");
        printf("0. Retour\n");

        choix = saisirEntier();

        switch(choix) {
            case 1: ajouterProduit(); break;
            case 2: afficherProduits(); break;
            case 3: afficherAvis(); break;
            case 4: meilleurProduitNote(); break;
            case 5: produitPlusVendu(); break;
        }

    } while(choix != 0);
}

void menuClient() {
    int choix;

    do {
        printf("\n--- CLIENT ---\n");
        printf("1. Voir produits\n");
        printf("2. Ajouter au panier\n");
        printf("3. Valider commande\n");
        printf("4. Donner avis\n");
        printf("0. Retour\n");

        choix = saisirEntier();

        switch(choix) {
            case 1: afficherProduits(); break;
            case 2: ajouterAuPanier(); break;
            case 3: validerPanier(); break;
            case 4: ajouterAvis(); break;
        }

    } while(choix != 0);
}

// Main

int main() {
    chargerProduits();
    chargerAvis();

    int choix;

    do {
        printf("\n=== DIGISTORE ===\n");
        printf("1. Admin\n");
        printf("2. Client\n");
        printf("0. Quitter\n");

        choix = saisirEntier();

        switch(choix) {
            case 1: menuAdmin(); break;
            case 2: menuClient(); break;
        }

    } while(choix != 0);

    return 0;
}
