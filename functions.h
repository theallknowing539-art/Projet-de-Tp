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


//Ayoub 
void sortirStock(int ref,int quant) {
    FILE *f = fopen("stock.dat", "rb+"); // to open stock file
    FILE *m = fopen("mouvements.dat", "ab"); // to open journal file

    Produit p;
    Mouvement mv;

    // int ref, qte;

    printf("Donner la reference: ");
    scanf("%d", &ref);

    // to search for the product in file
    while (fread(&p, sizeof(p), 1, f)) {

        if (p.reference == ref) {

            // printf("Donner la quantite a retirer: ");
            // scanf("%d", &qte);
            

            // to check if enough stock
            if (quant > p.quantite) {
                printf("Stock insuffisant\n");
                fclose(f);
                fclose(m);
                return;
            }

            // to save old quantity
            mv.stock_avant = p.quantite;

            // to update stock
            p.quantite = p.quantite - quant;

            // to update last operation
            strcpy(p.derniere_operation, "Retrait");

            // to go back in file to overwrite product
            fseek(f, -sizeof(p), SEEK_CUR);
            fwrite(&p, sizeof(p), 1, f);

            // to fill movement info
            printf("Donner la date: ");
            scanf("%s", mv.date);

            mv.reference = ref;
            strcpy(mv.operation, "Retrait");
            mv.quantite_concernee = quant;
            mv.stock_apres = p.quantite;

            // to write movement in journal
            fwrite(&mv, sizeof(mv), 1, m);

            // to check if stock is under seuil
            if (p.quantite < p.seuil_alerte) {
                printf("ALERTE: stock sous le seuil\n");
            }

            printf("Operation faite\n");

            fclose(f);
            fclose(m);
            return;
        }
    }

    // to handle case where product not found
    printf("Produit non trouve\n");

    fclose(f);
    fclose(m);
}


//islem consulter journal 
void consulterJournal() {
    FILE *f = fopen("mouvements.dat", "rb");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le journal des mouvements.\n");
        return;
    }

    Mouvement m;
    int count = 0;

    printf("\n========== JOURNAL DES MOUVEMENTS ==========\n");
    printf("%-12s %-10s %-10s %-18s %-12s %-12s\n",
           "Date", "Reference", "Operation", "Quantite concernee", "Stock avant", "Stock apres");
    printf("---------------------------------------------------------------------\n");

    while (fread(&m, sizeof(Mouvement), 1, f) == 1) {
        printf("%-12s %-10d %-10s %-18d %-12d %-12d\n",
               m.date,
               m.reference,
               m.operation,
               m.quantite_concernee,
               m.stock_avant,
               m.stock_apres);
        count++;
    }

    if (count == 0) {
        printf("Aucun mouvement enregistre.\n");
    } else {
        printf("---------------------------------------------------------------------\n");
        printf("Total : %d mouvement(s) affiche(s).\n", count);
    }

    fclose(f);
}