#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <random>
#include <iomanip>

using namespace std;

class Graph {
private:
    int numberOfVertices;
    int** adjacencyMatrix;
    static int* randomize(int numberOfVertices){
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> edge1(1, 50);
        uniform_int_distribution<> edge2(1, 50);
        uniform_int_distribution<> vertice1(0, numberOfVertices - 1);
        uniform_int_distribution<> vertice2(0, numberOfVertices - 1);

        int* result = new int[3];

        result[0] = edge1(gen);
        result[1] = edge2(gen);
        result[2] = vertice1(gen);
        result[3] = vertice2(gen);

        return result;
    };
public:
    static vector<vector<int>> graphGenerator(int numberOfVertices){
        vector<vector<int>> adjacencyMatrix;
//        for(int i = 0; i < numberOfVertices; i++){
//            adjacencyMatrix[i] = new int[numberOfVertices];
//        }
        adjacencyMatrix.resize(numberOfVertices, vector<int>(numberOfVertices));
        for (int i = 0; i < numberOfVertices; i++) {
            for (int j = 0; j < numberOfVertices; j++) {
                if(i != j) {
                    adjacencyMatrix[i][j] = 0;
                }
                else
                    adjacencyMatrix[i][j] = -1;
            }
        }
        int numberOfEdges = numberOfVertices*(numberOfVertices-1)/2;
        for(int i = 0; i < numberOfEdges; i++){
            int* data;
            bool check = true;
            while(check) {
                data = randomize(numberOfVertices);
                int vertice1 = data[2];
                int vertice2 = data[3];
                int weight1 = data[0];
                int weight2 = data[1];

                if (vertice1 != vertice2 && adjacencyMatrix[vertice1][vertice2] == 0) {
                    adjacencyMatrix[vertice1][vertice2] = weight1;
                    //cout << "Koszt przejscia od " << vertice1 << " do " << vertice2 << " wynosi " << weight1 << endl;
                    adjacencyMatrix[vertice2][vertice1] = weight2;
                    //cout << "Koszt przejscia od " << vertice2 << " do " << vertice1 << " wynosi " << weight2 << endl;
                    check = false;
                }
                delete[] data;
            }
        }
        return adjacencyMatrix;
    }

    static vector<vector<int>> graphGeneratorForVector(int numberOfVertices){
        vector<vector<int>> adjacencyMatrix;

        adjacencyMatrix.resize(numberOfVertices, vector<int>(numberOfVertices, 0));

        for (int i = 0; i < numberOfVertices; i++) {
            for (int j = 0; j < numberOfVertices; j++) {
                if (i == j) {
                    adjacencyMatrix[i][j] = -1;
                }
            }
        }
        int numberOfEdges = numberOfVertices*(numberOfVertices-1)/2;
        for(int i = 0; i < numberOfEdges; i++){
            int* data;
            bool check = true;
            while(check) {
                data = randomize(numberOfVertices);
                int vertice1 = data[2];
                int vertice2 = data[3];
                int weight1 = data[0];
                int weight2 = data[1];

                if (vertice1 != vertice2 && adjacencyMatrix[vertice1][vertice2] == 0) {
                    adjacencyMatrix[vertice1][vertice2] = weight1;
                    //cout << "Koszt przejscia od " << vertice1 << " do " << vertice2 << " wynosi " << weight1 << endl;
                    adjacencyMatrix[vertice2][vertice1] = weight2;
                    //cout << "Koszt przejscia od " << vertice2 << " do " << vertice1 << " wynosi " << weight2 << endl;
                    check = false;
                }
                delete[] data;
            }
        }
        return adjacencyMatrix;
    }


    static void matrixView(int** adjacencyMatrix, int numberOfVertices){
        cout << "    ";
        for(int i = 0; i < numberOfVertices; i++) {
            cout << setw(4) <<  i << " ";
        }
        cout << endl;
        for (int i = 0; i < numberOfVertices; i++) {
            cout << i << " | ";
            for (int j = 0; j < numberOfVertices; j++) {
                cout << setw(4) <<  adjacencyMatrix[i][j] << " ";
            }
            cout << " |";
            cout << endl;
        }
    }

    static void matrixViewForVector(vector<vector<int>> adjacencyMatrix, int numberOfVertices){
        cout << "    ";
        for(int i = 0; i < numberOfVertices; i++) {
            cout << setw(4) <<  i << " ";
        }
        cout << endl;
        for (int i = 0; i < numberOfVertices; i++) {
            cout << i << " | ";
            for (int j = 0; j < numberOfVertices; j++) {
                cout << setw(4) <<  adjacencyMatrix[i][j] << " ";
            }
            cout << " |";
            cout << endl;
        }
    }

    static void ReducedMatrixView(int** adjacencyMatrix, int numberOfVertices, vector<int> lines, vector<int> columns){
        cout << "    ";
        for(int i = 0; i < numberOfVertices; i++) {
            cout << setw(4) <<  columns[i] << " ";
        }
        cout << endl;
        for (int i = 0; i < numberOfVertices; i++) {
            cout << lines[i] << " | ";
            for (int j = 0; j < numberOfVertices; j++) {
                cout << setw(4) <<  adjacencyMatrix[i][j] << " ";
            }
            cout << " |";
            cout << endl;
        }
    }

};

#endif