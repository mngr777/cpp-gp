#include "indiv.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <random>

double fitness_combine_sum_abs(std::vector<double> diff) {
    double sum;
    for (auto d : diff)
        sum += std::fabs(d);
    return sum;
}

double fitness_combine_sum_squared(std::vector<double> diff) {
    double sum;
    for (auto d : diff)
        sum += d * d;
    return sum;
}

Indiv& Indiv::operator=(const Indiv& other) {
    tree_ = other.tree_;
    has_fitness_ = other.has_fitness_;
    fitness_ = other.fitness_;
    return *this;
}

Indiv& Indiv::operator=(Indiv&& other) {
    tree_ = std::move(other.tree_);
    has_fitness_ = other.has_fitness_;
    fitness_ = other.fitness_;
    return *this;
}

void Indiv::eval(
    const FitnessCaseList& fitness_cases,
    const FitnessCombine& combine)
{
    std::vector<double> diff(fitness_cases.size()); // deviations
    for (const auto& fc : fitness_cases)
        diff.push_back(tree_.get_value(fc.first) - fc.second);
    fitness_ = combine(diff);
    has_fitness_ = true;
}

void Indiv::eval(const FitnessCaseList& fitness_cases) {
    return eval(fitness_cases, fitness_combine_sum_squared);
}

Indiv crossover(
    const Indiv& p1,
    const Indiv& p2,
    float p_term)
{
    // std::cout << "Crossover {" << std::endl;
    Tree t = p1.tree();
    t.random_subtree(p_term) =
        p2.tree().random_subtree(p_term);
    // std::cout << "  parent 1:" << std::endl;
    // std::cout << p1.tree().as_pretty_string() << std::endl;
    // std::cout << "  parent 2:" << std::endl;
    // std::cout << p2.tree().as_pretty_string() << std::endl;
    // std::cout << "  child:" << std::endl;
    // std::cout << t.as_pretty_string() << std::endl;
    // std::cout << "}" << std::endl;
    return Indiv(std::move(t));
}

Population make_pop_ramped_hnh(
    const TermList& term_list,
    const FuncList& func_list,
    unsigned depth,
    std::size_t n)
{
    std::size_t n_full = n / 2;
    // std::size_t n_grow = n - n_full;

    Population pop;
    for (std::size_t i = 0; i < n; ++i)
        pop.emplace_back(
            ((i < n_full) ? full : grow)(term_list, func_list, depth));
    return pop;
}

const Indiv& tournament(const Population& pop) {
    // assertion: all population has been evaluated
    assert(
        std::all_of(
            pop.begin(),
            pop.end(),
            [](const Indiv& indiv) {
                return indiv.has_fitness();
            }));
    assert(pop.size() > 0);

    // select 2 contestants at random
    std::random_device rd;
    std::uniform_int_distribution<std::size_t> distr(0, pop.size() - 1);
    const Indiv& cont1 = pop[distr(rd)];
    const Indiv& cont2 = pop[distr(rd)];
    // more fit individual wins
    return (cont1.fitness() < cont2.fitness()) ? cont1 : cont2;
}
