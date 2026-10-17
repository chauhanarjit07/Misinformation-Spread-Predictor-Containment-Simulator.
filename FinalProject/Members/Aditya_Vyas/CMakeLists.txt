cmake_minimum_required(VERSION 3.10)
project(rumorguard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
set(SOURCES
    src/graph/Graph.cpp
    src/graph/BarabasiAlbert.cpp
    src/features/FeatureCompute.cpp
    src/simulation/SpreadSimulator.cpp
    src/strategies/NoIntervention.cpp
    src/strategies/DegreeBased.cpp
    src/strategies/CentralityBased.cpp
    src/strategies/GreedyInfluence.cpp
    src/evaluation/main.cpp
)

# Header include directories
include_directories(src)

add_executable(rumorguard ${SOURCES})

# Compilation warning flags
if(MSVC)
    target_compile_options(rumorguard PRIVATE /W4)
else()
    target_compile_options(rumorguard PRIVATE -Wall -Wextra -pedantic)
endif()
