#ifndef GPTEST_INDIV_HPP_
#define GPTEST_INDIV_HPP_

#include <cstddef>
#include <functional>
#include <utility>
#include <vector>
#include "expr.hpp"
#include "tree.hpp"

typedef std::pair<Params, double> FitnessCase;
typedef std::vector<FitnessCase> FitnessCaseList;
typedef std::function<double(std::vector<double>)> FitnessCombine;

double fitness_combine_sum_abs(std::vector<double> diff);

double fitness_combine_sum_squared(std::vector<double> diff);

class Indiv {
public:
    explicit Indiv(const Tree& tree)
        : tree_(tree),
          has_fitness_(false) {}

    explicit Indiv(Tree&& tree)
        : tree_(std::move(tree)),
          has_fitness_(false) {}

    Indiv(const Indiv& other)
        : tree_(other.tree_),
          has_fitness_(other.has_fitness_),
          fitness_(other.fitness_) {}

    Indiv(Indiv&& other)
        : tree_(std::move(other.tree_)),
          has_fitness_(other.has_fitness_),
          fitness_(other.fitness_) {}

    Indiv& operator=(const Indiv& other);

    Indiv& operator=(Indiv&& other);

    void eval(
        const FitnessCaseList& fitness_cases,
        const FitnessCombine& combine);

    void eval(const FitnessCaseList& fitness_cases);

    bool has_fitness() const {
        return has_fitness_;
    }

    double fitness() const {
        if (!has_fitness_)
            throw std::logic_error("Individual has not been evaluated");
        return fitness_;
    }

    const Tree& tree() const {
        return tree_;
    }

private:
    Tree tree_;
    bool has_fitness_;
    double fitness_;
};

Indiv crossover(
    const Indiv& p1,
    const Indiv& p2,
    float p_term = 0.1);

typedef std::vector<Indiv> Population;

Population make_pop_ramped_hnh(
    const TermList& term_list,
    const FuncList& func_list,
    unsigned depth,
    std::size_t n);

const Indiv& tournament(const Population& pop);

#endif
