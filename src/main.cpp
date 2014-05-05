#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <functional>
#include <vector>
#include "expr.hpp"
#include "indiv.hpp"
#include "func.hpp"
#include "run.hpp"

double square1(const Params& args) {
    return args[0] * args[0];
}

const std::size_t PopulationSize = 500;
const unsigned GenerationNumber = 50;
const float CrossoverRate = 0.9;
const double FitnessGoal = 0.01;
const unsigned InitialDepth = 3;

int main() {
    Run run;
    run.set_generation_number(GenerationNumber);
    run.set_crossover_rate(CrossoverRate);
    run.set_fitness_goal(FitnessGoal);

    // set terminals
    run.add_terminal(0, "a");
    // set functions
    run.add_function(plus2, 2, "+");
    run.add_function(minus2, 2, "-");
    run.add_function(mult2, 2, "*");
    run.add_function(safe_div2, 2, "%");
    run.add_function(sin1, 1, "sin");
    run.add_function(cos1, 1, "cos");
    run.add_function(rlog1, 1, "rlog");
    run.add_function(exp1, 1, "exp");

    // generage fitness cases
    std::random_device rand_dev;
    std::uniform_real_distribution<double> param_distr(-2.0, 2.0);
    for (std::size_t i = 0; i < 20; ++i) {
        // generate random parameters
        Params p;
        for (std::size_t k = 0; k < run.terminal_num(); ++k)
            p.push_back(param_distr(rand_dev));
        // a^4 + a^3  + a^2 + a
        double target =
            p[0]*p[0]*p[0]*p[0]
            + p[0]*p[0]*p[0]
            + p[0]*p[0]
            + p[0];
        run.add_fitness_case(p, target);
    }

    // initial population
    run.set_population(
        make_pop_ramped_hnh(
            run.terminals(), run.functions(),
            InitialDepth,
            PopulationSize));

    // run
    do {
        std::cout << "[Generation " << run.generation() << "]" << std::endl;
        std::cout << "Avg.  fitness: " << run.avg_fitness() << std::endl;
        auto best_worst = run.best_worst_fitness();
        std::cout << "Best  fitness: " << best_worst.first << std::endl;
        std::cout << "Worst fitness: " << best_worst.second << std::endl;
        run.next_generation();
    } while (!run.finished());

    if (run.solution_found()) {
        std::cout << "HARVEST:" << std::endl;
        std::cout << std::endl;
        for (const Indiv& indiv : run.harvest()) {
            std::cout << indiv.tree().as_pretty_string() << std::endl;
            std::cout << "Fitness: " << indiv.fitness() << std::endl;
            std::cout << std::endl;
        }
    } else {
        std::cout << "NO FIT INDIVIDUALS FOUND" << std::endl;
    }

    // SHOW SOME SAMPLE INDIVIDUALS
    // collect indiv pointers
    std::vector<const Indiv*> pop_ptrs(run.population_size());
    std::transform(
        run.population().begin(), run.population().end(),
        pop_ptrs.begin(),
        [](const Indiv& indiv) {
            return &indiv;
        });
    // sort indiv pointers by fitness
    std::sort(
        pop_ptrs.begin(), pop_ptrs.end(),
        [](const Indiv* ip1, const Indiv* ip2) {
            return ip1->fitness() < ip2->fitness();
        });


    std::size_t sample_size = 3;
    assert(pop_ptrs.size() >= sample_size);

    // best individuals
    std::cout << "BEST " << sample_size << ":" << std::endl;;
    for (std::size_t i = 0; i < sample_size; ++i) {
        std::cout << i << ":" << std::endl;
        std::cout << pop_ptrs[i]->tree().as_pretty_string() << std::endl;
        std::cout << "Fitness: " << pop_ptrs[i]->fitness() << std::endl;
        std::cout << std::endl;
    }

    // worst individuals
    std::cout << "WORST " << sample_size << ":" << std::endl;;
    for (std::size_t i = pop_ptrs.size() - 1;
         i > pop_ptrs.size() - 1 - sample_size;
         --i
    ) {
        std::cout << i << ":" << std::endl;
        std::cout << pop_ptrs[i]->tree().as_pretty_string() << std::endl;
        std::cout << "Fitness: " << pop_ptrs[i]->fitness() << std::endl;
        std::cout << std::endl;
    }
}
