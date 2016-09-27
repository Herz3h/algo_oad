#include <iostream>
#include <map>
#include <vector>
#include "csv.h"

using namespace std;
using namespace io;

#define ID_DEM_JOUR 0
#define ID_STOCK 1
#define ID_RUPTURE 2
#define ID_COUT_STOCKAGE 3
#define ID_TOTAL_JOURNALIER 4
#define ID_COMMANDE 5
#define ID_RECEPTION 6
#define ID_FRAIS_LANCEMENT 7
#define ID_STOCK_FICTIF 8

#define NB_JOURS_OUVRES_ANNEE 261

struct produit {
    int demandeJournaliere;
    int stock;
    float rutpure;
    float coutStockage;
    float totalJournalier;
    int jourCommande;
    int jourReception;
    int stockFictic;
} typedef produit;

struct mois {
    int premierJour;
    int nbJoursOuvres;
} ;

struct mois joursOuvresParMois[12] = { 
{1, 23}, {24, 20}, {44, 22}, {66, 21}, {87, 23}, {110, 21}, {131, 22}, {153, 23}, {176, 20}, {196, 23}, {219, 22}, {241, 21},
};

int getDemandeJournaliere(int jour, int demandeJournaliere, int variationJournaliere, int moisSaisonnalite, float tauxSaisonnalite, int croissanceJournaliere);

int main()
{

    // Initialisation
    io::CSVReader<13, io::trim_chars<' '>, io::no_quote_escape<';'> > in("donneesventes.csv");
    in.read_header(io::ignore_no_column, "Produit", "Categorie", "Stock_de_depart", "Demande_d1", "Croissance_par_jour", "Mois_de_saisonnalite", "Saisonnalite", "Variation_journaliere", "Delai_livraison", "Cout_de_lancement", "Taux_de_possession", "Prix_d_achat", "Pourcentage_perte");
    string produit, categorie;

    // Variable pour les colonnes
    int stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, prixAchat;
    float tauxSaisonnalite, tauxPossession, pourcentagePerte;

    map<int, vector<struct produit> > tableauResultats;

    int jour = 1;
    // Récuperation des données par ligne (1produit/ligne)
    //while(in.read_row(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, tauxSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, tauxPossession, prixAchat, pourcentagePerte)){
    in.read_row(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, tauxSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, tauxPossession, prixAchat, pourcentagePerte);

    for(int i=0; i < NB_JOURS_OUVRES_ANNEE;i++)
    {
        struct produit p;
        p.demandeJournaliere = getDemandeJournaliere(jour, demandeJournaliere, variationJournaliere, moisSaisonnalite, tauxSaisonnalite, croissanceJournaliere);
        cout << p.demandeJournaliere << endl;
        jour ++;
        
    }

}

bool isInSaison(int jour, int moisSaisonnalite)
{
    struct mois m= joursOuvresParMois[moisSaisonnalite-1]; // -1 indexés à 1 au lieu de 0
    cout << jour << " - mois " << m.premierJour << " " << m.premierJour+m.nbJoursOuvres << endl;
    if(jour >= m.premierJour && jour <= (m.premierJour + m.nbJoursOuvres)) 
        return true;

    return false;
}

// Modifie directement demande Journaliere
void setDemandeJournaliereActuelle(int jour, int &demandeJournaliere, int croissanceJournaliere)
{
    demandeJournaliere = demandeJournaliere + croissanceJournaliere;
}

int getDemandeJournaliere(int jour, int demandeJournaliere, int variationJournaliere, int moisSaisonnalite, float tauxSaisonnalite, int croissanceJournaliere)
{
    int maxDemandeJournaliere = 0;
    int minDemandeJournaliere = 0;

    if(isInSaison(jour, moisSaisonnalite))
    {
        maxDemandeJournaliere = demandeJournaliere * (1.0 + tauxSaisonnalite) + variationJournaliere;
        minDemandeJournaliere = demandeJournaliere * (1.0 + tauxSaisonnalite) - variationJournaliere;

    }
    else 
    {
        maxDemandeJournaliere = demandeJournaliere + variationJournaliere;
        minDemandeJournaliere = demandeJournaliere - variationJournaliere;
    }


    return rand()%(maxDemandeJournaliere-minDemandeJournaliere + 1) + minDemandeJournaliere;

}
