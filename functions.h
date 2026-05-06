#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include <time.h>

//structure declarations

// Structure pour le stock ( fichier binaire )
typedef struct {
int reference ;
char designation [80];
int quantite ;
int seuil_alerte ;
char derniere_operation [20]; // " Ajout " , " Retrait " , " Initial "
} Produit ;

// Structure pour le journal ( fichier binaire )
typedef struct {
char date [11];
// JJ / MM / AAAA
int reference ;
char operation [10];
// " Ajout " ou " Retrait "
int quantite_concernee ;
int stock_avant ;
int stock_apres ;
} Mouvement ;
//functions

void addProduct(int ref, int quant) {
    FILE *f;
    Produit p;
    Produit temp;
    Mouvement m;
    int existe = 0;

    time_t now;
    struct tm *t;
    time(&now);
    t = localtime(&now);

    // Get product information from user 
    // printf("Reference : ");
    // scanf("%d", &p.reference);
    p.reference = ref;

    printf("Designation : ");
    scanf(" %[^\n]", p.designation);

    // printf("Quantite initiale : ");
    // scanf("%d", &p.quantite);
    p.quantite = quant;
    printf("Seuil d'alerte : ");
    scanf("%d", &p.seuil_alerte);

    strcpy(p.derniere_operation, "Initial");

    //Check if reference already exists in stock.dat 
    f = fopen("stock.dat", "rb");
    //error check 0
       if (f == NULL) {
        printf("error opening files");
        return;
    }
    if (f != NULL) {
        while (fread(&temp, sizeof(Produit), 1, f) == 1) {
            if (temp.reference == p.reference) {
                existe = 1;
                break;
            }
        }
        fclose(f);
    }

    if (existe) {
        printf("Error: reference %d existe deja.\n", p.reference);
        return;
    }

    //Write new product to stock.dat 
    f = fopen("stock.dat", "ab");
    //error check 1

    if (f == NULL) {
        printf("error opening files");
        return;
    }
    fwrite(&p, sizeof(Produit), 1, f);
    fclose(f);

    // Build and write movement entry to mouvements.dat 
    m.reference = p.reference;
    strcpy(m.operation, "Ajout");
    m.quantite_concernee = p.quantite;
    m.stock_avant = 0;
    m.stock_apres = p.quantite;
    strftime(m.date, sizeof(m.date), "%d/%m/%Y", t);

    FILE *fJournal = fopen("mouvements.dat", "ab");
    if (fJournal != NULL) {
        fwrite(&m, sizeof(Mouvement), 1, fJournal);
        fclose(fJournal);
    }

    printf("Produit ajouté avec succés.\n");
}


void entrerStock(int ref, int quant) {
    FILE *f = fopen("stock.dat", "r+b"); // Open in update mode
    int found = 0; // Initialized to avoid garbage memory
    Produit p;
    Mouvement m;

    if (f == NULL) {
        printf("Error: stock.dat could not be opened.\n");
        return; 
    }

    time_t now;
    struct tm *t;
    time(&now);               
    t = localtime(&now);

    while (fread(&p, sizeof(Produit), 1, f) == 1) { // Process one record at a time
        if (p.reference == ref) {
            found = 1;

            m.reference = ref;
            strcpy(m.operation, "Ajout"); 
            m.quantite_concernee = quant;
            m.stock_avant = p.quantite;
            m.stock_apres = p.quantite + quant;
            
            strftime(m.date, sizeof(m.date), "%d/%m/%Y", t);

            p.quantite = m.stock_apres;
            strcpy(p.derniere_operation, "Ajout"); 

            fseek(f, -sizeof(Produit), SEEK_CUR); // Move back to the record start
            fwrite(&p, sizeof(Produit), 1, f);

            FILE *fJournal = fopen("mouvements.dat", "ab"); // Link to journal
            if (fJournal != NULL) {
                fwrite(&m, sizeof(Mouvement), 1, fJournal);
                fclose(fJournal);
            }

            printf("Stock mis a jour.\n");
            break; 
        }
    }

    if (!found) {
        printf("Erreur : Reference %d introuvable.\n", ref);
    }

    fclose(f); 
}

//walidAfficherStock
void afficherStock() {
    FILE *f = fopen("stock.dat", "rb");
    FILE *m = fopen("mouvements.dat", "rb");

    Produit p;
    Mouvement mv;

    if (f == NULL) {
        printf("Stock vide\n");
        return;
    }

    while (fread(&p, sizeof(p), 1, f)) {

        int count = 0;

        if (m != NULL) {
            rewind(m);
            while (fread(&mv, sizeof(mv), 1, m)) {
                if (mv.reference == p.reference) {
                    count++;
                }
            }
        }

        printf("\nRef: %d\n", p.reference);
        printf("Nom: %s\n", p.designation);
        printf("Qte: %d\n", p.quantite);
        printf("Derniere op: %s\n", p.derniere_operation);
        printf("Nb mouvements: %d\n", count);
    }

    fclose(f);
    if (m != NULL) fclose(m);
}


