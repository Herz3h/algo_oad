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
    borneMin(0), borneMax(30000), indexListeTabou(0), nbEssaiSansAmelioration(0)
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

    listeTabou.resize(TAILLE_LISTE_TABOU);


    meilleurCandidatGlobal.stockAlerte = 3000;
    meilleurCandidatGlobal.quantite = 2000;
    meilleurCandidatGlobal.coutTotal = 15000000;
    /* vector<struct candidat> voisins; */
    /* struct candidat tmp1 = {1500, 1500, 20000}; */ 
    /* struct candidat tmp2 = {1501, 1501, 1000}; */ 
    /* struct candidat tmp3 = {1502, 1502, 4000}; */ 
    /* struct candidat tmp4 = {1503, 1503, 8000}; */ 
    /* struct candidat tmp5 = {1504, 1504, 12000}; */ 
    /* struct candidat tmp6 = {1504, 1504, 500}; */ 
    /* voisins.push_back(tmp1); */
    /* voisins.push_back(tmp2); */
    /* voisins.push_back(tmp3); */
    /* voisins.push_back(tmp4); */
    /* voisins.push_back(tmp5); */
    /* voisins.push_back(tmp6); */

    /* retenirMeilleurVoisin(voisins); */


    setStockAlerteTheorique();
    struct candidat candidat_depart = {quantite, stockAlerte, 99999999999999.0};
    while(nbEssaiSansAmelioration < NB_ESSAI_SANS_AMELIORATION_MAX)
    {
        vector<struct candidat> voisins = genererVoisins(candidat_depart);
        retenirMeilleurVoisin(voisins);
    }

    cout << meilleurCandidatGlobal.quantite << " " << meilleurCandidatGlobal.stockAlerte << " " << meilleurCandidatGlobal.coutTotal << endl;
}

void Produit::setStockAlerteTheorique()
{
    for(int i=0; i < NB_JOURS_OUVRES_ANNEE;i++)
    {
        int d = getDemandeJournaliere();
        demandeJournalieres[i] = d;
    };

    float moyenne= 0;	
    float variance=0;
    float ecartType=0;
    struct coupleDemandeEffectif cDemEff; 
    int mycount;
    int stockSecurite;
    int ROP;
    vector<struct coupleDemandeEffectif > tableaucDemEff;

    // Calcul de la moyenne
    moyenne = accumulate(demandeJournalieres.begin(), demandeJournalieres.end(), 0);
    moyenne = moyenne/NB_JOURS_OUVRES_ANNEE;

    // Alimentation tableau cDemEff
    for (int i=0; i < NB_JOURS_OUVRES_ANNEE; i++){
        int demandeActuelle = demandeJournalieres[i];

        mycount = count_if(demandeJournalieres.begin(), demandeJournalieres.end(),
                [demandeActuelle](const int &cDEE){
                return demandeActuelle == cDEE;
                });

        cDemEff.demande = demandeActuelle;
        cDemEff.nbEffectif = mycount;

        vector<struct coupleDemandeEffectif>::iterator it = find_if(tableaucDemEff.begin(), tableaucDemEff.end(),
                [cDemEff] (const struct coupleDemandeEffectif &cDemEffA) {
                return cDemEffA.demande == cDemEff.demande && cDemEffA.nbEffectif == cDemEff.nbEffectif;
                }
                );
        if (it == tableaucDemEff.end())
            tableaucDemEff.push_back(cDemEff);
    }	

    //Calcul de la variance
    for (int i =0; i < tableaucDemEff.size(); i++){		
        variance = variance + (float)(tableaucDemEff[i].nbEffectif * pow((tableaucDemEff[i].demande - moyenne), 2));
    }
    variance = variance / NB_JOURS_OUVRES_ANNEE;

    // Calcul de l'ecart type
    ecartType = sqrt(variance); 	

    //Calcul du Stock de Securite	
    stockSecurite = 2.36 * ecartType * sqrt(delaiLivraison);

    //Calcul ROP = (Average Demand x Lead Time) + Safety Stock
    ROP = stockSecurite + (moyenne * delaiLivraison);
    cout << "ROP " << ROP << endl;

    resetJour();
    resetDemandeJournaliere();
}

int Produit::randInt(int a, int b)
{
    return rand()%(b-a)+a;
}

vector<struct candidat> Produit::genererVoisins(struct candidat candidat_depart){

    vector<struct candidat> voisins;
    int i=0, quatite=0, st=0;
    int minQ,maxQ,minSA,maxSA;

    minQ= candidat_depart.quantite  - candidat_depart.quantite * BORNE_QUANTITE_VOISINS;
    maxQ= candidat_depart.quantite  + candidat_depart.quantite * BORNE_QUANTITE_VOISINS;

    minSA= candidat_depart.stockAlerte  - candidat_depart.stockAlerte * BORNE_STOCK_ALERT_VOISINS;
    maxSA= candidat_depart.stockAlerte  + candidat_depart.stockAlerte * BORNE_STOCK_ALERT_VOISINS;


    for(i=0; i<NB_VOISINS;i++){

        struct candidat voisin;


        voisin.quantite=  randInt(minQ,maxQ);

        voisin.stockAlerte=  randInt(minSA,maxSA);

        quantite= voisin.quantite;
        stockAlerte= voisin.stockAlerte;

        resetDemandeJournaliere();
        resetJour();
        computeData();
        voisin.coutTotal = getTotalAnnee(); 

        voisins.push_back(voisin);
    }

    return voisins;

}

void Produit::retenirMeilleurVoisin(vector<struct candidat> &voisins)
{
    bool notInTabou = false;

    while(!notInTabou)
    {

        sort(voisins.begin(), voisins.end(),
                [] (const struct candidat &vA, const struct candidat &vB) {
                return vA.coutTotal < vB.coutTotal;
                }
            );

        if(!isTabou(voisins[0]))
        {
            compareResultatGlobal(voisins[0]);

            addListeTabou(voisins[0]);
            notInTabou = true;
        }
        else
            voisins.erase(voisins.begin());
    }
}

void Produit::addListeTabou(struct candidat &voisin)
{
    if(indexListeTabou == 0)
        listeTabou[indexListeTabou] = voisin;
    else
        listeTabou[indexListeTabou % TAILLE_LISTE_TABOU] = voisin;
    indexListeTabou++;
}

bool Produit::isTabou(struct candidat c)
{
    vector<struct candidat>::iterator it = find_if(listeTabou.begin(), listeTabou.end(),
            [c] (const struct candidat &C) {
            return c.stockAlerte == C.stockAlerte && c.quantite == C.quantite && c.coutTotal == C.coutTotal; 
            }
            );

    if(it != listeTabou.end())
        return true;
    else
        return false;
}

void Produit::compareResultatGlobal(struct candidat &voisin)
{
    if(voisin.coutTotal < meilleurCandidatGlobal.coutTotal)
    {
        meilleurCandidatGlobal = voisin;
        nbEssaiSansAmelioration = 0;
    }
    else
        nbEssaiSansAmelioration++;
}

void Produit::doPassage()
{

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

}

bool Produit::isInSaison()
{
    if(moisSaisonnalite == 0)
        return false;

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
