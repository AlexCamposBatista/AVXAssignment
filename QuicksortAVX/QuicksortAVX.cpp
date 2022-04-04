// QuicksortAVX.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//
#include <random>
#include <chrono>
#include <iostream>
#include "QuickSort.h"

int main()
{
    //creating different datasets for performance tests
    QuickSort q;
    const uint64_t size = 32 * 1024 * 1024;
    std::vector<int> normalQuickSortDataBig = q.createRandomData(size);
    std::vector<int> avxQuickSortDataBig(normalQuickSortDataBig);

    std::vector<int> normalQuickSortDataMedium = { 24, 23, 22, 21 , 20 ,19, 18 ,17,
                                                   16 ,15 ,14, 13 , 12 , 11 , 10, 9, 
                                                   8 ,7 ,6, 5, 4, 3, 2, 1, 40, 50, 
                                                   60, 70, 80, 90, 100, 110 , 120,
                                                   33, 35, 44, 51 , 52 ,57, 118 ,117,
                                                   116 ,111 ,114, 113 , 112 , 211 , 210, 900,
                                                   800 ,700 ,600, 500, 400, 300, 200, 311,
                                                   410, 510, 610, 710, 810, 910, 1010, 1110 , 1210, -1, 1};
    std::vector<int> avxQuickSortDataMedium(normalQuickSortDataMedium);

    std::vector<int> normalQuickSortDataSmall = { 8, 1, 7, 2, 6, 3, 5, 4 };
    std::vector<int> avxQuickSortDataSmall(normalQuickSortDataSmall);
    

    //Small dataset
    double normalQuickSortTime = q.normalQuickSort(normalQuickSortDataSmall);
    
    if (!q.proove(normalQuickSortDataSmall)) {
        std::cout << "Small data set was not sorted correctly with normal Quicksort" << std::endl;
        exit(-1);
    }

    double avxQuickSortTime = q.avxQuickSort(avxQuickSortDataSmall);

    if (!q.proove(avxQuickSortDataSmall)) {
        std::cout << "Small data set was not sorted correctly with avx Quicksort" << std::endl;
        exit(-1);
    }

    std::cout << "Amount of Elements: " << normalQuickSortDataSmall.size() << std::endl;
    std::cout << "normal Quicksort sorted in " << normalQuickSortTime / 1000 << " us" << std::endl;
    std::cout << "avx Quicksort sorted in " << avxQuickSortTime / 1000 << " us" << std::endl;
    std::cout << "Speedup is " << normalQuickSortTime / avxQuickSortTime << std::endl << std::endl;

    //medium data set
    normalQuickSortTime = q.normalQuickSort(normalQuickSortDataMedium);

    if (!q.proove(normalQuickSortDataMedium)) {
        std::cout << "Medium data set was not sorted correctly with normal Quicksort" << std::endl;
        exit(-1);
    }

    avxQuickSortTime = q.avxQuickSort(avxQuickSortDataMedium);

    if (!q.proove(avxQuickSortDataMedium)) {
        std::cout << "Medium data set was not sorted correctly with avx Quicksort" << std::endl;
        exit(-1);
    }

    std::cout << "Amount of Elements: " << normalQuickSortDataMedium.size() << std::endl;
    std::cout << "normal Quicksort sorted in " << normalQuickSortTime / 1000 << " us" << std::endl;
    std::cout << "avx Quicksort sorted in " << avxQuickSortTime / 1000 << " us" << std::endl;
    std::cout << "Speedup is " << normalQuickSortTime / avxQuickSortTime << std::endl << std::endl;



    //Big data set
    normalQuickSortTime = q.normalQuickSort(normalQuickSortDataBig);

    if (!q.proove(normalQuickSortDataBig)) {
        std::cout << "Big data set was not sorted correctly with avx Quicksort" << std::endl;
        exit(-1);
    }

    avxQuickSortTime = q.avxQuickSort(avxQuickSortDataBig);

    if (!q.proove(avxQuickSortDataBig)) {
        std::cout << "Big data set was not sorted correctly with normal Quicksort" << std::endl;
        exit(-1);
    }

    std::cout << "Amount of Elements: " << normalQuickSortDataBig.size() << std::endl;
    std::cout << "normal Quicksort sorted in " << normalQuickSortTime / 1000 << " us" << std::endl;
    std::cout << "avx Quicksort sorted in " << avxQuickSortTime / 1000 << " us" << std::endl;
    std::cout << "Speedup is " << normalQuickSortTime / avxQuickSortTime << std::endl << std::endl;

    //std:: cout << "normal Quicksort sorted in " << normalQuickSortTime / 1000000 << " ms" << std::endl;


}

// Programm ausführen: STRG+F5 oder Menüeintrag "Debuggen" > "Starten ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"

// Tipps für den Einstieg: 
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzufügen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzufügen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzufügen", um dem Projekt vorhandene Codedateien hinzuzufügen.
//   6. Um dieses Projekt später erneut zu öffnen, wechseln Sie zu "Datei" > "Öffnen" > "Projekt", und wählen Sie die SLN-Datei aus.
