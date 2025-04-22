#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <tuple>
#include <cmath>
#include <fstream>

using namespace std;

class SimulatedAnnealing {
private:

    // Oblicz koszt trasy
    static int calculateCost(const vector<int> &solution, vector<vector<int>> adjacencyMatrix, int dimension) {
        int cost = 0;
        //dimension = adjacencyMatrix.size();
        for (int i = 0; i < dimension - 1; i++) {
            cost += adjacencyMatrix[solution[i]][solution[i + 1]];
        }
        cost += adjacencyMatrix[solution[dimension - 1]][solution[0]]; // Powrót do startu
        return cost;
    }

public:
    static pair<vector<int>, int>
    simulatedAnnealing(double timeLimitSeconds, vector<int> firstSolution, vector<vector<int>> adjacencyMatrix,
                       int dimension, double tempChange, double& timeWhenFound) {
        auto startTime = chrono::steady_clock::now();
        vector<int> currentSolution = firstSolution;
        double time;
        //double timeWhenFound;

        int currentCost = calculateCost(currentSolution, adjacencyMatrix, dimension);
        vector<int> bestSolution = currentSolution;
        int bestCost = currentCost;

        vector<int> newSolution;

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> randrand(1, dimension - 2);

        vector<int> startedCosts;
        int newCost;
        int firstVertex;
        int secondVertex;
        for(int i = 0; i < 50; i++){
            newSolution = currentSolution;
            do {
                firstVertex = randrand(gen);
                secondVertex = randrand(gen);
            } while (firstVertex == secondVertex);


            swap(newSolution[firstVertex], newSolution[secondVertex]);
            startedCosts.push_back(calculateCost(newSolution, adjacencyMatrix, dimension));
        }

        int avg = 0;
        for(int i = 0; i < 50; i++){
            startedCosts[i] -= currentCost;
            //cout << "Roznica dla " << i+1 << " wylosowanego rozwiazania: " << startedCosts[i] << endl;
            avg += startedCosts[i];
        }

        avg /= 50;      // obliczenie sredniej z wyliczonych roznic
        double P = 0.98; // Wstępnie zadane prawdopodobieństwo akceptacji

        double temp = -avg / log(P);

        cout << "Calculated initial temperature is " << temp << endl;

        int iterations = 0;
        int licznik = 0;
        while (true) {

            int newPlace;
            int vertex;
            double expo;
            do {
                newSolution = currentSolution;
                do {
                    vertex = randrand(gen);
                    newPlace = randrand(gen);
                } while (vertex == newPlace);

                int city = newSolution[vertex];
                newSolution.erase(newSolution.begin() + vertex); // Usuń wierzchołek
                newSolution.insert(newSolution.begin() + newPlace, city); // Wstaw w nowe miejsce

                if(chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count() >= timeLimitSeconds){
                    cout << "Iterations: " << iterations << endl;
                    //cout << "Licznik: " << licznik << endl;
                    cout << "Time to find the best solution: " << timeWhenFound << "s" << endl;
                    ofstream plik;
                    plik.open("SA_results.txt", ios::app);
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
                temp *= tempChange;
                //cout << temp << endl;
                iterations++;
                expo = exp((currentCost - newCost) / temp);
            }while(newCost >= currentCost || expo <= 0.9);

            // Aktualizuj najlepsze rozwiązanie
            if (newCost < bestCost) {
                bestSolution = newSolution;
                bestCost = newCost;
                timeWhenFound = time;

                ofstream plik;
                plik.open("SA_results.txt", ios::app);
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

            currentSolution = newSolution;
            licznik++;
        }
        cout << "Iteracje: " << iterations << endl;
        cout << "Liczba przyjetych nowych rozwiazan (lepszych lub gorszych, ktore algorytm zdecydowal sie przyjac do dalszych obliczen): " << licznik << endl;
        cout << "Czas znalezienia najlepszego rozwiazania: " << timeWhenFound << "s" << endl;
        return {bestSolution, bestCost};
    }
};

#endif
