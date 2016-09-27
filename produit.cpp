#include "produit.hpp"

struct mois Produit::joursOuvresParMois[12] = {{1, 23}, {24, 20}, {44, 22}, {66, 21}, {87, 23}, {110, 21}, {131, 22}, {153, 23}, {176, 20}, {196, 23}, {219, 22}, {241, 21}};

Produit::Produit(string _p, string _c, int _sD, int _dJ, int _cJ, int _mS, int _vJ, int _dL, int _cL, int _pA, string _tS, string _tP, string _pP) : 
        produit(_p), categorie(_c), stockDepart(_sD), demandeJournaliere(_dJ),
        croissanceJournaliere(_cJ), moisSaisonnalite(_mS), variationJournaliere(_vJ), delaiLivraison(_dL), coutLancement(_cL),
        prixAchat(_pA), 
        demandeJournalieres(NB_JOURS_OUVRES_ANNEE), stocks(NB_JOURS_OUVRES_ANNEE), rutpures(NB_JOURS_OUVRES_ANNEE),
        coutsStockage(NB_JOURS_OUVRES_ANNEE), totalJournalier(NB_JOURS_OUVRES_ANNEE), joursCommande(NB_JOURS_OUVRES_ANNEE),
        joursReception(NB_JOURS_OUVRES_ANNEE), stocksFictif(NB_JOURS_OUVRES_ANNEE),
        jour(1)
{
    replace(_tS.begin(), _tS.end(), ',', '.');
    replace(_tP.begin(), _tP.end(), ',', '.');
    replace(_pP.begin(), _pP.end(), ',', '.');
    tauxSaisonnalite = atof(_tS.c_str());
    tauxPossession = atof(_tP.c_str());
    pourcentagePerte = atof(_pP.c_str());
    computeData();

}


void Produit::computeData()
{
    for(int i=0; i < NB_JOURS_OUVRES_ANNEE;i++)
    {
        int d = getDemandeJournaliere();
        demandeJournalieres.push_back(d);
        if(isInSaison())
            cout << d << endl;
        incJour();
    }

}

bool Produit::isInSaison()
{
    struct mois m = joursOuvresParMois[moisSaisonnalite-1]; // -1 indexés à 1 au lieu de 0
    if(jour >= m.premierJour && jour <= (m.premierJour + m.nbJoursOuvres)) 
    {

        /* cout << jour << " - mois " << m.premierJour << " " << m.premierJour+m.nbJoursOuvres << endl; */
        return true;
    }

    return false;
}

// Modifie directement demande Journaliere
void Produit::setDemandeJournaliereActuelle()
{
    demandeJournaliere += croissanceJournaliere;
}

int Produit::getDemandeJournaliere()
{
    int maxDemandeJournaliere = 0;
    int minDemandeJournaliere = 0;

    if(isInSaison())
    {
        cout << tauxSaisonnalite  << " " << demandeJournaliere << endl;
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

void Produit::incJour()
{
    jour++;
}
