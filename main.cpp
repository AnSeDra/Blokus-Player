#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>

using namespace std;

const int Nr_Jucatori = 2;
const int Adancime_Alpha_Beta = 3;
const int NrDirectii = 4;
const int Marime_Piese = 5;
const int Marime_Tabla = 14;
const int Nr_Piese_Diferite = 21;
const int Nr_Configuratii_Piese = 14005;
const int INF = 1e9 + 5;

int dirX[] = {1, -1, 0,  0};
int dirY[] = {0,  0, 1, -1};

int dirColtX[] = {1,  1, -1, -1};
int dirColtY[] = {1, -1,  1, -1};

struct arbore{
    int x, y, parinte;
    vector<int> vecini;
};

struct piesa{
    int marime, indice;
    arbore biti[Marime_Piese + 1];
};

struct mutare{
    int indice_piesa, indice_configuratie;
    int poz_start_x, poz_start_y;
};

string comanda;
char culoare;
int nr_piese;
piesa piese[Nr_Configuratii_Piese];
bool folosit[Nr_Jucatori + 1][Nr_Piese_Diferite + 1], mask[Marime_Piese + 1][Marime_Piese + 1], rotatie[Marime_Piese + 1][Marime_Piese + 1], eValid;

int tabla_de_joc[Marime_Tabla + 2][Marime_Tabla + 2];

///   Pe tabla de joc se afla:
/// -1  => celula bordata
///  0  => celula libera
///  1  => celula ocupata de o mutare de mine
///  2  => celula ocupata de o mutare a adversarului
///  3  => celula adiacenta cu o piesa pusa de jucatorul curent
///  4  => colt valid pentru piesele jucatorului curent
///  5, 6, 7  => piese puse temporar pe tabla

void comenzi_initiale(){
    ///citire set_game Blokus Duo
    getline(cin, comanda);
    cout << "= " << '\n' << '\n';
    cout.flush();

    ///citire clear_board
    getline(cin, comanda);
    cout << "= " << '\n' << '\n';
    cout.flush();

    ///citire si raspuns cputime
    getline(cin, comanda);
    cout << "= 0" << '\n' << '\n';
    cout.flush();
}

void bordare_tabla(){
    for(int i = 0; i < Marime_Tabla; i++){
        tabla_de_joc[i][0] = -1;
        tabla_de_joc[i][Marime_Tabla + 1] = -1;

        tabla_de_joc[0][i] = -1;
        tabla_de_joc[Marime_Tabla + 1][i] = -1;
    }
}

void dfs(int i, int j, int curent){
    rotatie[i][j] = 0;

    int auxi, auxj;

    for(int h = 0; h < NrDirectii; h++){
        auxi = i + dirX[h];
        auxj = j + dirY[h];

        if(auxi <= 0 || auxi > Marime_Piese || auxj <= 0 || auxj > Marime_Piese){
            continue;
        }

        if(rotatie[auxi][auxj] == 1){
            piese[nr_piese].biti[++piese[nr_piese].marime].x = piese[nr_piese].biti[curent].x + dirX[h];
            piese[nr_piese].biti[piese[nr_piese].marime].y = piese[nr_piese].biti[curent].y + dirY[h];

            piese[nr_piese].biti[piese[nr_piese].marime].vecini.push_back(curent);
            piese[nr_piese].biti[curent].vecini.push_back(piese[nr_piese].marime);

            dfs(auxi, auxj, piese[nr_piese].marime);
        }
    }
}

void transformare_in_arbore(int indice){
    for(int i = 1; i <= Marime_Piese; i++){
        for(int j = 1; j <= Marime_Piese; j++){
            rotatie[i][j] = mask[i][j];
        }
    }

    nr_piese++;
    piese[nr_piese].marime = 0;
    piese[nr_piese].indice = indice;

    for(int i = 1; i <= Marime_Piese; i++){
        for(int j = 1; j <= Marime_Piese; j++){
            if(rotatie[i][j] == 1){
                piese[nr_piese].biti[++piese[nr_piese].marime].x = 0;
                piese[nr_piese].biti[piese[nr_piese].marime].y = 0;

                dfs(i, j, 1);
                return;
            }
        }
    }
}

void reset_mask(){
    for(int i = 1; i <= Marime_Piese; i++){
        for(int j = 1; j <= Marime_Piese; j++){
            mask[i][j] = 0;
        }
    }
}

void compute_and_rotate(int indice, int type){
    int rotatii;

    ///type = 1 => trebuie sa rotim de 2 ori cu 90 de grade
    ///type = 2 => trebuie sa rotim de 4 ori cu 90 de grade
    ///type = 3 => trebuie sa rotim si piesa si oglinditul ei de 4 ori cu 90 de grade
    ///type = 4 => trebuie sa rotim si piesa si oglinditul ei de 2 ori cu 90 de grade

    if(type == 1 || type == 4){
        rotatii = 2;
    }
    else{
        rotatii = 4;
    }

    for(int t = 0; t < rotatii; t++){
        transformare_in_arbore(indice);

        ///rotatie cu 90 de grade
        for(int i = 1; i <= Marime_Piese; i++){
            for(int j = 1; j <= Marime_Piese; j++){
                rotatie[i][j] = mask[j][Marime_Piese - i + 1];
            }
        }

        for(int i = 1; i <= Marime_Piese; i++){
            for(int j = 1; j <= Marime_Piese; j++){
                mask[i][j] = rotatie[i][j];
            }
        }
    }

    if(type == 3 || type == 4){
        ///oglindim masca

        for(int i = 1; i <= Marime_Piese; i++){
            for(int j = 1; j <= Marime_Piese; j++){
                rotatie[i][j] = mask[Marime_Piese - i + 1][j];
            }
        }

        for(int i = 1; i <= Marime_Piese; i++){
            for(int j = 1; j <= Marime_Piese; j++){
                mask[i][j] = rotatie[i][j];
            }
        }

        ///rotim si oglinditul
        for(int t = 0; t < rotatii; t++){
            transformare_in_arbore(indice);

            for(int i = 1; i <= Marime_Piese; i++){
                for(int j = 1; j <= Marime_Piese; j++){
                    rotatie[i][j] = mask[j][Marime_Piese - i + 1];
                }
            }

            for(int i = 1; i <= Marime_Piese; i++){
                for(int j = 1; j <= Marime_Piese; j++){
                    mask[i][j] = rotatie[i][j];
                }
            }
        }
    }
}

void generare_piese_marime_1(){
    ///*
    mask[1][1] = 1;
    transformare_in_arbore(1);
}

void generare_piese_marime_2(){
    ///**
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    compute_and_rotate(2, 1);
}

void generare_piese_marime_3(){
    ///***
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[1][3] = 1;
    compute_and_rotate(3, 1);

    ///**
    ///*
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[2][1] = 1;
    compute_and_rotate(4, 2);
}

void generare_piese_marime_4(){
    /// ****
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[1][3] = 1;
    mask[1][4] = 1;
    compute_and_rotate(5, 1);

    /// **
    /// **
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[2][1] = 1;
    mask[2][2] = 1;
    transformare_in_arbore(6);

    ///**
    /// **
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[2][2] = 1;
    mask[2][3] = 1;
    compute_and_rotate(7, 4);

    /// ***
    ///  *
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[1][3] = 1;
    mask[2][2] = 1;
    compute_and_rotate(8, 2);

    ///***
    ///*
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[1][3] = 1;
    mask[2][1] = 1;
    compute_and_rotate(9, 3);
}

void generare_piese_marime_5(){
    ///* *
    ///***
    reset_mask();
    mask[1][1] = 1;
    mask[2][1] = 1;
    mask[2][2] = 1;
    mask[2][3] = 1;
    mask[1][3] = 1;
    compute_and_rotate(10, 2);

    ///***
    ///**
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[1][3] = 1;
    mask[2][1] = 1;
    mask[2][2] = 1;
    compute_and_rotate(11, 3);

    ///****
    /// *
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[1][3] = 1;
    mask[1][4] = 1;
    mask[2][2] = 1;
    compute_and_rotate(12, 3);

    ///*
    ///***
    ///*
    reset_mask();
    mask[1][1] = 1;
    mask[2][1] = 1;
    mask[2][2] = 1;
    mask[2][3] = 1;
    mask[3][1] = 1;
    compute_and_rotate(13, 2);

    /// *
    ///***
    /// *
    reset_mask();
    mask[1][2] = 1;
    mask[2][1] = 1;
    mask[2][2] = 1;
    mask[2][3] = 1;
    mask[3][2] = 1;
    transformare_in_arbore(14);

    ///*****
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[1][3] = 1;
    mask[1][4] = 1;
    mask[1][5] = 1;
    compute_and_rotate(15, 1);

    /// **
    ///**
    /// *
    reset_mask();
    mask[1][2] = 1;
    mask[1][3] = 1;
    mask[2][1] = 1;
    mask[2][2] = 1;
    mask[3][2] = 1;
    compute_and_rotate(16, 3);

    ///*
    ///**
    /// **
    reset_mask();
    mask[1][1] = 1;
    mask[2][1] = 1;
    mask[2][2] = 1;
    mask[3][2] = 1;
    mask[3][3] = 1;
    compute_and_rotate(17, 2);

    ///**
    /// ***
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[2][2] = 1;
    mask[2][3] = 1;
    mask[2][4] = 1;
    compute_and_rotate(18, 3);

    ///**
    /// *
    /// **
    reset_mask();
    mask[1][1] = 1;
    mask[1][2] = 1;
    mask[2][2] = 1;
    mask[3][2] = 1;
    mask[3][3] = 1;
    compute_and_rotate(19, 4);

    ///*
    ///****
    reset_mask();
    mask[1][1] = 1;
    mask[2][1] = 1;
    mask[2][2] = 1;
    mask[2][3] = 1;
    mask[2][4] = 1;
    compute_and_rotate(20, 3);

    ///*
    ///*
    ///***
    reset_mask();
    mask[1][1] = 1;
    mask[2][1] = 1;
    mask[3][1] = 1;
    mask[3][2] = 1;
    mask[3][3] = 1;
    compute_and_rotate(21, 2);
}

void generare_piese(){
    nr_piese = 0;

    generare_piese_marime_1();
    generare_piese_marime_2();
    generare_piese_marime_3();
    generare_piese_marime_4();
    generare_piese_marime_5();
}

void afisare_tabla(){
    int scor_eu = 0, scor_el = 0;

    cerr.flush();

    cerr << '\n';
    for(int i = 1; i <= Marime_Tabla; i++){
        for(int j = 1; j <= Marime_Tabla; j++){
            cerr << tabla_de_joc[i][j];

            if(tabla_de_joc[i][j] == 1){
                scor_eu++;
            }
            else if(tabla_de_joc[i][j] == 2){
                scor_el++;
            }
        }
        cerr << '\n';
    }

    cerr << scor_eu << " " << scor_el << '\n';
}

void proceseaza_mutarea_adversarului(){
    unsigned poz, i, j, min_i, min_j;
    char culoare_adversar;
    bool eEgal;
    string pozitii_ocupate;
    vector<pair<int, int>> pozitii;

    cin >> culoare_adversar;
    getline(cin, pozitii_ocupate);

    min_i = Marime_Tabla;
    min_j = Marime_Tabla;

    poz = 1;  ///sarim primul spatiu pana la pozitii
    while(poz < pozitii_ocupate.size()){
        i = pozitii_ocupate[poz++] - 'a' + 1;

        j = 0;
        while(poz < pozitii_ocupate.size() && pozitii_ocupate[poz] >= '0' && pozitii_ocupate[poz] <= '9'){
            j = j * 10 + pozitii_ocupate[poz++] - '0';
        }

        if(poz < pozitii_ocupate.size()){
            poz++;  ///sarim virgula
        }

        tabla_de_joc[i][j] = 2;

        min_i = min(min_i, i);
        min_j = min(min_j, j);
        pozitii.push_back({i, j});
    }

    reset_mask();
    for(unsigned i = 0; i < pozitii.size(); i++){
        pozitii[i].first -= min_i - 1;
        pozitii[i].second -= min_j - 1;

        mask[pozitii[i].first][pozitii[i].second] = 1;
    }

    transformare_in_arbore(22);

    for(int i = 1; i < nr_piese; i++){
        if(piese[i].marime != piese[nr_piese].marime){
            continue;
        }
        else if(folosit[2][piese[i].indice] == 1){
            continue;
        }

        eEgal = 1;
        for(int j = 1; j <= piese[i].marime; j++){
            if((piese[i].biti[j].x != piese[nr_piese].biti[j].x) || (piese[i].biti[j].y != piese[nr_piese].biti[j].y)){
                eEgal = 0;
                break;
            }
        }

        if(eEgal){
            folosit[2][piese[i].indice] = 1;
        }
    }

    nr_piese--;

    cout << "= " << '\n' << '\n';
    cout.flush();
}

void prima_mutare(){
    cin >> comanda;

    if(comanda == "genmove"){
        cin >> culoare;

        cout << "= e9,d10,e10,f10,e11" << '\n' << '\n';
        cout.flush();

        tabla_de_joc[4][10] = 1;
        tabla_de_joc[5][10] = 1;
        tabla_de_joc[5][9] = 1;
        tabla_de_joc[5][11] = 1;
        tabla_de_joc[6][10] = 1;

        cin >> comanda;
        proceseaza_mutarea_adversarului();
    }
    else{
        proceseaza_mutarea_adversarului();

        cin >> comanda >> culoare;

        cout << "= j4,i5,j5,k5,j6" << '\n' << '\n';
        cout.flush();

        tabla_de_joc[10][4] = 1;
        tabla_de_joc[10][5] = 1;
        tabla_de_joc[9][5] = 1;
        tabla_de_joc[11][5] = 1;
        tabla_de_joc[10][6] = 1;
    }

    folosit[1][14] = 1; ///marchez x ca folosit
}

void incepe_partida(){
    comenzi_initiale();

    bordare_tabla();
    generare_piese();

    prima_mutare();
}

void pregatire_tabla(int jucator){
    int vecin_i, vecin_j, colt_i, colt_j;

    ///marcare celule blocate prin invecinarea cu piesele jucatorului curent
    for(int i = 1; i <= Marime_Tabla; i++){
        for(int j = 1; j <= Marime_Tabla; j++){
            if(tabla_de_joc[i][j] != 0 && tabla_de_joc[i][j] != 3 && tabla_de_joc[i][j] != 4 && ((tabla_de_joc[i][j] % 2) == (jucator % 2))){
                for(int dir = 0; dir < NrDirectii; dir++){
                    vecin_i = i + dirX[dir];
                    vecin_j = j + dirY[dir];

                    if(vecin_i >= 1 && vecin_i <= Marime_Tabla && vecin_j >= 1 && vecin_j <= Marime_Tabla){
                        if(tabla_de_joc[vecin_i][vecin_j] == 0 || tabla_de_joc[vecin_i][vecin_j] == 4){
                            tabla_de_joc[vecin_i][vecin_j] = 3;
                        }
                    }

                    colt_i = i + dirColtX[dir];
                    colt_j = j + dirColtY[dir];

                    if(colt_i >= 1 && colt_i <= Marime_Tabla && colt_j >= 1 && colt_j <= Marime_Tabla){
                        if(tabla_de_joc[colt_i][colt_j] == 0){
                            tabla_de_joc[colt_i][colt_j] = 4;
                        }
                    }
                }
            }
        }
    }
}

void parcurgere_piesa(int indice_configuratie, int x, int y, int bit){
    int vecin_x, vecin_y;

    if(!eValid){
        return;
    }

    for(int indice_vecin : piese[indice_configuratie].biti[bit].vecini){
        if(!eValid){
            return;
        }

        if(indice_vecin != piese[indice_configuratie].biti[bit].parinte){
            piese[indice_configuratie].biti[indice_vecin].parinte = bit;

            vecin_x = x + piese[indice_configuratie].biti[indice_vecin].x - piese[indice_configuratie].biti[bit].x;
            vecin_y = y + piese[indice_configuratie].biti[indice_vecin].y - piese[indice_configuratie].biti[bit].y;

            if(vecin_x <= 0 || vecin_x > Marime_Tabla || vecin_y <= 0 || vecin_y > Marime_Tabla){
                eValid = 0;
            }
            else if(tabla_de_joc[vecin_x][vecin_y] != 0 && tabla_de_joc[vecin_x][vecin_y] != 4){
                eValid = 0;
            }

            parcurgere_piesa(indice_configuratie, vecin_x, vecin_y, indice_vecin);
        }
    }
}

void verificare_pozitie(int indice_configuratie, int x, int y, int bit_start){
    for(int i = 1; i <= piese[indice_configuratie].marime; i++){
        piese[indice_configuratie].biti[i].parinte = 0;
    }
    piese[indice_configuratie].biti[bit_start].parinte = -1;

    parcurgere_piesa(indice_configuratie, x, y, bit_start);
}

void actualizare_finala_matrice(mutare mutare_optima){
    folosit[1][mutare_optima.indice_piesa] = 1;

    for(int i = 1; i <= Marime_Tabla; i++){
        for(int j = 1; j <= Marime_Tabla; j++){
            if(tabla_de_joc[i][j] == 3 || tabla_de_joc[i][j] == 4){
                tabla_de_joc[i][j] = 0;
            }
        }
    }

    for(int i = 1; i <= piese[mutare_optima.indice_configuratie].marime; i++){
        tabla_de_joc[mutare_optima.poz_start_x + piese[mutare_optima.indice_configuratie].biti[i].x][mutare_optima.poz_start_y + piese[mutare_optima.indice_configuratie].biti[i].y] = 1;
    }
}

void actualizare_temporara_matrice(mutare mt, int valoare, int jucator){
    folosit[jucator][mt.indice_piesa] = 1;

    for(int i = 1; i <= Marime_Tabla; i++){
        for(int j = 1; j <= Marime_Tabla; j++){
            if(tabla_de_joc[i][j] == 3 || tabla_de_joc[i][j] == 4){
                tabla_de_joc[i][j] = 0;
            }
        }
    }

    for(int i = 1; i <= piese[mt.indice_configuratie].marime; i++){
        tabla_de_joc[mt.poz_start_x + piese[mt.indice_configuratie].biti[i].x][mt.poz_start_y + piese[mt.indice_configuratie].biti[i].y] = valoare;
    }
}

void reseteaza_ultima_mutare(mutare mt, int valoare, int jucator){
    folosit[jucator][mt.indice_piesa] = 0;

    for(int i = 1; i <= Marime_Tabla; i++){
        for(int j = 1; j <= Marime_Tabla; j++){
            if(tabla_de_joc[i][j] == valoare || tabla_de_joc[i][j] == 3 || tabla_de_joc[i][j] == 4){
                tabla_de_joc[i][j] = 0;
            }
        }
    }
}

void afisare_mutare(mutare mutare_optima){
    cout << "= ";

    for(int i = 1; i <= piese[mutare_optima.indice_configuratie].marime; i++){
        cout << (char)('a' + mutare_optima.poz_start_x + piese[mutare_optima.indice_configuratie].biti[i].x - 1) << mutare_optima.poz_start_y + piese[mutare_optima.indice_configuratie].biti[i].y;

        if(i < piese[mutare_optima.indice_configuratie].marime){
            cout << ',';
        }
    }

    cout << '\n' << '\n';
    cout.flush();
}

int evaluare_statica(int jucator){
    int scor = 0;

    pregatire_tabla(jucator);

    for(int i = 1; i <= Marime_Tabla; i++){
        for(int j = 1; j <= Marime_Tabla; j++){
            if(tabla_de_joc[i][j] == 4){
                scor++;
            }
        }
    }

    return scor;
}

pair<mutare, int> calculare_mutari_posibile_si_alpha_beta(int nivel, int alpha, int beta, int jucator){
    int scor_mutare;
    mutare mt, best_mt;

    if(nivel > Adancime_Alpha_Beta){
        return {mt, evaluare_statica(jucator)};
    }

    pregatire_tabla(jucator);

    for(int r = 1; r <= nr_piese; r++){
        if(folosit[jucator][piese[r].indice] == 0){
            ///verificam tabla si gasim colutrile valide
            for(int i = 1; i <= Marime_Tabla; i++){
                for(int j = 1; j <= Marime_Tabla; j++){
                    if(tabla_de_joc[i][j] == 4){
                        ///verificam pentru fiecare bit din piesa daca poate fi pozitionat pe pozitia curenta

                        for(int h = 1; h <= piese[r].marime; h++){
                            eValid = 1;
                            verificare_pozitie(r, i, j, h);
                            if(eValid){
                                mt.indice_configuratie = r;
                                mt.indice_piesa = piese[r].indice;
                                mt.poz_start_x = i - piese[r].biti[h].x;
                                mt.poz_start_y = j - piese[r].biti[h].y;

                                actualizare_temporara_matrice(mt, nivel + 4, jucator);

                                scor_mutare = -calculare_mutari_posibile_si_alpha_beta(nivel + 1, -beta, -alpha, 3 - jucator).second;

                                reseteaza_ultima_mutare(mt, nivel + 4, jucator);

                                pregatire_tabla(jucator);

                                if(scor_mutare >= beta){
                                    best_mt = mt;
                                    beta = scor_mutare;
                                    return {best_mt, beta};
                                }
                                else if(scor_mutare >= alpha){
                                    alpha = scor_mutare;
                                    best_mt = mt;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return {best_mt, alpha};
}

void generare_mutare(){
    mutare mutare_optima;
    char culoare;

    cin >> culoare;

    mutare_optima = calculare_mutari_posibile_si_alpha_beta(1, -INF, INF, 1).first;
    actualizare_finala_matrice(mutare_optima);

    afisare_mutare(mutare_optima);
}

int main(){
    incepe_partida();

    while(1){
        cin >> comanda;

        if(comanda == "cputime"){
            cout << "= 0" << '\n' << '\n';
            cout.flush();
        }
        else if(comanda == "quit"){
            cerr << "Succes" << '\n';

            cout << "= 0" << '\n' << '\n';
            cout.flush();
            break;
        }

        if(comanda == "play"){
            proceseaza_mutarea_adversarului();
        }
        else if(comanda == "genmove"){
            generare_mutare();
        }
    }

    return 0;
}
