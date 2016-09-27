#ifndef DEF_PRODUIT
#define DEF_PRODUIT

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

struct mois {
    int premierJour;
    int nbJoursOuvres;
} ;

class Produit 
{
    private:
    int jour;

    vector<int>   demandeJournalieres;
    vector<int>   stocks;
    vector<float> rutpures;
    vector<float> coutsStockage;
    vector<float> totalJournalier;
    vector<int>   joursCommande;
    vector<int>   joursReception;
    vector<int>   stocksFictif;

    string produit, categorie;
    float tauxSaisonnalite, tauxPossession, pourcentagePerte;
    int stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, prixAchat;

    static struct mois joursOuvresParMois[12];

    public:
    Produit(string _p, string _c, int _sD, int _dJ, int _cJ, int _mS, int _vJ, int _dL, int _cL, int _pA, string _tS, string _tP, string _pP);
    void computeData();
    int getDemandeJournaliere();
    bool isInSaison();
    void incJour();
    void setDemandeJournaliereActuelle();
};

#endif 
