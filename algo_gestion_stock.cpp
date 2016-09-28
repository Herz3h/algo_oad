#include "produit.hpp"

void readCsv();

int main()
{
    srand(time(NULL));
    readCsv();
}

void readCsv()
{
    // Initialisation
    io::CSVReader<13, io::trim_chars<' '>, io::no_quote_escape<';'> > in("donneesventes.csv");
    in.read_header(io::ignore_no_column, "Produit", "Categorie", "Stock_de_depart", "Demande_d1", "Croissance_par_jour", "Mois_de_saisonnalite", "Saisonnalite", "Variation_journaliere", "Delai_livraison", "Cout_de_lancement", "Taux_de_possession", "Prix_d_achat", "Pourcentage_perte");

    vector<Produit*> liste_produits;

    // Variable pour les lignes lues 
    string produit, categorie, tauxSaisonnalite, tauxPossession, pourcentagePerte;
    int stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, prixAchat;

    int i = 1;
    // Récuperation des données par ligne (1produit/ligne)
    /* while(in.read_row(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, tauxSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, tauxPossession, prixAchat, pourcentagePerte)){ */
    /*     cout << i << endl; */
    /*     new Produit(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, prixAchat, tauxSaisonnalite, tauxPossession, pourcentagePerte); */
    /*     i++; */
    /* } */


    in.read_row(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, tauxSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, tauxPossession, prixAchat, pourcentagePerte);

    new Produit(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, prixAchat, tauxSaisonnalite, tauxPossession, pourcentagePerte);

    /* in.read_row(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, tauxSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, tauxPossession, prixAchat, pourcentagePerte); */

    /* new Produit(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, prixAchat, tauxSaisonnalite, tauxPossession, pourcentagePerte); */
    /* in.read_row(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, tauxSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, tauxPossession, prixAchat, pourcentagePerte); */

    /* new Produit(produit, categorie, stockDepart, demandeJournaliere, croissanceJournaliere, moisSaisonnalite, variationJournaliere, delaiLivraison, coutLancement, prixAchat, tauxSaisonnalite, tauxPossession, pourcentagePerte); */
}

