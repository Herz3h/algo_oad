#ifndef DEF_PRODUIT
#define DEF_PRODUIT

#include <iostream>
#include <map>
#include <vector>
#include "csv.h"
#include <cmath>
#include <fstream>

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
#define JOURS_OUVRES_PAR_SEMAINE 5
#define NB_PAS 4
#define NB_PASSAGE 100
#define NB_TOP_CANIDATS 5
#define TAILLE_LISTE_TABOU 1000
#define NB_ESSAI_SANS_AMELIORATION_MAX 100
#define BORNE_QUANTITE_VOISINS 0.4
#define BORNE_STOCK_ALERT_VOISINS 0.4
#define NB_VOISINS 1000
#define TOLERANCE_RUPTURE 2.36
#define FREQUENCE_EXPLORATION 10
#define FREQUENCE_EXPLOITATION 5
#define RATIO_EXPLORATION 0.002
#define RATIO_EXPLOITATION 0.0

struct coupleDemandeEffectif {
	int demande;
	int nbEffectif;
};

struct candidat {
    int quantite;
    int stockAlerte;
    float coutTotal;
};

struct mois {
    int premierJour;
    int nbJoursOuvres;
} ;

struct stockAlerte_coutTotal
{
    int stockAlerte;
    int coutTotal;
};

class Produit 
{
    private:
    int jour;
    int jourIndexZero; // usage interne

    vector<int>   demandeJournalieres;
    vector<int>   stocks;
    vector<float> ruptures;
    vector<float> coutsStockage;
    vector<int>   quantiteesCommandees;
    vector<int>   quantiteesRecues;
    vector<int>   coutsLancement;
    vector<int>   stocksFictif;

    string produit, categorie;
    float tauxSaisonnalite, tauxPossession, pourcentagePerte;
    int stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, 
        variationJournaliere, delaiLivraison, coutLancement, prixAchat, demandeJournaliereInitiale;

    static struct mois joursOuvresParMois[12];

    float csvFloatToCppFloat(string csvFloat);

    int bestCoutTotalBorneMin, bestCoutTotalBorneMax, bestMinStockAlerte, bestMaxStockAlerte,  borneMin, borneMax;
    int bestStockAlertePassage, bestTotalAnneePassage;
    int pas[NB_PAS];
    vector<struct stockAlerte_coutTotal > top_stockAlert_coutTotal;
    vector<struct candidat > listeTabou;
    int indexListeTabou;
    struct candidat meilleurCandidatGlobal;
    int nbEssaiSansAmelioration;
    int stockAlerteTheorique;

    // Constantes
    int premierJourCommande;

    // Parametres
    int quantite;
    int stockAlerte;

    public:
    Produit(string _p, string _c, int _sD, int _dJ, int _cJ, int _mS, int _vJ, int _dL, int _cL, int _pA, string _tS, string _tP, string _pP);
    void  incJour();
    bool  isInSaison();
    void  computeData();
    int   getDemandeJournaliere();
    int   getStockJournalier();
    float getRuptureJournaliere();
    float getCoutStockageJournalier();
    float getTotalJournalier();
    int   getQuantiteesCommandeesJournalier();
    int   getQuantiteesRecuesJournalier();
    int   getCoutLancementJournalier();
    int   getStockFictifJournalier();
    void  setDemandeJournaliereActuelle();
    float getTotalAnnee();
    void  findMeilleurBornes();
    void  resetJour();
    void  resetDemandeJournaliere();
    void retenirMeilleurVoisin(vector<struct candidat> &candidats);
    void doPassage();
    bool isTabou(struct candidat c);
    void compareResultatGlobal(struct candidat &voisin);
    void addListeTabou(struct candidat &voisin);
    void getMoinsPireCandidat(vector<struct stockAlerte_coutTotal> &sa_ct);
    vector<struct stockAlerte_coutTotal> getMeilleurCandidat();

    vector<struct candidat> genererVoisins(struct candidat);
    int randInt(int a, int b);
    void setStockAlerteTheorique();
};

#endif 
