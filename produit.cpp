#include "produit.hpp"

struct mois Produit::joursOuvresParMois[12] = {{1, 23}, {24, 20}, {44, 22}, {66, 21}, {87, 23}, {110, 21}, {131, 22}, {153, 23}, {176, 20}, {196, 23}, {219, 22}, {241, 21}};

Produit::Produit(string _p, string _c, int _sD, int _dJ, int _cJ, int _mS, int _vJ, int _dL, int _cL, int _pA, string _tS, string _tP, string _pP) : 
        produit(_p), categorie(_c), stockDepart(_sD), demandeJournaliere(_dJ),
        croissanceJournaliere(_cJ), moisSaisonnalite(_mS), variationJournaliere(_vJ), delaiLivraison(_dL), coutLancement(_cL),
        prixAchat(_pA), demandeJournaliereInitiale(_dJ),
        demandeJournalieres(NB_JOURS_OUVRES_ANNEE), stocks(NB_JOURS_OUVRES_ANNEE), ruptures(NB_JOURS_OUVRES_ANNEE),
        coutsStockage(NB_JOURS_OUVRES_ANNEE),  quantiteesCommandees(NB_JOURS_OUVRES_ANNEE), quantiteesRecues(NB_JOURS_OUVRES_ANNEE),
        stocksFictif(NB_JOURS_OUVRES_ANNEE), coutsLancement(NB_JOURS_OUVRES_ANNEE),
        jour(1),
        jourIndexZero(0),
        stockAlerte(10000),
        borneMin(0), borneMax(30000)
{
    tauxSaisonnalite = 1.0 + csvFloatToCppFloat(_tS);
    tauxPossession =  csvFloatToCppFloat(_tP);
    pourcentagePerte =  csvFloatToCppFloat(_pP);

    pas[0] = 1000;
    pas[1] = 100;
    pas[2] = 10;
    pas[3] = 1;

    int besoinAnnuel = demandeJournaliere * NB_JOURS_OUVRES_ANNEE - stockDepart;
    quantite = sqrt((2 * besoinAnnuel * (float)coutLancement) / (tauxPossession * (float)prixAchat));
    cout << "Quantite : " << quantite << endl;

    /* computeData(); */
    /* cout << getTotalAnnee() << endl; */


    doPassage();
    vector<struct stockAlerte_coutTotal> sa_ct = getMeilleurCandidat();
    getMoinsPireCandidat(sa_ct);

    cout << bestStockAlertePassage << endl;

}


void Produit::doPassage()
{
    for(int i = 0;i < NB_PASSAGE;i++)
    {

        bestStockAlertePassage = 0;
        bestTotalAnneePassage = 0;

        findBestSockAlerte();

        int bestBorne = 0;
        if(bestCoutTotalBorneMin < bestCoutTotalBorneMax)
            bestBorne = borneMin;
        else
            bestBorne = borneMax;

        struct stockAlerte_coutTotal sa_ct = {bestBorne, min(bestCoutTotalBorneMin, bestCoutTotalBorneMax)};
        top_stockAlert_coutTotal.push_back(sa_ct);
    }
}

vector<struct stockAlerte_coutTotal> Produit::getMeilleurCandidat()
{

    // Classement croissant selon le coutTotal
    sort(top_stockAlert_coutTotal.begin(), top_stockAlert_coutTotal.end(), 
            [](const struct stockAlerte_coutTotal &A, const struct stockAlerte_coutTotal &B) -> bool {
            return A.coutTotal > B.coutTotal;
    });

    vector<struct stockAlerte_coutTotal> sa_ct;
    for(int i=0;i < NB_TOP_CANIDATS;i++)
    {
        sa_ct.push_back(top_stockAlert_coutTotal[i]);
    }

    return sa_ct;
}

void Produit::getMoinsPireCandidat(vector<struct stockAlerte_coutTotal> &meilleurCandidats)
{
    vector< vector<struct stockAlerte_coutTotal> > resultatsPassagesCandidat(NB_TOP_CANIDATS);

    for(int i=0;i < NB_TOP_CANIDATS;i++)
    {
        stockAlerte = meilleurCandidats[i].stockAlerte;
        for(int j=0;j < NB_PASSAGE;j++)
        {
            computeData();

        }
    }
}

void Produit::findBestSockAlerte()
{
    for(int j = 0;j < NB_PAS;j++)
    {
        bestCoutTotalBorneMin = 0;
        bestCoutTotalBorneMax = 0;

        for(int i = borneMin; i < borneMax; i += pas[j])
        {
            resetJour();
            resetDemandeJournaliere();
            stockAlerte = i;
            computeData();
            int totalAnnee = getTotalAnnee();


        }

        borneMin = bestMinStockAlerte;
        borneMax = bestMaxStockAlerte;
    }
    /* cout << "BestMinStockAlerte : " << bestMinStockAlerte << " bestMaxStockAlerte" << bestMaxStockAlerte << endl; */

}

void Produit::get()
{
    int totalAnnee = getTotalAnnee();
    if(totalAnnee < bestCoutTotalBorneMin || bestCoutTotalBorneMin == 0)
    {
        bestMinStockAlerte = i;
        bestCoutTotalBorneMin = totalAnnee;
    }
    else if(totalAnnee < bestCoutTotalBorneMax || bestCoutTotalBorneMax == 0)
    {
        bestMaxStockAlerte = i;
        bestCoutTotalBorneMax = totalAnnee;
    }
}

void Produit::computeData()
{
    /* cout << "Demande" << "\t" << "Stock" << "\t" << "Rupture" << "\t" << "CoutSt" << "\t" << "QtCmd" << "\t" << "QtRcpt" << "\t" << "StkFict" << "\t" << "CoutLanc" << "\t" << endl; */
    for(int i=0; i < NB_JOURS_OUVRES_ANNEE;i++)
    {
        int d = getDemandeJournaliere();
        demandeJournalieres[i] = d;

        int s = getStockJournalier();
        stocks[i] = s;

        float r = getRuptureJournaliere();
        ruptures[i] = r;

        float cs = getCoutStockageJournalier();
        coutsStockage[i] = cs;


        int qc = getQuantiteesCommandeesJournalier();
        quantiteesCommandees[i] = qc;

        int qr = getQuantiteesRecuesJournalier();
        quantiteesRecues[i] = qr;

        int sf = getStockFictifJournalier();
        stocksFictif[i] = sf;

        int cl = getCoutLancementJournalier();
        coutsLancement[i] = cl;

        /* cout.precision(2); */
        /* cout << d << "\t" << s << "\t" << r << "\t" << cs << "\t" << qc << "\t" << qr << "\t" << sf << "\t" << cl << "\t" << endl; */
        incJour();
    }

    /* cout << getTotalAnnee() << endl; */

}

bool Produit::isInSaison()
{
    struct mois m = joursOuvresParMois[moisSaisonnalite-1]; // -1 indexés à 1 au lieu de 0
    if(jour >= m.premierJour && jour < (m.premierJour + m.nbJoursOuvres)) { 
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
    setDemandeJournaliereActuelle();

    int maxDemandeJournaliere = 0;
    int minDemandeJournaliere = 0;

    if(isInSaison())
    {
        maxDemandeJournaliere = demandeJournaliere * tauxSaisonnalite + variationJournaliere;
        minDemandeJournaliere = demandeJournaliere * tauxSaisonnalite - variationJournaliere;
    }
    else 
    {
        maxDemandeJournaliere = demandeJournaliere + variationJournaliere;
        minDemandeJournaliere = demandeJournaliere - variationJournaliere;
    }


    return rand()%(maxDemandeJournaliere-minDemandeJournaliere + 1) + minDemandeJournaliere;

}

int Produit::getStockJournalier()
{
    // Premier jour
    if(jour == 1)
        return stockDepart;

    int stockVeille = stocks[jourIndexZero-1];
    int demandeJour = demandeJournalieres[jourIndexZero];
    int quantiteeRecue = getQuantiteesRecuesJournalier();

    return stockVeille - demandeJour + quantiteeRecue;
}

float Produit::getRuptureJournaliere()
{
    if(jour == 1)
        return 0;

    int stockVeille = stocks[jourIndexZero-1]; 
    int stockJourAvecReception = stockVeille + quantiteesCommandees[jourIndexZero]; 
    int demandeJour = demandeJournalieres[jourIndexZero];
    if(demandeJour > stockJourAvecReception)
    {
        if(stockJourAvecReception > 0)
            return demandeJour + stockJourAvecReception;
        else
            return demandeJour - stockJourAvecReception;
    }

    return 0;
}

float Produit::getCoutStockageJournalier()
{
    int stockVeille;
    if(jour == 1)
        stockVeille = stockDepart;
    else
        stockVeille = stocks[jourIndexZero-1];

    int stockJourAvecReception = stockVeille + quantiteesRecues[jourIndexZero]; 
    int demandeJour = demandeJournalieres[jourIndexZero];

    if(stockJourAvecReception > 0 && demandeJour <= stockJourAvecReception)
    {
        float coutStockage = (float)(stockJourAvecReception - demandeJour) * tauxPossession * (float)prixAchat / 365.0;

        // Case où le jour = vendredi, on prend en compte le week end
        if(jour % JOURS_OUVRES_PAR_SEMAINE == 0)
        {
            coutStockage *= 3.0;
        }

        return coutStockage;
    }

    return 0;
}

void Produit::incJour()
{
    jour++;
    jourIndexZero++;
}

float Produit::csvFloatToCppFloat(string csvFloat)
{

    replace(csvFloat.begin(), csvFloat.end(), ',', '.');
    return atof(csvFloat.c_str());
}

int Produit::getQuantiteesCommandeesJournalier()
{
    int stockFictifVeille = 0;

    if(jour == 1)
        stockFictifVeille = stockDepart; // Antoine ne respecte rien
    else 
        stockFictifVeille = stocksFictif[jourIndexZero - 1];

    int demandeJour = demandeJournalieres[jourIndexZero];
    int stockFictifMoinsDemande = (stockFictifVeille - demandeJour);
    if(stockFictifMoinsDemande < stockAlerte)
    {
        return quantite;
    }

    return 0;
}


int Produit::getQuantiteesRecuesJournalier()
{
    if(jour < delaiLivraison)
        return 0;

    return quantiteesCommandees[jourIndexZero - delaiLivraison];
}

int Produit::getStockFictifJournalier()
{
    if(jour == 1)
        return stockDepart + quantiteesCommandees[jourIndexZero];

    int stockFictifVeille = stocksFictif[jourIndexZero - 1];
    int demandeJour = demandeJournalieres[jourIndexZero];
    int quantiteeCommandeeJour = quantiteesCommandees[jourIndexZero];

    return stockFictifVeille - demandeJour + quantiteeCommandeeJour;
}

int Produit::getCoutLancementJournalier()
{
    if(quantiteesCommandees[jourIndexZero] != 0)
        return coutLancement;

    return 0;
}

float Produit::getTotalAnnee()
{
    float totalRuptures = accumulate(ruptures.begin(), ruptures.end(), 0);
    float totalCoutsStockage = accumulate(coutsStockage.begin(), coutsStockage.end(), 0);
    float totalCoutsLancement = accumulate(coutsLancement.begin(), coutsLancement.end(), 0);

    return totalRuptures + totalCoutsStockage + totalCoutsLancement;
}

void Produit::resetJour()
{
    jour = 1;
    jourIndexZero = 0;
}

void Produit::resetDemandeJournaliere()
{
    demandeJournaliere = demandeJournaliereInitiale;
}
