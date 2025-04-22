cmake_minimum_required(VERSION 3.27)
project(PEA_projekt2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "-static")


add_executable(PEA_projekt2 main.cpp
        FileService.cpp
        Graph.cpp
        GreedyMethod.cpp
        TabuSearch.cpp
        SimulatedAnnealing.cpp
	pugixml.cpp
        class.h
        GeneticAlgorithm.cpp)
