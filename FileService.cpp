#ifndef LOADINGFROMFILE_H
#define LOADINGFROMFILE_H

#include <iostream>
#include <random>
#include <fstream>
#include "pugixml.hpp"
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

class FileService{
private:
public:
    static vector<vector<int>> loadingFromFile(string nazwaPliku){
        vector<vector<int>> adjacencyMatrix;

        int dimension;

        ifstream file;
        file.open(nazwaPliku.c_str());

        if (!file.is_open()) {
            cerr << "File not found or cannot open it" << endl;
            throw runtime_error("File opening failed");
        }

        string line;
        while (std::getline(file, line)) {
            if (line.find("DIMENSION") != string::npos) {
                // Znaleziono linię z DIMENSION
                size_t colonPos = line.find(":");  // Znajdź pozycję dwukropka
                if (colonPos != std::string::npos) {
                    std::string value = line.substr(colonPos + 1);  // Wytnij wszystko po dwukropku
                    //value.erase(remove(value.begin(), value.end(), ' '), value.end());  // Usuń spacje
                    dimension = std::stoi(value);  // Konwertuj na liczbę
                    std::cout << "dimension: " << dimension << std::endl;
                } else {
                    std::cerr << "Colon not found on line: " << line << std::endl;
                }
                break;  // Przerwij, gdy znajdziesz DIMENSION
            }
        }

        adjacencyMatrix.resize(dimension, vector<int>(dimension, 0));


        while (std::getline(file, line)) {
            if (line.find("EDGE_WEIGHT_SECTION") != string::npos) {


                for (int i = 0; i < dimension; i++) {
                    for (int j = 0; j < dimension; j++) {
                        file >> adjacencyMatrix[i][j];
                    }
                }
                break;
            }
        }


        file.close();
        return adjacencyMatrix;
    }

    static vector<vector<int>> loadingFromFileForVector(string nazwaPliku){
        static int numberOfVertices = 0;
        vector<vector<int>> adjacencyMatrix;

        ifstream file;
        file.open(nazwaPliku.c_str());

        if (!file.is_open()) {
            cerr << "File not found or cannot open it" << endl;
            throw runtime_error("File opening failed");
        }

        file >> numberOfVertices;

        adjacencyMatrix.resize(numberOfVertices, vector<int>(numberOfVertices, 0));

        for(int i = 0; i < numberOfVertices; i++){
            for(int j = 0; j < numberOfVertices; j++){
                file >> adjacencyMatrix[i][j];
            }
        }


        file.close();
        return adjacencyMatrix;
    }

    static void saveToFile(vector<int> path){
        ofstream plik;
        plik.open("solution_path.txt");

        plik << path.size() - 1 << endl;

        for(int i = 0; i < path.size(); i++){
            plik << path[i] << endl;
        }
        plik.flush();
        plik.close();
    }

    static vector<int> readFromFile() {
        ifstream plik("solution_path.txt"); // Otwieranie pliku do odczytu
        vector<int> path;

        if (!plik.is_open()) {
            throw runtime_error("Cannot open file solution_path.txt");
        }

        int dimension;
        plik >> dimension; // Wczytanie pierwszej liczby - liczby wierzchołków

        if (plik.fail() || dimension <= 0) { // Sprawdzenie, czy wczytano poprawną wartość
            throw runtime_error("Incorrect number of vertices in file.");
        }

        for (int i = 0; i < dimension+1; ++i) {
            int vertex;
            if (!(plik >> vertex)) { // Wczytanie kolejnych wierzchołków
                throw runtime_error("Unexpected end of file or format error while loading vertices.");
            }
            path.push_back(vertex); // Dodanie wierzchołka do wektora
        }

        plik.close(); // Zamknięcie pliku
        return path;
    }

    static vector<vector<int>> loadingFromXMLFile(const string& filename) {
        vector<vector<int>> adjacencyMatrix;

        // Load XML file
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(filename.c_str());
        if (!result) {
            cerr << "Failed to open XML file: " << result.description() << endl;
            throw runtime_error("Could not load XML file.");
        }

        // Locate the graph structure
        pugi::xml_node graph = doc.child("travellingSalesmanProblemInstance").child("graph");
        if (!graph) {
            throw runtime_error("XML file does not contain a valid 'graph' node.");
        }

        // Count vertices
        size_t vertexCount = 0;
        for (pugi::xml_node vertex = graph.child("vertex"); vertex; vertex = vertex.next_sibling("vertex")) {
            vertexCount++;
        }

        // Initialize adjacency matrix
        adjacencyMatrix.resize(vertexCount, vector<int>(vertexCount, 0));

        // Parse edges
        size_t i = 0;
        for (pugi::xml_node vertex = graph.child("vertex"); vertex; vertex = vertex.next_sibling("vertex"), i++) {
            size_t j = 0;
            for (pugi::xml_node edge = vertex.child("edge"); edge; edge = edge.next_sibling("edge"), j++) {
                double cost = edge.attribute("cost").as_double();
                adjacencyMatrix[i][j] = static_cast<int>(cost);
            }
        }

        return adjacencyMatrix;
    }





};

#endif
