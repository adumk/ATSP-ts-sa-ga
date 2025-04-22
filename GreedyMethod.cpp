#ifndef GREEDYMETHOD_H
#define GREEDYMETHOD_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

class GreedyMethod {
private:
    static string resultPath;

    static void calculations(vector<vector<int>> adjacencyMatrix, int startVertex, vector<int>& resultsValues, vector<string>& resultsPaths){
        int resultValue = 0;
        //int startVertex = 0;
        int finalVertex = startVertex;
        vector<int> visited;
        int dimension = adjacencyMatrix.size();
        resultPath = to_string(finalVertex);

        for(int i = 0; i < dimension; i++){
            visited.push_back(finalVertex);
            int min = INT_MAX;
            int nextVertex = -1;

            for(int j = 0; j < dimension; j++) {
                if(finalVertex != j && find(visited.begin(), visited.end(), j) == visited.end()) {
                    if (adjacencyMatrix[finalVertex][j] < min){
                        min = adjacencyMatrix[finalVertex][j];
                        nextVertex = j;
                    }
                }
            }

            if (nextVertex == -1) {
                // Wszystkie wierzchołki odwiedzone, zamknij cykl
                resultValue += adjacencyMatrix[finalVertex][startVertex];
                resultPath += " " + to_string(startVertex);
                break;
            }

            resultPath += " " + to_string(nextVertex);
            finalVertex = nextVertex;
            resultValue += min;

        }
//        cout << "Sciezka: " << resultPath << endl;
//        cout << "Koszt: " << resultValue << endl;
        resultsValues.push_back(resultValue);
        resultsPaths.push_back(resultPath);

    }
public:
    static vector<int> mainCalculations(vector<vector<int>> adjacencyMatrix, vector<int>& resultsValues, vector<string>& resultsPaths){
        int dimension = adjacencyMatrix.size();
        for(int i = 0; i < dimension; i++){
            calculations(adjacencyMatrix, i, resultsValues, resultsPaths);
        }

        int minValue = INT_MAX;
        string minPath;
        for(int i = 0; i < resultsValues.size(); i++){
            if(resultsValues[i] < minValue){
                minValue = resultsValues[i];
                minPath = resultsPaths[i];
            }
        }
        cout << "Best path by greedy method: " << minPath << endl;
        cout << "Best cost by greedy method: " << minValue << endl;

        stringstream ss(minPath);
        int number;
        vector<int> path;

        while (ss >> number) {   // Wczytanie liczby ze strumienia
            path.push_back(number);
        }

        return path;
    }

    static pair<vector<vector<int>>, vector<int>> mainCalculationsGA(vector<vector<int>> adjacencyMatrix) {
        int dimension = adjacencyMatrix.size();
        vector<int> resultsValues;
        vector<string> resultsPaths;

        //int licznik = 50;

        for (int i = 0; i < dimension; i++) {
            calculations(adjacencyMatrix, i, resultsValues, resultsPaths);
        }

        vector<vector<int>> allPaths;
        vector<int> allCosts;

        for (size_t i = 0; i < resultsPaths.size(); i++) {
            stringstream ss(resultsPaths[i]);
            int number;
            vector<int> path;

            while (ss >> number) {
                path.push_back(number);
            }

            if (!path.empty()) {
                path.pop_back();  // Usunięcie powrotu do startowego wierzchołka
            }
            //if(licznik >= 0) {
                allPaths.push_back(path);
                allCosts.push_back(resultsValues[i]);
                //licznik--;
            //}
//
//            cout << "\nSciezka: \n";
//            for (int j = 0; j < dimension; j++) {
//                cout << allPaths[i][j] << " ";
//            }
//            cout << "\nKoszt: " << allCosts[i];

        }

        return {allPaths, allCosts};
    }


};
#endif