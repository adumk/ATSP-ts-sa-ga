#ifndef GENETICALGORITHM2_H
#define GENETICALGORITHM2_H

#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <iostream>
#include <chrono>
#include "GreedyMethod.cpp"
#include <unordered_set>
#include <unordered_map>
#include <fstream>

using namespace std;

class GeneticAlgorithm2 {
private:
    static vector<int> generateIndividual(int dimension) {
        vector<int> individual(dimension);
        for (int i = 0; i < dimension; i++) {
            individual[i] = i;
        }
        shuffle(individual.begin(), individual.end(), default_random_engine(time(nullptr)));
        return individual;
    }

    static int calculateCost(const vector<int> &solution, const vector<vector<int>> &adjacencyMatrix) {
        int cost = 0;
        int dimension = solution.size();
        for (int i = 0; i < dimension - 1; i++) {
            cost += adjacencyMatrix[solution[i]][solution[i + 1]];
        }
        cost += adjacencyMatrix[solution[dimension - 1]][solution[0]];
        return cost;
    }

    static double randomProbability() {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dist(0.0, 1.0);
        return dist(gen);
    }

    static vector<int> tournamentSelection(const vector<pair<vector<int>, int>> &population, int tournamentSize) {
        vector<pair<vector<int>, int>> tournament;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, population.size() - 1);

        for (int i = 0; i < tournamentSize; i++) {
            tournament.push_back(population[dist(gen)]);
        }
        return min_element(tournament.begin(), tournament.end(), [](auto &a, auto &b) {
            return a.second < b.second;
        })->first;
    }

    static pair<vector<int>, vector<int>> crossoverOX(const vector<int> &parent1, const vector<int> &parent2) {
        int size = parent1.size();
        vector<int> child1(size, -1), child2(size, -1);     // inicjalizujemy dwojke dzieci, wypelniamy je -1

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, size - 1);
        int start = dist(gen), end = dist(gen);         // losowanie sekcji dopasowania
        if (start > end) swap(start, end);

        // Kopiowanie segmentu od start do end
        for (int i = start; i <= end; i++) {            // kopiowanie sekcji dopasowania
            child1[i] = parent1[i];
            child2[i] = parent2[i];
        }

        auto fillChild = [&](vector<int> &child, const vector<int> &parent) {       // wypelnianie miejsc poza sekcja dopasowania
            int insertPos = (end + 1) % size;
            for (int i = 0; i < size; i++) {
                int gene = parent[(end + 1 + i) % size];
                if (find(child.begin(), child.end(), gene) == child.end()) {
                    child[insertPos] = gene;
                    insertPos = (insertPos + 1) % size;
                }
            }
        };

        fillChild(child1, parent2);
        fillChild(child2, parent1);

        return {child1, child2};
    }

    static pair<vector<int>, vector<int>> crossoverEX(const vector<int> &parent1, const vector<int> &parent2) {     // z wykorzystaniem funkcji lambda, ktore maja referencje do zmiennych
        int size = parent1.size();
        vector<int> child1, child2;
        unordered_map<int, unordered_set<int>> adjacencyList;   // lista sąsiedztwa przechowująca możliwe połączenia miast

        auto buildAdjacencyList = [&](const vector<int> &p1, const vector<int> &p2) {   //budowanie listy sąsiedztwa
            for (int i = 0; i < size; i++) {
                int prev = (i == 0) ? size - 1 : i - 1;
                int next = (i == size - 1) ? 0 : i + 1;

                adjacencyList[p1[i]].insert(p1[prev]);
                adjacencyList[p1[i]].insert(p1[next]);
                adjacencyList[p2[i]].insert(p2[prev]);
                adjacencyList[p2[i]].insert(p2[next]);
            }
        };

        auto chooseNextVertex = [&](int current, unordered_set<int> &used) -> int {     //wybór następnego miasta
            if (adjacencyList.find(current) != adjacencyList.end()) {
                vector<int> candidates;
                int minSize = INT_MAX;

                for (int neighbor : adjacencyList[current]) {       // sprawdzamy kazdy wierzcholek w liscie
                    if (used.find(neighbor) == used.end()) {     // jesli nie jest uzyty
                        int size = adjacencyList[neighbor].size();
                        if (size < minSize) {
                            candidates.clear();
                            candidates.push_back(neighbor);         // dodajemy go
                            minSize = size;
                        } else if (size == minSize) {
                            candidates.push_back(neighbor);
                        }
                    }
                }

                if (!candidates.empty()) {
                    return candidates[rand() % candidates.size()];
                }
            }

            // Jeśli nie można wybrać z listy sąsiedztwa, wybierz losowy niewykorzystany wierzchołek
            for (int i = 0; i < size; i++) {
                if (used.find(parent1[i]) == used.end()) {
                    return parent1[i];
                }
            }
            return -1;
        };

        auto generateChild = [&](vector<int> &child) {      // generowanie potomka
            unordered_set<int> used;
            int current = parent1[rand() % size];       // pierwsze miasto jest losowe z rodzica

            while (child.size() < size) {               // dopoki potomek nie jest pełny
                child.push_back(current);               // dodaj wybrany wierzcholek do dziecka
                used.insert(current);                // oznacz go jako wykorzystany

                for (int neighbor : adjacencyList[current]) {
                    adjacencyList[neighbor].erase(current);     // usun wierzcholek z list sasiedztwa pozostalych wierzcholkow
                }
                adjacencyList.erase(current);

                if (!adjacencyList.empty()) {
                    int next = chooseNextVertex(current, used); // wybierz kolejny wierzcholek za pomoca choosenextvertex
                    if (next == -1) {
                        for (int i = 0; i < size; i++) {
                            if (used.find(parent1[i]) == used.end()) {  // jesli nie ma dostpenych, wybiera pierwsze mozliwe
                                next = parent1[i];
                                break;
                            }
                        }
                    }
                    current = next;
                }
            }
        };

        buildAdjacencyList(parent1, parent2);  // buduje liste sasiedztwa na podstawie rodzicow
        generateChild(child1);                      // generuje dziecko 1
        buildAdjacencyList(parent1, parent2); // Odtworzenie listy sąsiedztwa dla drugiego dziecka
        generateChild(child2);                      // generuje dziecko 2

        return {child1, child2};
    }

    static void mutationSwap(vector<int> &individual) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, individual.size() - 1);
        int i, j;
        do {
            i = dist(gen);
            j = dist(gen);
        }while(i == j);
        swap(individual[i], individual[j]);
    }

    static vector<int> mutationInsert(vector<int> individual){
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(0, individual.size() - 1);
        int gene;
        int newPlace;

        do {
            gene = dist(gen);
            newPlace = dist(gen);
        }while(gene == newPlace);

        int newGen = individual[gene];
        individual.erase(individual.begin() + gene);
        individual.insert(individual.begin() + newPlace, newGen);

        return individual;
    }


    static vector<pair<vector<int>, int>> succession(const vector<pair<vector<int>, int>> &population, vector<pair<vector<int>, int>> &offspring, int populationSize, int eliteSize) {
        vector<pair<vector<int>, int>> newGeneration = population;      // nowa generacja na poczatku jest kopią starej generacji
        sort(newGeneration.begin(), newGeneration.end(), [](auto &a, auto &b) {
            return a.second < b.second;
        });                                                     // sortowanie rosnąco generacji w celu wybrania elity
        newGeneration.resize(eliteSize);                // za pomocą resize zostawiamy tylko elite

        sort(offspring.begin(), offspring.end(), [](auto &a, auto &b) {
            return a.second < b.second;
        });                                                      // sortujemy potomstwo wedlug jakosci

        for (int i = 0; i < populationSize - eliteSize; i++) {  // dodajemy potomstwo do generacji
            newGeneration.push_back(offspring[i]);
        }

        return newGeneration;
    }

public:
    static pair<vector<int>, int> geneticAlgorithm(vector<vector<int>> adjacencyMatrix, int populationSize, string crossoverType, string mutationType, double crossoverProb, double mutationProb, int timeLimitSeconds) {
        ofstream plik;
        plik.open("GA_results.txt", ios::app);
        auto startTime = chrono::steady_clock::now();
        int dimension = adjacencyMatrix.size();
        vector<pair<vector<int>, int>> population;
        int tournamentSize = 20;
        int eliteSize = 0.1 * populationSize;
        int numTournaments = populationSize - eliteSize;
        double relativeError;
        //const double crossoverProb = 0.8;
        //const double mutationProb = 0.01;

        // Pobranie heurystycznych rozwiązań (greedy)
        auto [greedyPaths, greedyCosts] = GreedyMethod::mainCalculationsGA(adjacencyMatrix);    // generowanie rozwiazan greedy

        int greedyCount = greedyPaths.size();
        int remainingRandom = populationSize - greedyCount; // Liczba brakujących losowych osobników

        // Dodanie heurystycznych ścieżek do populacji
        for (int i = 0; i < greedyCount; i++) {
            population.emplace_back(greedyPaths[i], greedyCosts[i]);
        }

        // Uzupełnienie populacji losowymi osobnikami
        for (int i = 0; i < remainingRandom; i++) {
            vector<int> individual = generateIndividual(dimension);
            int cost = calculateCost(individual, adjacencyMatrix);
            population.emplace_back(individual, cost);
        }

        int timeCounter = 5;
        int licznik = 0;

        // Funkcja do obliczania średniego kosztu populacji
        auto calculateAverageCost = [&](const vector<pair<vector<int>, int>> &pop) {
            double sum = 0;
            for (const auto &ind : pop) {
                sum += ind.second;
            }
            return sum / pop.size();
        };

        //time_t startTime = time(nullptr);
        while (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count() <
               timeLimitSeconds) {
            licznik++;
            vector<pair<vector<int>, int>> offspring;
            for (int i = 0; i < numTournaments; i++) {
                vector<int> parent1 = tournamentSelection(population, tournamentSize);
                vector<int> parent2 = tournamentSelection(population, tournamentSize);

                if(crossoverType == "OX") {
                    if (randomProbability() < crossoverProb) {
                        auto [child1, child2] = crossoverOX(parent1, parent2);
                        if(mutationType == "swap") {
                            if (randomProbability() < mutationProb) mutationSwap(child1);
                            if (randomProbability() < mutationProb) mutationSwap(child2);
                        }
                        else if(mutationType == "insert"){
                            if (randomProbability() < mutationProb) mutationInsert(child1);
                            if (randomProbability() < mutationProb) mutationInsert(child2);
                        }

                        offspring.emplace_back(child1, calculateCost(child1, adjacencyMatrix));
                        offspring.emplace_back(child2, calculateCost(child2, adjacencyMatrix));
                    } else {
                        offspring.emplace_back(parent1, calculateCost(parent1, adjacencyMatrix));
                        offspring.emplace_back(parent2, calculateCost(parent2, adjacencyMatrix));
                    }
                }
                else if (crossoverType == "EX"){
                    if (randomProbability() < crossoverProb) {
                        auto [child1, child2] = crossoverEX(parent1, parent2);
                        if(mutationType == "swap") {
                            if (randomProbability() < mutationProb) mutationSwap(child1);
                            if (randomProbability() < mutationProb) mutationSwap(child2);
                        }
                        else if(mutationType == "insert"){
                            if (randomProbability() < mutationProb) mutationInsert(child1);
                            if (randomProbability() < mutationProb) mutationInsert(child2);
                        }

                        offspring.emplace_back(child1, calculateCost(child1, adjacencyMatrix));
                        offspring.emplace_back(child2, calculateCost(child2, adjacencyMatrix));
                    } else {
                        offspring.emplace_back(parent1, calculateCost(parent1, adjacencyMatrix));
                        offspring.emplace_back(parent2, calculateCost(parent2, adjacencyMatrix));
                    }
                }

            }
            population = succession(population, offspring, populationSize, eliteSize);

            if(timeCounter <= chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count()){
                auto best = *min_element(population.begin(), population.end(), [](auto &a, auto &b) {
                    return a.second < b.second;
                });
                double avgCost = calculateAverageCost(population);


                plik << endl;
                plik << "After " << chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - startTime).count() << "s" << endl;
                plik << "Best cost: \n" << best.second << "\n";
                plik << "Average cost: \n" << avgCost << "\n";
                plik.flush();
                timeCounter += 5;
            }
        }

        auto best = *min_element(population.begin(), population.end(), [](auto &a, auto &b) {
            return a.second < b.second;
        });

        double avgCostFinal = calculateAverageCost(population);
        double optValue = 2755.0;
        relativeError = ((best.second-optValue)/optValue)*100;

        cout << "Best solution found: " << best.second << "\nPath: ";
        for (int city : best.first) {
            cout << city << " ";
        }
        cout << endl;
        //cout << licznik;
        plik << "Best solution found: \n" << best.second;
        plik << "\nRelative error: \n";
        plik << relativeError << "%" << endl;
        plik << "Average population cost: \n" << avgCostFinal << "\n";
        plik << "Path: \n";
        for (int city : best.first) {
            plik << city << " ";
        }
        plik.flush();
        plik.close();
        return best;
    }
};

#endif
