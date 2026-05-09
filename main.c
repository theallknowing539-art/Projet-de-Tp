
#include "functions.h"

int main(void)
{
    int choix;
    int ref;
    int quant;

    do {

        printf("\n");
        printf("====================================================\n");
        printf("           GESTIONNAIRE DE STOCK EN C\n");
        printf("====================================================\n");
        printf(" 1. Ajouter un produit\n");
        printf(" 2. Afficher le stock\n");
        printf(" 3. Entree de stock\n");
        printf(" 4. Sortie de stock\n");
        printf(" 5. Consulter le journal\n");
        printf(" 6. Quitter\n");
        printf("====================================================\n");

        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {

            case 1:

                printf("\n=== AJOUT PRODUIT ===\n");

                printf("Reference : ");
                scanf("%d", &ref);

                printf("Quantite initiale : ");
                scanf("%d", &quant);

                addProduct(ref, quant);

                break;

            case 2:

                printf("\n=== AFFICHAGE DU STOCK ===\n");

                afficherStock();

                break;

            case 3:

                printf("\n=== ENTREE DE STOCK ===\n");

                printf("Reference : ");
                scanf("%d", &ref);

                printf("Quantite a ajouter : ");
                scanf("%d", &quant);

                entrerStock(ref, quant);

                break;

            case 4:

                printf("\n=== SORTIE DE STOCK ===\n");

                printf("Reference : ");
                scanf("%d", &ref);

                printf("Quantite a retirer : ");
                scanf("%d", &quant);

                sortirStock(ref, quant);

                break;

            case 5:

                printf("\n=== JOURNAL DES MOUVEMENTS ===\n");

                consulterJournal();

                break;

            case 6:

                printf("\nMerci d'avoir utilise le programme.\n");

                break;

            default:

                printf("\nChoix invalide.\n");
        }

    } while (choix != 6);

    return 0;
}
