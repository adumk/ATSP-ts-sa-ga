#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <tuple>
#include <cmath>
#include <fstream>

using namespace std;



class TabuSearch {
public:
    // Oblicz koszt trasy
    static int calculateCost(const vector<int>& solution, vector<vector<int>> adjacencyMatrix, int dimension) {
        int cost = 0;
        //dimension = adjacencyMatrix.size();
        for (int i = 0; i < dimension - 1; i++) {
            cost += adjacencyMatrix[solution[i]][solution[i + 1]];
        }
        cost += adjacencyMatrix[solution[dimension - 1]][solution[0]]; // Powrót do startu
        return cost;
    }

    // Główna metoda wykonująca Tabu Search
    static pair<vector<int>, int> tabuSearchOneVertexChange(double timeLimitSeconds, vector<int> firstSolution, vector<vector<int>> adjacencyMatrix, int dimension, int tabuTenure, double& timeWhenFound) {
        auto startTime = chrono::steady_clock::now();
        vector<int> currentSolution = firstSolution;
        int currentCost = calculateCost(currentSolution, adjacencyMatrix, dimension);
        vector<int> bestSolution = currentSolution;
        int bestCost = currentCost;

        // Lista tabu jako macierz
        vector<vector<int>> tabuList(dimension, vector<int>(dimension, 0)); // Macierz [vertex][newPlace]

        int newCost;
        double time;
        //double timeWhenFound;
        int iterations = 0;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> randrand(1, dimension - 2);

        while (true) {
            vector<int> newSolution;
            int vertex, newPlace;

            // Znajdź nowe rozwiązanie, które nie jest tabu
            while(true){
                newSolution = currentSolution;
                do {
                    vertex = randrand(gen);
                    newPlace = randrand(gen);
                } while (vertex == newPlace);

                int city = newSolution[vertex];
                newSolution.erase(newSolution.begin() + vertex);
                newSolution.insert(newSolution.begin() + newPlace, city);

//                cout << "Wstawienie wierzcholka na indeksie: " << vertex << " na nowy indeks: " << newPlace << endl;
//                cout << "Rozwiazanie: ";
//                for(int i = 0; i < newSolution.size(); i++){
//                    cout << newSolution[i] << " ";
//                }

                if(chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count() >= timeLimitSeconds){
                    cout << "Iterations: " << iterations << endl;
//                    cout << "Czas znalezienia najlepszego rozwiazania: " << timeWhenFound << "ms" << endl;
//                    cout << "Iteracja, w ktorej znaleziono najlepsze rozwiazanie: " << iterationfound << endl;
                    ofstream plik;
                    plik.open("TS_results.txt", ios::app);
                    for(int i = 0; i < bestSolution.size(); i++){
                        plik << bestSolution[i] << " ";
                    }
                    plik << endl;
                    plik << bestCost << endl;
                    plik << timeWhenFound;
                    plik << endl;
                    plik << endl;
                    plik << endl;
                    plik << endl;
                    plik.flush();
                    plik.close();
                    return  {bestSolution, bestCost};
                }

                newCost = calculateCost(newSolution, adjacencyMatrix, dimension);
                time = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startTime).count();
                //cout << "Koszt: " << newCost << endl;


                double acceptanceThreshold = 1.05; // 5% pogorszenia

                if ((tabuList[vertex][newPlace] <= 0 && newCost < bestCost*acceptanceThreshold) || newCost < bestCost) {
                    // Aktualizacja bieżącego rozwiązania
                    currentSolution = newSolution;
                    iterations++;

                    if (newCost < bestCost) {
                        bestCost = newCost;
                        bestSolution = newSolution;
                        timeWhenFound = time;

                        ofstream plik;
                        plik.open("TS_results.txt", ios::app);
                        for(int i = 0; i < bestSolution.size(); i++){
                            plik << bestSolution[i] << " ";
                        }
                        plik << endl;
                        plik << bestCost << endl;
                        plik << timeWhenFound;
                        plik << endl;
                        plik.flush();
                        plik.close();
                    }

                    // Dodanie ruchu do tabu list
                    tabuList[vertex][newPlace] = tabuTenure; // Ustaw odpowiedni czas życia tabu
                }

                // Zmniejsz wartości na liście tabu
                for (auto& row : tabuList) {
                    for (auto& cell : row) {
                        if (cell > 0) cell--;
                    }
                }
            }
        }
    }

    static pair<vector<int>, int> tabuSearchSwapPlaces(double timeLimitSeconds, vector<int> firstSolution, vector<vector<int>> adjacencyMatrix, int dimension, int tabuTenure, double& timeWhenFound) {
        auto startTime = chrono::steady_clock::now();
        vector<int> currentSolution = firstSolution;
        //tabuList.assign(dimension, vector<int>(dimension, 0)); // Inicjalizacja 2D tablicy z zerami
        int currentCost = calculateCost(currentSolution, adjacencyMatrix, dimension);
        vector<int> bestSolution = currentSolution;
        int bestCost = currentCost;
        double time;
        vector<int> newSolution;

        vector<vector<int>> tabuList(dimension, vector<int>(dimension, 0)); // Macierz [vertex][newPlace]

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> randrand(1, dimension-2);

        int iterations = 0;
        while (true) {
            vector<int> newSolution;
            int newCost;
            int firstVertex;
            int secondVertex;
            while(true){
                newSolution = currentSolution;
                do {
                    firstVertex = randrand(gen);
                    secondVertex = randrand(gen);
                } while (firstVertex == secondVertex);

                swap(newSolution[firstVertex], newSolution[secondVertex]);

                if(chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count() >= timeLimitSeconds){
//                    cout << "Iteracje: " << iterations << endl;
//                    cout << "Czas znalezienia najlepszego rozwiazania: " << timeWhenFound << "ms" << endl;
//                    cout << "Iteracja, w ktorej znaleziono najlepsze rozwiazanie: " << iterationFound << endl;
                    ofstream plik;
                    plik.open("TS_results.txt", ios::app);
                    for(int i = 0; i < bestSolution.size(); i++){
                        plik << bestSolution[i] << " ";
                    }
                    plik << endl;
                    plik << bestCost << endl;
                    plik << timeWhenFound;
                    plik << endl;
                    plik << endl;
                    plik << endl;
                    plik << endl;
                    plik.flush();
                    plik.close();
                    return  {bestSolution, bestCost};
                }

                newCost = calculateCost(newSolution, adjacencyMatrix, dimension);
                time = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startTime).count();
                iterations++;

                double acceptanceThreshold = 1.05; // 5% pogorszenia

                if ((tabuList[firstVertex][secondVertex] <= 0 && newCost < bestCost*acceptanceThreshold) || newCost < bestCost) {
                    // Aktualizacja bieżącego rozwiązania
                    currentSolution = newSolution;

                    if (newCost < bestCost) {
                        bestCost = newCost;
                        bestSolution = newSolution;
                        timeWhenFound = time;

                        ofstream plik;
                        plik.open("TS_results.txt", ios::app);
                        for(int i = 0; i < bestSolution.size(); i++){
                            plik << bestSolution[i] << " ";
                        }
                        plik << endl;
                        plik << bestCost << endl;
                        plik << timeWhenFound;
                        plik << endl;
                        plik.flush();
                        plik.close();
                    }

                    // Dodanie ruchu do tabu list
                    tabuList[firstVertex][secondVertex] = tabuTenure; // Ustaw odpowiedni czas życia tabu
                }

                // Zmniejsz wartości na liście tabu
                for (auto& row : tabuList) {
                    for (auto& cell : row) {
                        if (cell > 0) cell--;
                    }
                }
                iterations++;
            }
        }

//        cout << "Iteracje: " << iterations << endl;
//        cout << "Czas znalezienia najlepszego rozwiazania: " << timeWhenFound << endl;
//        return {bestSolution, bestCost};
    }

    static pair<vector<int>, int> tabuSearchReverseOrder(double timeLimitSeconds, vector<int> firstSolution, vector<vector<int>> adjacencyMatrix, int dimension, int tabuTenure, double& timeWhenFound) {
        auto startTime = chrono::steady_clock::now();
        vector<int> currentSolution = firstSolution;
        //tabuList.assign(dimension, vector<int>(dimension, 0)); // Inicjalizacja 2D tablicy z zerami
        int currentCost = calculateCost(currentSolution, adjacencyMatrix, dimension);
        vector<int> bestSolution = currentSolution;
        int bestCost = currentCost;
        double time;

        vector<int> newSolution;

        vector<vector<int>> tabuList(dimension, vector<int>(dimension, 0)); // Macierz [vertex][newPlace]

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> randrand(1, dimension-2);

        int iterations = 0;
        while (true) {
            vector<int> newSolution = currentSolution;
            int newCost;
            int firstVertex;
            int secondVertex;
            while(true){
                newSolution = currentSolution;
                do {
                    firstVertex = randrand(gen);
                    secondVertex = randrand(gen);
                } while (firstVertex == secondVertex);


                int bigger = max(firstVertex, secondVertex);
                int smaller = min(firstVertex, secondVertex);
                int numberOfVerticesToReverse = bigger - smaller + 1;
                int iterator = 0;
                int numOfSwaps = numberOfVerticesToReverse/2;

                while(iterator != numOfSwaps){
                    swap(newSolution[smaller+iterator], newSolution[bigger-iterator]);
                    iterator++;
                }

                if(chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count() >= timeLimitSeconds){
//                    cout << "Iteracje: " << iterations << endl;
//                    cout << "Czas znalezienia najlepszego rozwiazania: " << timeWhenFound << "ms" << endl;
//                    cout << "Iteracja, w ktorej znaleziono najlepsze rozwiazanie: " << iterationFound << endl;
                    ofstream plik;
                    plik.open("TS_results.txt", ios::app);
                    for(int i = 0; i < bestSolution.size(); i++){
                        plik << bestSolution[i] << " ";
                    }
                    plik << endl;
                    plik << bestCost << endl;
                    plik << timeWhenFound;
                    plik << endl;
                    plik << endl;
                    plik << endl;
                    plik << endl;
                    plik.flush();
                    plik.close();

                    return  {bestSolution, bestCost};
                }

                newCost = calculateCost(newSolution, adjacencyMatrix, dimension);
                time = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startTime).count();

                double acceptanceThreshold = 1.05; // 1% pogorszenia

                if ((tabuList[firstVertex][secondVertex] <= 0 && newCost < bestCost*acceptanceThreshold) || newCost < bestCost) {
                    // Aktualizacja bieżącego rozwiązania
                    currentSolution = newSolution;

                    if (newCost < bestCost) {
                        bestCost = newCost;
                        bestSolution = newSolution;
                        timeWhenFound = time;

                        ofstream plik;
                        plik.open("TS_results.txt", ios::app);
                        for(int i = 0; i < bestSolution.size(); i++){
                            plik << bestSolution[i] << " ";
                        }
                        plik << endl;
                        plik << bestCost << endl;
                        plik << timeWhenFound;
                        plik << endl;
                        plik.flush();
                        plik.close();
                    }

                    // Dodanie ruchu do tabu list
                    tabuList[firstVertex][secondVertex] = tabuTenure; // Ustaw odpowiedni czas życia tabu
                }


                // Zmniejsz wartości na liście tabu
                for (auto& row : tabuList) {
                    for (auto& cell : row) {
                        if (cell > 0) cell--;
                    }
                }


                iterations++;
            }
        }

//        cout << "Iteracje: " << iterations << endl;
//        cout << "Czas znalezienia najlepszego rozwiazania: " << timeWhenFound << endl;
//        return {bestSolution, bestCost};
    }
};

#endif
