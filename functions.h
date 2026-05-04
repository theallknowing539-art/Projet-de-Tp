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

void entrerStock(int ref, int quant) {
    FILE *f = fopen("stock.dat","r+b");
    bool found;
    Produit p;
    Mouvement m;


    if(f == NULL) {
        printf("error opening files");
    }
    time_t now;
    
    struct tm *t;

    time(&now);               
    t = localtime(&now);


    while (fread(&p, sizeof(Produit), 1, f) == 1) {
        if (p.reference == ref) {
            found = 1;

            // Prepare Movement data for the journal
            m.reference = ref;
            strcpy(m.operation, "Ajout");
            m.quantite_concernee = quant;
            m.stock_avant = p.quantite;
            m.stock_apres = p.quantite + quant;
            
            // Set date using the lib time.h
            strftime(m.date, sizeof(m.date), "%d/%m/%Y", t);

            // Update the Product struct
            p.quantite = m.stock_apres;
            strcpy(p.derniere_operation, "Ajout");

            // Move pointer back to overwrite this specific record
            fseek(f, -sizeof(Produit), SEEK_CUR);
            fwrite(&p, sizeof(Produit), 1, f);

            // Write to movements.dat
            FILE *fJournal = fopen("mouvements.dat", "ab");
            if (fJournal != NULL) {
                fwrite(&m, sizeof(Mouvement), 1, fJournal);
                fclose(fJournal);
            }

            printf("Stock mis a jour et mouvement enregistre.\n");
            break; 
        }
    }

    if (!found) {
        printf("Erreur : Reference %d introuvable.\n", ref);
    }

    fclose(f);
}


