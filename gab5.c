#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define FICHIER_UTILISATEURS "utilisateurs.txt"

// Définition de la structure d'un utilisateur
typedef struct {
    char id[20];
    char mot_de_passe[20];
    int solde;
    int compte_bloque; // 0 = non bloqué, 1 = bloqué
} Utilisateur;

// Fonction pour charger les utilisateurs depuis le fichier
int charger_utilisateurs(Utilisateur utilisateurs[], int taille_max) {
    FILE *fichier = fopen(FICHIER_UTILISATEURS, "rb");
    if (!fichier) {
        return 0;
    }
    int nombre_utilisateurs = fread(utilisateurs, sizeof(Utilisateur), taille_max, fichier);
    fclose(fichier);
    return nombre_utilisateurs;
}

// Fonction pour sauvegarder les utilisateurs dans le fichier
void sauvegarder_utilisateurs(Utilisateur utilisateurs[], int taille) {
    FILE *fichier = fopen(FICHIER_UTILISATEURS, "wb");
    if (!fichier) {
        return;
    }
    fwrite(utilisateurs, sizeof(Utilisateur), taille, fichier);
    fclose(fichier);
}

// Fonction pour vérifier si une chaîne est un nombre entier
int est_entier(const char* chaine) {
    char* p;
    strtol(chaine, &p, 10);
    return *p == '\0';
}

// Fonction pour vérifier si une chaîne est un nombre décimal
int est_decimal(const char* chaine) {
    int points = 0;
    for (int i = 0; chaine[i] != '\0'; i++) {
        if (chaine[i] == '.') {
            points++;
        } else if (!isdigit(chaine[i])) {
            return 0; // Caractère non numérique
        }
    }
    return points == 1; // Doit y avoir exactement un point décimal
}

// Fonction pour débloquer un utilisateur
void debloquer_utilisateur(Utilisateur utilisateurs[], int *taille, const char *id_utilisateur) {
    for (int i = 0; i < *taille; i++) {
        if (strcmp(utilisateurs[i].id, id_utilisateur) == 0) {
            if (utilisateurs[i].compte_bloque) {
                utilisateurs[i].compte_bloque = 0; // Débloquer le compte
                sauvegarder_utilisateurs(utilisateurs, *taille);
                printf("L'utilisateur %s a été débloqué.\n", id_utilisateur);
            } else {
                printf("L'utilisateur %s n'est pas bloqué.\n", id_utilisateur);
            }
            return;
        }
    }
    printf("Utilisateur %s non trouvé.\n", id_utilisateur);
}

int main() {
    // Initialiser la liste des utilisateurs
    Utilisateur utilisateurs[100];
    int taille_utilisateurs = charger_utilisateurs(utilisateurs, sizeof(utilisateurs) / sizeof(utilisateurs[0]));

    if (taille_utilisateurs == 0) {
        // Si le fichier est vide ou ne peut pas être ouvert, initialiser des utilisateurs par défaut
        utilisateurs[0] = (Utilisateur){"user1", "password1", 50000, 0};
        utilisateurs[1] = (Utilisateur){"user2", "password2", 30000, 0};
        utilisateurs[2] = (Utilisateur){"user3", "password3", 45000, 0};
        utilisateurs[3] = (Utilisateur){"user4", "password4", 20000, 0};
        utilisateurs[4] = (Utilisateur){"user5", "password5", 35000, 0};
        taille_utilisateurs = 5;
        sauvegarder_utilisateurs(utilisateurs, taille_utilisateurs);
    }

    while (1) {
        int choix;
        char identifiant[20];
        char mot_de_passe[20];
        Utilisateur utilisateur_actuel;

        // Menu principal
        printf("Menu Principal\n");
        printf("1. Connexion Utilisateur\n");
        printf("2. Débloquer un Utilisateur (Admin)\n");
        printf("3. Quitter\n");
        printf("Entrez votre choix : ");
        if (scanf("%d", &choix) != 1) {
            printf("Entrée invalide. Veuillez entrer un nombre.\n");
            while (getchar() != '\n'); // Vider le buffer d'entrée
            continue;
        }

        switch (choix) {
            case 1:
                // Connexion utilisateur
                printf("Entrez votre identifiant : ");
                scanf("%19s", identifiant);

                // Vérifier si l'identifiant existe et si le compte n'est pas bloqué
                int utilisateur_trouve = 0;
                for (int i = 0; i < taille_utilisateurs; i++) {
                    if (strcmp(utilisateurs[i].id, identifiant) == 0) {
                        utilisateur_actuel = utilisateurs[i];
                        utilisateur_trouve = 1;
                        break;
                    }
                }

                if (!utilisateur_trouve) {
                    printf("Identifiant incorrect.\n");
                    continue;
                }

                if (utilisateur_actuel.compte_bloque) {
                    printf("Compte bloqué. Veuillez contacter l'assistance.\n");
                    continue;
                }

                // Demander le mot de passe
                int mot_de_passe_correct = 0;
                for (int tentatives = 0; tentatives < 3; tentatives++) {
                    printf("Entrez votre mot de passe : ");
                    scanf("%19s", mot_de_passe);

                    if (strcmp(utilisateur_actuel.mot_de_passe, mot_de_passe) == 0) {
                        mot_de_passe_correct = 1;
                        break;
                    } else {
                        if (tentatives < 2) {
                            printf("Mot de passe incorrect. Il vous reste %d tentatives.\n", 2 - tentatives);
                        }
                    }
                }

                if (!mot_de_passe_correct) {
                    // Marquer le compte comme bloqué
                    for (int i = 0; i < taille_utilisateurs; i++) {
                        if (strcmp(utilisateurs[i].id, identifiant) == 0) {
                            utilisateurs[i].compte_bloque = 1;
                            sauvegarder_utilisateurs(utilisateurs, taille_utilisateurs);
                            printf("Compte bloqué après plusieurs tentatives échouées.\n");
                            break;
                        }
                    }
                    continue;
                }

                // Menu utilisateur
                while (1) {
                    int choix_utilisateur;
                    int montant;

                    // Afficher le menu des options
                    printf("Menu Utilisateur\n");
                    printf("1. Consulter le solde\n");
                    printf("2. Retrait\n");
                    printf("3. Quitter\n");
                    printf("Entrez votre choix : ");
                    if (scanf("%d", &choix_utilisateur) != 1) {
                        printf("Entrée invalide. Veuillez entrer un nombre.\n");
                        while (getchar() != '\n'); // Vider le buffer d'entrée
                        continue;
                    }

                    switch (choix_utilisateur) {
                        case 1:
                            printf("Votre solde est de %d\n", utilisateur_actuel.solde);
                            break;

                        case 2:
                            do {
                                printf("Choisissez le montant du retrait\n");
                                printf("1. Retrait de 10 000\n");
                                printf("2. Retrait de 50 000\n");
                                printf("3. Autre montant\n");
                                printf("4. Quitter\n");
                                printf("Entrez votre choix : ");
                                int choix_retrait;
                                if (scanf("%d", &choix_retrait) != 1) {
                                    printf("Entrée invalide. Veuillez entrer un nombre.\n");
                                    while (getchar() != '\n'); // Vider le buffer d'entrée
                                    continue;
                                }
                                
                                int montant_valide = 0;
                                switch (choix_retrait) {
                                    case 1:
                                        montant = 10000;
                                        montant_valide = 1;
                                        break;
                                    case 2:
                                        montant = 50000;
                                        montant_valide = 1;
                                        break;
                                    case 3:
                                        printf("Entrez le montant à retirer (minimum 1000, multiple de 1000) : ");
                                        char montant_str[20];
                                        scanf("%s", montant_str);

                                        if (est_decimal(montant_str) || !est_entier(montant_str)) {
                                            printf("Montant invalide. La somme doit être un entier sans décimales et un multiple de 1000.\n");
                                            continue;
                                        }

                                        montant = atoi(montant_str);
                                        if (montant < 1000 || montant % 1000 != 0) {
                                            printf("Montant invalide. La somme doit être un multiple de 1000.\n");
                                            continue;
                                        }
                                        montant_valide = 1;
                                        break;
                                    case 4:
                                        printf("Retour au menu utilisateur.\n");
                                        montant_valide = 2; // Utiliser une valeur spéciale pour indiquer "quitter"
                                        break;
                                    default:
                                        printf("Choix invalide.\n");
                                        break;
                                }

                                if (montant_valide == 1) {
                                    if (utilisateur_actuel.solde >= montant) {
                                        utilisateur_actuel.solde -= montant;
                                        printf("Retrait de %d réussi. Solde restant : %d\n", montant, utilisateur_actuel.solde);
                                        break;
                                    } else {
                                        printf("Solde insuffisant pour retirer %d. Veuillez recharger votre compte.\n", montant);
                                        break;
                                    }
                                } else if (montant_valide == 2) {
                                    break; // Quitter la boucle de retrait et retourner au menu utilisateur
                                }
                            } while (1);
                            break;

                        case 3:
                            printf("Déconnexion réussie.\n");
                            break;

                        default:
                            printf("Choix invalide. Veuillez sélectionner une option valide.\n");
                            break;
                    }

                    if (choix_utilisateur == 3) {
                        break;
                    }
                }
                break;

            case 2:
                // Déblocage utilisateur (Admin)
                printf("Entrez le mot de passe admin : ");
                char mot_de_passe_admin[20];
                scanf("%19s", mot_de_passe_admin);
                
                if (strcmp(mot_de_passe_admin, "admin") == 0) {
                    printf("Entrez l'identifiant de l'utilisateur à débloquer : ");
                    scanf("%19s", identifiant);
                    debloquer_utilisateur(utilisateurs, &taille_utilisateurs, identifiant);
                } else {
                    printf("Mot de passe admin incorrect. Accès refusé.\n");
                }
                break;

            case 3:
                printf("Quitter l'application.\n");
                return 0;

            default:
                printf("Choix invalide. Veuillez sélectionner une option valide.\n");
                break;
        }
    }

    return 0;
}