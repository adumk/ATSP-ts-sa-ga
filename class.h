//
// Created by krisk on 15/10/2024.

#include "TSP.h"

#include <algorithm>
#include <iostream>
#include <cmath>
#include <complex>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <set>
#include <random>
#include <unordered_map>

/**
 * Calculates the cost of the path along the given path
 * @param city_matrix object that contains the matrix of cities
 * @param path array of cities
 * @return cost of the path
 */

int TSP::calculatePathCost(cityMatrix &city_matrix, Tour &tour) {
    int cost = 0;
    for (int i = 0; i < tour.path.size() - 2; ++i) {
        cost += city_matrix.matrix[tour.path[i]][tour.path[i + 1]];
    }
    cost += city_matrix.matrix[tour.path[tour.path.size() - 2]][tour.path[0]]; // cost of returning to the starting city
    return cost;
}

// method to find the greedy tour starting from the given city
Tour findGreedyTour(cityMatrix &city_matrix, int start){
    Tour tour;
    int size = city_matrix.size;
    tour.path.push_back(start);
    tour.totalCost = 0;

    std::vector<bool> visited(size+1, false);
    visited[start] = true;

    int current = start;

    for (int step = 1; step < size; step++) {
        int nearest = -1;
        int minCost = INT_MAX;

        for (int i = 0; i < size; i++) {
            if (!visited[i] && city_matrix.matrix[current][i] < minCost) {
                minCost = city_matrix.matrix[current][i];
                nearest = i;
            }
        }

        if (nearest == -1) {
            // No more cities to visit
            break;
        }

        // add the nearest city to the path
        tour.path.push_back(nearest);
        tour.totalCost += minCost;
        visited[nearest] = true;
        current = nearest;
    }

    // add the cost of the return to the starting city
    tour.path.push_back(start);
    tour.totalCost += city_matrix.matrix[current][start];

    return tour;
}

// method to find the best greedy tour
Tour findBestGreedyTour(cityMatrix &city_matrix) {
    Tour bestTour;
    bestTour.totalCost = INT_MAX;

    for (int start = 0; start < city_matrix.size; ++start) {
        Tour currentTour = findGreedyTour(city_matrix, start);
        if (currentTour.totalCost < bestTour.totalCost) {
            bestTour = currentTour;
        }
    }
    return bestTour;
}

std::pair<int, int> selectSubsequenceIndices(int size) {
    // Randomly select a subsequence
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, size-2);
    int start_index, end_index;
    // Generate the random indices
    do{
        start_index = dis(gen);
        end_index = dis(gen);
    } while (start_index >= end_index);
    return {start_index, end_index};
}

Tour generateRandomTour(cityMatrix& city_matrix) {
    Tour tour;
    tour.path = std::vector<int>(city_matrix.size);
    for (int i = 0; i < city_matrix.size; i++) {
        tour.path[i] = i;
    }
    std::random_shuffle(tour.path.begin()+1, tour.path.end()-1);
    tour.path.push_back(tour.path[0]);
    tour.totalCost = TSP::calculatePathCost(city_matrix, tour);
    return tour;
}

void initializePopulation(cityMatrix &city_matrix, std::vector<Tour> &population, int population_size) {
    int num_vertices = city_matrix.size;

    // Generate greedy solutions
    for (int start_vertex = 0; start_vertex < num_vertices; ++start_vertex) {
        Tour greedy_tour;
        greedy_tour = findGreedyTour(city_matrix, start_vertex);
        population.push_back(greedy_tour);
    }

    // Fill remaining population slots with random solutions
    while (population.size() < population_size) {
        Tour random_tour;
        random_tour = generateRandomTour(city_matrix);
        population.push_back(random_tour);
    }
}

Tour rouletteWheelSelection(std::vector<Tour>& population) {
    int totalCost = 0;
    for (Tour& tour : population) {
        totalCost += tour.totalCost;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, totalCost);

    // generate a random number between 0 and totalCost
    int random = dis(gen);

    int sum = 0;
    for (const Tour& tour : population) {
        sum += tour.totalCost;
        if (sum >= random) {
            return tour;
        }
    }
    return population.back(); // should never reach this point
}

std::pair<Tour, Tour> OXCrossover(cityMatrix& city_matrix, std::pair<Tour, Tour>& parents) {
    auto parent1 = parents.first;
    auto parent2 = parents.second;
    int size = parent1.path.size();
    Tour child1; child1.path = std::vector<int>(size, -1);
    Tour child2; child2.path = std::vector<int>(size, -1);

    auto indices = selectSubsequenceIndices(size);

    // Copy the subsequence from parent1 to child1 and parent2 to child2
    for (int i = indices.first; i <= indices.second; i++) {
        child1.path[i] = parent1.path[i];
        child2.path[i] = parent2.path[i];
    }

    // Helper lambda to fill remaining cities
    auto fillRemainingCities = [&](Tour& child, const Tour& otherParent) {
        int currentIndex = (indices.second + 1) % (size - 1);
        for (int i = 0; i < size - 1; i++) {
            int city = otherParent.path[(indices.second + 1 + i) % (size - 1)];
            if (std::find(child.path.begin(), child.path.end() - 1, city) == child.path.end() - 1) {
                child.path[currentIndex] = city;
                currentIndex = (currentIndex + 1) % (size - 1);
            }
        }
        child.path[size - 1] = child.path[0];
    };

    // Fill the remaining cities in both children
    fillRemainingCities(child1, parent2);
    fillRemainingCities(child2, parent1);

    // Calculate total cost for each child
    child1.totalCost = TSP::calculatePathCost(city_matrix, child1);
    child2.totalCost = TSP::calculatePathCost(city_matrix, child2);

    return {child1, child2};
}


std::pair<Tour, Tour> PMXCrossover(cityMatrix& city_matrix, std::pair<Tour, Tour>& parents) {
    auto parent1 = parents.first;
    auto parent2 = parents.second;
    Tour child1;
    Tour child2;

    int size = parent1.path.size();
    child1.path = std::vector<int>(size, -1);
    child2.path = std::vector<int>(size, -1);

    auto indices = selectSubsequenceIndices(size - 1);  // Exclude the last city for crossover
    // Copy the subsequence from each parent
    for (int i = indices.first; i <= indices.second; i++) {
        child1.path[i] = parent2.path[i];
        child2.path[i] = parent1.path[i];
    }

    // Create mappings for the swapped subsequences
    std::unordered_map<int, int> mapping1, mapping2;
    for (int i = indices.first; i <= indices.second; i++) {
        mapping2.insert({parent1.path[i], parent2.path[i]});
        mapping1.insert({parent2.path[i], parent1.path[i]});
    }

    // Resolve conflicts and fill remaining positions
    for (int i = 0; i < size - 1; i++) {
        if (i >= indices.first && i <= indices.second) continue; // Skip the subsequence

        int city1 = parent1.path[i];
        int city2 = parent2.path[i];

        while (std::find(child1.path.begin(), child1.path.end() - 1, city1) != child1.path.end() - 1) {
            city1 = mapping1.at(city1);
        }
        while (std::find(child2.path.begin(), child2.path.end() - 1, city2) != child2.path.end() - 1) {
            city2 = mapping2.at(city2);
        }

        child1.path[i] = city1;
        child2.path[i] = city2;
    }
    // Fill the last city
    child1.path[size - 1] = child1.path[0];
    child2.path[size - 1] = child2.path[0];

    child1.totalCost = TSP::calculatePathCost(city_matrix, child1);
    child2.totalCost = TSP::calculatePathCost(city_matrix, child2);
    return {child1, child2};
}

std::pair<Tour,Tour> crossover(cityMatrix &city_matrix, std::pair<Tour,Tour>& parents, crossoverType type) {
    if (type == crossoverType::OX) {
        return OXCrossover(city_matrix, parents);
    }
    if (type == crossoverType::PMX) {
        return PMXCrossover(city_matrix, parents);
    }
    return {parents.first, parents.second}; // should never reach this point
}

void mutate(cityMatrix city_matrix, std::vector<Tour>& population, mutationType type) {
    // Seed the random number generator with a random device
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (auto& tour : population) {
        if (dist(rng) < city_matrix.mutation_factor) {
            if (type == mutationType::SWAP) {
                auto indices = selectSubsequenceIndices(tour.path.size());
                std::swap(tour.path[indices.first], tour.path[indices.second]);
                tour.totalCost = TSP::calculatePathCost(city_matrix, tour);
            } else if (type == mutationType::INVERSION) {
                auto indices = selectSubsequenceIndices(tour.path.size());
                std::reverse(tour.path.begin() + indices.first, tour.path.begin() + indices.second);
                tour.totalCost = TSP::calculatePathCost(city_matrix, tour);
            }
        }
    }
}

#define OPTIMAL_170 2755
#define OPTIMAL_55 1608
double calculateAverageError(std::vector<Tour>& population) {
    double errors_sum = 0;
    for (const Tour &tour: population) {
        errors_sum += (double)std::abs(tour.totalCost - OPTIMAL_170)/OPTIMAL_170;
    }
    return errors_sum/(double)population.size();

}

std::pair<Tour, Tour> selectParents(std::vector<Tour>& population) {
    Tour parent1 = rouletteWheelSelection(population);
    Tour parent2 = rouletteWheelSelection(population);

    return {parent1, parent2};
}

#define INTERVAL 50
Tour TSP::geneticAlgorithm(cityMatrix& city_matrix, int population_size, crossoverType crossover_type, mutationType mutation_type, std::string output_filename) {
    std::vector<Tour> population = std::vector<Tour>();
    std::vector<Tour> new_population = std::vector<Tour>();
    int parent_number = population_size * city_matrix.crossover_factor;

    initializePopulation(city_matrix, population, population_size);

    // Prepare the output file
    std::ofstream output_file(output_filename);
    output_file << "GA\n" << city_matrix.size-1 << "\n";

    Tour best_tour = population[0];
    double best_error = calculateAverageError(population);
    auto start = std::chrono::steady_clock::now();
    auto stop = start + city_matrix.stop_criterion_time;
    output_file << 0.0 << "; " << best_tour.totalCost << "; " << calculateAverageError(population) << "\n";

    auto next_record_time = start + std::chrono::milliseconds(INTERVAL);

    // Main loop
    while (std::chrono::steady_clock::now() < stop) {
        // Perform crossover on crossover_factor of population
        for (int i = 0; i < parent_number; ++i) {
            auto parents = selectParents(population);
            std::pair<Tour,Tour> children = crossover(city_matrix, parents, crossover_type);
            population.push_back(children.first);
            population.push_back(children.second);
        }
        //apply mutationSwap
        mutate(city_matrix, population, mutation_type);

        // Sort the population by the cost of the tours
        sort(population.begin(), population.end(), [](const Tour &a, const Tour &b) {
            return a.totalCost < b.totalCost;
        });

        // Fill the new population with the best tours
        int iterator = 0;
        while (new_population.size() < population_size) {
            new_population.push_back(population[iterator]);
            iterator++;
        }

        population.clear();
        std::swap(population, new_population);


        // Check if it's time to record the path cost
        auto now = std::chrono::steady_clock::now();
        if (now >= next_record_time) {
            double average_error = calculateAverageError(population);
            auto elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            output_file << elapsed_seconds << "; " << population.front().totalCost << "; " << average_error << "\n";
            next_record_time += std::chrono::milliseconds(INTERVAL);
        }

        if (population[0].totalCost < best_tour.totalCost) {
            best_tour = population[0];
            best_error = calculateAverageError(population);
            std::cout << "Better solution found: " << population.front().totalCost << std::endl;

        }
    }
    output_file << best_tour.totalCost << "; " << calculateAverageError(population) << "\n";

    for (auto city: best_tour.path) {
        output_file << city << " ";
    }

    output_file.close();

    std::cout << "Best tour cost: " << best_tour.totalCost << std::endl;
    std::cout << "Best average error: " << best_error << std::endl;

    for (int i = 0; i < best_tour.path.size(); i++) {
        std::cout << best_tour.path[i] << " ";
    }

    return best_tour;
}
