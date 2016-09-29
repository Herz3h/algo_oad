#include "produit.hpp"

void readCsv();

int main()
{
    srand(time(NULL));
    readCsv();
}

void readCsv()
{

    vector<Produit*> liste_produits;

    // Variable pour les lignes lues 
    string produit, categorie, tauxSaisonnalite, tauxPossession, pourcentagePerte;
    int stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, prixAchat;

    int i = 1;

    for(int j = 1;j < 10;j++)
    {
        // Initialisation
        io::CSVReader<13, io::trim_chars<' '>, io::no_quote_escape<';'> > in("donneesventes.csv");
        in.read_header(io::ignore_no_column, "Produit", "Categorie", "Stock_de_depart", "Demande_d1", "Croissance_par_jour", "Mois_de_saisonnalite", "Saisonnalite", "Variation_journaliere", "Delai_livraison", "Cout_de_lancement", "Taux_de_possession", "Prix_d_achat", "Pourcentage_perte");

        // On efface le contenu du fichier de sortie précedent
        ofstream ofs;
        ofs.open("output" + to_string(j) + ".csv", ios::out | ios::trunc);
        ofs << "Produit;Pdc_stock_alerte;Pdc_quantite" << endl;
        ofs.close();

        int totalSimulation = 0;
        // Récuperation des données par ligne (1produit/ligne)
        while(in.read_row(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, tauxSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, tauxPossession, prixAchat, pourcentagePerte)){
            Produit p(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, prixAchat, tauxSaisonnalite, tauxPossession, pourcentagePerte, "output" + to_string(j) + ".csv");
            i++;
            totalSimulation += p.getMeilleurCandidatGlobal().coutTotal;
        }
        cout << totalSimulation << endl;
    }


    /* in.read_row(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, tauxSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, tauxPossession, prixAchat, pourcentagePerte); */

    /* new Produit(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, prixAchat, tauxSaisonnalite, tauxPossession, pourcentagePerte); */
}

