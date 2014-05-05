#include "run.hpp"
#include <algorithm>
#include <functional>
#include <limits>

bool Run::finished() {
    return (generation_ == generation_number_)
        || solution_found();
}

bool Run::solution_found() {
    assert(population_.size() > 0);
    eval_population();
    return std::any_of(
        population_.begin(), population_.end(),
        [this](const Indiv& indiv) {
            return indiv.fitness() < fitness_goal_;
        });
}

void Run::next_generation() {
    validate();
    eval_population();

    // crossover
    population_next_.clear();
    while (population_next_.size() < population_size_ * crossover_rate_)
        population_next_.push_back(
            crossover(tournament(population_), tournament(population_)));

    // reproduction
    while (population_next_.size() < population_size_)
        population_next_.push_back(tournament(population_));

    // swap generations
    population_.swap(population_next_);
    population_next_.clear();

    // update generation counter
    ++generation_;
}

Population Run::harvest() {
    eval_population();
    Population harv;
    for (const Indiv& indiv : population_)
        if (indiv.fitness() < fitness_goal_)
            harv.push_back(indiv);
    return harv;
}

double Run::avg_fitness() {
    eval_population();
    double sum = 0.0;
    for (const Indiv& indiv : population_)
        sum += indiv.fitness();
    return sum / population_size_;
}

std::pair<double, double> Run::best_worst_fitness() {
    eval_population();
    double best = std::numeric_limits<double>::max();
    double worst = 0.0;
    for (const Indiv& indiv : population_) {
        best = std::min(best, indiv.fitness());
        worst = std::max(worst, indiv.fitness());
    }
    return std::make_pair(best, worst);
}

void Run::validate() {
    if (generation_number_ < 1)
        throw std::logic_error("Generation number is not set");
    if (population_size_ < 1)
        throw std::logic_error("Initial population not set");
    if (fitness_cases_.empty())
        throw std::logic_error("No fitness cases provided");
}

void Run::eval_population() {
    for (auto& indiv : population_) {
        if (!indiv.has_fitness())
            indiv.eval(fitness_cases_, fitness_combine_method_);
    }
}


