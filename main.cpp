#include <iostream>
#include <utility>
#include "FileService.cpp"
#include "Graph.cpp"
#include "GreedyMethod.cpp"
#include "TabuSearch.cpp"
#include "SimulatedAnnealing.cpp"
#include "GeneticAlgorithm.cpp"

string GreedyMethod::resultPath = "";

using namespace std;

vector<vector<int>> graph;

int main() {
    int wybor;
    string nazwaPliku;
    vector<int> resultsValues;
    vector<string> resultsPaths;
    vector<int> firstPath;
    vector<vector<int>> checkedPaths;
    double stop = 0;
    int neighborhoodChoice;
    int Avalue;
    vector<int> path;
    double timeWhenFound;
    string crossoverType;
    string mutationType;
    int initPopulationSize;
    double mutationFactor;
    double crossoverFactor;
    while (true) {
        start:
        cout << endl << endl;
        cout << "Choose option: " << endl;
        cout << "1. Load data from file" << endl;
        cout << "2. Introduce the stop criterium (number of seconds)" << endl;
        cout << "3. Calculate the result by greedy method" << endl;
        cout << "4. Choose neighbourhood for Tabu Search" << endl;
        cout << "5. Run the Tabu Search algorithm for the data loaded and the parameters set" << endl;
        cout << "6. Set up the cooling factor for the Simulated Annealing algorithm" << endl;
        cout << "7. Run the Simulated Annealing algorithm for the data loaded and the parameters set" << endl;
        cout << "8. Save the solution path to the file" << endl;
        cout << "9. Load a solution path from the file and calculate the path from the loaded cost table" << endl;
        cout << "10. Load XML file" << endl;
        cout << "11. Set initial population size" << endl;
        cout << "12. Set the mutation factor" << endl;
        cout << "13. Set the crossover rate" << endl;
        cout << "14. Choose crossover method" << endl;
        cout << "15. Choose mutation method" << endl;
        cout << "16. Run genetic algorithm" << endl;

        cin >> wybor;

        switch (wybor) {

            case 1: {
                cout << "Provide file name: " << endl;
                cin >> nazwaPliku;
                graph = FileService::loadingFromFile(nazwaPliku);

                ifstream file;
                file.open(nazwaPliku.c_str());
                string line;
                while(getline(file, line)) { ;
                    std::cout << line;
                }
                file.close();

//                Graph::matrixViewForVector(graph, 56);
                break;
            }
            case 2: {
                cout << "Provide number of seconds: " << endl;
                cin >> stop;
                break;
            }
            case 3: {
                firstPath = GreedyMethod::mainCalculations(graph, resultsValues, resultsPaths);
                break;
            }
            case 4: {
                cout << "1. Moving one vertex to another location" << endl;
                cout << "2. Swap two vertices" << endl;
                cout << "3. Reversal of the order of vertices on a certain route subsection" << endl;
                cin >> neighborhoodChoice;
                break;
            }
            case 5:{
                firstPath = GreedyMethod::mainCalculations(graph, resultsValues, resultsPaths);
                if(neighborhoodChoice == 1) {
                        pair<vector<int>, int> results = TabuSearch::tabuSearchOneVertexChange(stop, firstPath, graph,
                                                                                               graph.size(), 10, timeWhenFound);
                        path = results.first; // Ścieżka (trasa)
                        int cost = results.second;              // Koszt

                        cout << "================================" << endl;
                        cout << "Best path: ";
                        for (size_t i = 0; i < path.size(); i++) {
                            cout << path[i];
                            if (i < path.size() - 1) {
                                cout << " ";
                            }
                        }
                        cout << endl;

                        cout << "Cost: " << cost << endl;

                        ofstream plik;
                        plik.open("overall_results.txt", ios::app);
                        plik << "Change one vertex" << endl;
                        plik << cost << endl;
                        plik.flush();
                        plik.close();
                }
                else if(neighborhoodChoice == 2){
                    pair<vector<int>, int> results = TabuSearch::tabuSearchSwapPlaces(stop, firstPath, graph,
                                                                                           graph.size(), 10, timeWhenFound);
                    path = results.first;
                    int cost = results.second;

                    cout << "================================" << endl;
                    cout << "Best path: ";
                    for (size_t i = 0; i < path.size(); i++) {
                        cout << path[i];
                        if (i < path.size() - 1) {
                            cout << " ";
                        }
                    }
                    cout << endl;

                    cout << "Cost: " << cost << endl;

                    ofstream plik;
                    plik.open("overall_results.txt", ios::app);
                    plik << "Swap places" << endl;
                    plik << cost << endl;
                    plik.flush();
                    plik.close();
                }
                else if(neighborhoodChoice == 3){
                    pair<vector<int>, int> results = TabuSearch::tabuSearchReverseOrder(stop, firstPath, graph,
                                                                                      graph.size(), 10, timeWhenFound);
                    path = results.first; // Ścieżka (trasa)
                    int cost = results.second;              // Koszt

                    cout << "================================" << endl;
                    cout << "Best path: ";
                    for (size_t i = 0; i < path.size(); i++) {
                        cout << path[i];
                        if (i < path.size() - 1) {
                            cout << " ";
                        }
                    }
                    cout << endl;

                    cout << "Cost: " << cost << endl;

                    ofstream plik;
                    plik.open("overall_results.txt", ios::app);
                    plik << "Reverse Order" << endl;
                    plik << cost << endl;
                    plik.flush();
                    plik.close();
                }



                break;
            }
            case 6: {
                cout << "Choose value of an 'a' factor: " << endl;
                cout << "1. 0.95" << endl;
                cout << "2. 0.98" << endl;
                cout << "3. 0.99" << endl;
                cin >> Avalue;
                break;
            }
            case 7: {
                firstPath = GreedyMethod::mainCalculations(graph, resultsValues, resultsPaths);
                pair<vector<int>, int> results;
                if(Avalue == 1) {
                    results = SimulatedAnnealing::simulatedAnnealing(stop, firstPath, graph,
                                                                                            graph.size(), 0.95, timeWhenFound);
                }
                else if(Avalue == 2){
                    results = SimulatedAnnealing::simulatedAnnealing(stop, firstPath, graph,
                                                                                            graph.size(), 0.98, timeWhenFound);
                }
                else if(Avalue == 3){
                    results = SimulatedAnnealing::simulatedAnnealing(stop, firstPath, graph,
                                                                                            graph.size(), 0.99, timeWhenFound);
                }
                path = results.first; // Ścieżka (trasa)
                int cost = results.second;              // Koszt

                cout << "=============SIMULATED ANNEALING===================" << endl;
                cout << "Best path: ";
                for (size_t i = 0; i < path.size(); i++) {
                    cout << path[i];
                    if (i < path.size() - 1) {
                        cout << " ";
                    }
                }
                cout << endl;

                cout << "Cost: " << cost << endl;

                ofstream plik;
                plik.open("overall_results.txt", ios::app);
                plik << "Simulated annealing" << endl;
                plik << cost << endl;
                plik.flush();
                plik.close();

                break;
            }
            case 8: {
                FileService::saveToFile(path);
                break;
            }
            case 9:{
                vector<int> readPath = FileService::readFromFile();
                if(graph.empty()){
                    cout << "Cost table wasn't loaded" << endl;
                    break;
                }
                int readCost = TabuSearch::calculateCost(readPath, graph, graph.size());
                cout << "Cost of the path read from a file: " << readCost << endl;

                break;
            }
                case 10: {
                    try {
                    cout << "Provide file name: " << endl;
                    cin >> nazwaPliku;
                    graph = FileService::loadingFromXMLFile(nazwaPliku);
                    for (const auto& row : graph) {
                        for (int cost : row) {
                            cout << cost << " ";
                        }
                        cout << endl;
                    }
                    } catch (const exception& e) {
                        cerr << e.what() << endl;
                    }
                }
                case 11:{
                    cout << " Provide the size of the initial population:\n";
                    cin >> initPopulationSize;

                    break;
                }
                case 12:{
                    cout << " Provide mutation factor:\n";
                    cin >> mutationFactor;

                    break;
                }
                case 13:{
                    cout << " Provide crossover rate:\n";
                    cin >> crossoverFactor;

                    break;
                }
                case 14:{
                    cout << "\nOX or EX\n";
                    cin >> crossoverType;

                    break;
                }
                case 15:{
                    cout << "\nswap or insert\n";
                    cin >> mutationType;

                    break;
                }
                case 16:{
                    GeneticAlgorithm2::geneticAlgorithm(graph, initPopulationSize, crossoverType, mutationType, crossoverFactor, mutationFactor, stop);
                    break;
                }
            }
        }
    }

