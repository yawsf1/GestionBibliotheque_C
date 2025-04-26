#include <stdio.h>


typedef struct {
    int id;
    char nom[100], prenom[100];
}utilisateur;


typedef struct {
    int id;
    char titre[100], auteur[100];
    int disponible;
}livre;


typedef struct{
    int idlivre;
    int idutilisateur;
    char date[40];
}emprunt;


void afficherMenu() {
    printf("**** Menu de Gestion de Bibliothèque ****\n");
    printf("1. Ajouter un livre\n");
    printf("2. Supprimer un livre\n");
    printf("3. Rechercher un livre\n");
    printf("4. Lister tous les livres\n");
    printf("5. Enregistrer un utilisateur\n");
    printf("6. Emprunter un livre\n");
    printf("7. Retourner un livre\n");
    printf("8. Quitter\n");
}

void ajouterLivre() {
    FILE *f = fopen("livres.txt", "a");
    livre l;
    printf("Titre: ");
    scanf(" %[^\n]", l.titre);
    printf("Auteur: ");
    scanf(" %[^\n]", l.auteur);
    l.id = genererIdLivre();
    l.disponible = 1;
    fwrite(&l, sizeof(livre), 1, f);
    fclose(f);
}


void supprimerLivre() {
    int idSupp;
    printf("Entrez l'ID du livre à supprimer: ");
    scanf("%d", &idSupp);

    FILE *f = fopen("livres.txt", "rb");
    FILE *temp = fopen("temp.txt", "wb");

    if (!f || !temp) {
        printf("Erreur d'ouverture du fichier.\n");
        return;
    }

    livre l;
    int trouve = 0;

    while (fread(&l, sizeof(livre), 1, f)) {
        if (l.id != idSupp) {
            fwrite(&l, sizeof(livre), 1, temp);
        } else {
            trouve = 1;
        }
    }

    fclose(f);
    fclose(temp);

    remove("livres.txt");
    rename("temp.txt", "livres.txt");

    if (trouve)
        printf("Livre supprimé avec succès.\n");
    else
        printf("Livre avec ID %d non trouvé.\n", idSupp);
}



void rechercherLivre() {
    FILE *f = fopen("livres.txt", "rb");
    if (!f) {
        printf("Erreur d'ouverture du fichier.\n");
        return;
    }

    int choix;
    printf("Rechercher par :\n1. ID\n2. Titre\nVotre choix : ");
    scanf("%d", &choix);

    livre l;
    int trouve = 0;

    if (choix == 1) {
        int id;
        printf("Entrez l'ID du livre : ");
        scanf("%d", &id);

        while (fread(&l, sizeof(livre), 1, f)) {
            if (l.id == id) {
                printf("\nLivre trouvé !\n");
                printf("ID: %d\nTitre: %s\nAuteur: %s\nDisponible: %s\n",
                       l.id, l.titre, l.auteur, l.disponible ? "Oui" : "Non");
                trouve = 1;
                break;
            }
        }
    } else if (choix == 2) {
        char titre[100];
        printf("Entrez le titre du livre : ");
        scanf(" %[^\n]", titre);

        while (fread(&l, sizeof(livre), 1, f)) {
            if (strcmp(l.titre, titre) == 0) {
                printf("\nLivre trouvé !\n");
                printf("ID: %d\nTitre: %s\nAuteur: %s\nDisponible: %s\n",
                       l.id, l.titre, l.auteur, l.disponible ? "Oui" : "Non");
                trouve = 1;
                break;
            }
        }
    } else {
        printf("Choix invalide.\n");
    }

    if (!trouve)
        printf("Aucun livre trouvé.\n");

    fclose(f);
}



void listerLivres() {
    FILE *f = fopen("livres.txt", "rb");
    if (!f) {
        printf("Erreur d'ouverture du fichier.\n");
        return;
    }

    livre l;
    printf("\n--- Liste des livres ---\n");

    while (fread(&l, sizeof(livre), 1, f)) {
        printf("ID: %d | Titre: %s | Auteur: %s | Disponible: %s\n",
               l.id, l.titre, l.auteur, l.disponible ? "Oui" : "Non");
    }

    fclose(f);
}


void enregistrerUtilisateur() {
    FILE *f = fopen("utilisateurs.txt", "ab");
    if (!f) {
        printf("Erreur d'ouverture du fichier.\n");
        return;
    }

    utilisateur u;

    printf("Nom de l'utilisateur : ");
    scanf(" %[^\n]", u.nom);
    printf("Prenom de l'utilisateur : ");
    scanf(" %[^\n]", u.prenom);

    u.id = genererIdUtilisateur();

    fwrite(&u, sizeof(utilisateur), 1, f);
    fclose(f);

    printf("Utilisateur enregistré avec succès. ID: %d\n", u.id);
}


void emprunterLivre() {
    int idLivre, idUtilisateur;
    printf("ID du livre à emprunter : ");
    scanf("%d", &idLivre);
    printf("ID de l'utilisateur : ");
    scanf("%d", &idUtilisateur);

    FILE *f = fopen("livres.txt", "rb+");
    if (!f) {
        printf("Erreur d'ouverture.\n");
        return;
    }

    livre l;
    int trouve = 0;
    while (fread(&l, sizeof(livre), 1, f)) {
        if (l.id == idLivre) {
            trouve = 1;
            if (l.disponible) {
                l.disponible = 0;
                fseek(f, -sizeof(livre), SEEK_CUR);
                fwrite(&l, sizeof(livre), 1, f);
                printf("Livre emprunté avec succès.\n");
            } else {
                printf("Ce livre n'est pas disponible.\n");
            }
            break;
        }
    }

    if (!trouve)
        printf("Livre introuvable.\n");

    fclose(f);
}



void retournerLivre() {
    int idLivre;
    printf("ID du livre à retourner : ");
    scanf("%d", &idLivre);

    FILE *f = fopen("livres.txt", "rb+");
    if (!f) {
        printf("Erreur d'ouverture.\n");
        return;
    }

    livre l;
    int trouve = 0;
    while (fread(&l, sizeof(livre), 1, f)) {
        if (l.id == idLivre) {
            trouve = 1;
            if (!l.disponible) {
                l.disponible = 1;
                fseek(f, -sizeof(livre), SEEK_CUR);
                fwrite(&l, sizeof(livre), 1, f);
                printf("Livre retourné avec succès.\n");
            } else {
                printf("Ce livre est déjà disponible.\n");
            }
            break;
        }
    }

    if (!trouve)
        printf("Livre introuvable.\n");

    fclose(f);
}

int genererIdLivre() {
    FILE *f = fopen("livres.txt", "rb");
    if (!f) return 1;

    livre l;
    int maxId = 0;

    while (fread(&l, sizeof(livre), 1, f)) {
        if (l.id > maxId) {
            maxId = l.id;
        }
    }

    fclose(f);
    return maxId + 1;
}


int genererIdUtilisateur() {
    FILE *f = fopen("utilisateurs.txt", "rb");
    if (!f) return 1;

    utilisateur u;
    int maxId = 0;

    while (fread(&u, sizeof(utilisateur), 1, f)) {
        if (u.id > maxId){
            maxId = u.id;
        }
    }

    fclose(f);
    return maxId + 1;
}

int main() {
    int choix;
    do {
        printf("\n=== MENU BIBLIOTHÈQUE ===\n");
        printf("1. Ajouter un livre\n");
        printf("2. Supprimer un livre\n");
        printf("3. Rechercher un livre\n");
        printf("4. Lister tous les livres\n");
        printf("5. Enregistrer un utilisateur\n");
        printf("6. Emprunter un livre\n");
        printf("7. Retourner un livre\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: ajouterLivre(); break;
            case 2: supprimerLivre(); break;
            case 3: rechercherLivre(); break;
            case 4: listerLivres(); break;
            case 5: enregistrerUtilisateur(); break;
            case 6: emprunterLivre(); break;
            case 7: retournerLivre(); break;
            case 0: printf("Au revoir !\n"); break;
            default: printf("Choix invalide.\n");
        }
    } while (choix != 0);

    return 0;
}
