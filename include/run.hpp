#ifndef GPTEST_RUN_HPP_
#define GPTEST_RUN_HPP_

#include <cstddef>
#include <utility>
#include "expr.hpp"
#include "indiv.hpp"

class Run {
public:
    Run()
        : population_size_(0),
          generation_number_(0),
          crossover_rate_(0.9),
          fitness_goal_(0.01),
          fitness_combine_method_(fitness_combine_sum_abs),
          generation_(0) {}

    bool finished();

    bool solution_found();

    void next_generation();

    Population harvest();

    double avg_fitness();

    std::pair<double, double> best_worst_fitness();

    void set_generation_number(unsigned generation_number) {
        generation_number_ = generation_number;
    }

    void set_population(const Population& population) {
        population_ = population;
        population_size_ = population_.size();
    }

    void set_population(Population&& population) {
        population_ = population;
        population_size_ = population_.size();
    }

    const Population& population() const {
        return population_;
    }

    std::size_t population_size() const {
        return population_size_;
    }

    void set_crossover_rate(float crossover_rate) {
        crossover_rate_ = crossover_rate;
    }

    void set_fitness_goal(double fitness_goal) {
        fitness_goal_ = fitness_goal;
    }

    void add_terminal(int id, const std::string& name) {
        terminals_.push_back(std::make_shared<Term>(id, name));
    }

    const TermList& terminals() {
        return terminals_;
    }

    const std::size_t terminal_num() const {
        return terminals_.size();
    }

    void add_function(
        std::function<double(const Params&)> f,
        unsigned arity,
        std::string name)
    {
        functions_.push_back(std::make_shared<Func>(f, arity, name));
    }

    const FuncList& functions() {
        return functions_;
    }

    const std::size_t function_num() const {
        return terminals_.size();
    }

    void add_fitness_case(const Params& params, double target) {
        fitness_cases_.emplace_back(params, target);
    }

    void set_fitness_combine_method(
        std::function<double(std::vector<double>)> method)
    {
        fitness_combine_method_ = method;
    }

    unsigned generation() {
        return generation_;
    }

private:
    void validate();

    void eval_population();

    std::size_t population_size_;
    unsigned generation_number_;
    float crossover_rate_;
    double fitness_goal_;
    TermList terminals_;
    FuncList functions_;
    FitnessCaseList fitness_cases_;
    FitnessCombine fitness_combine_method_;
    unsigned generation_;
    Population population_;
    Population population_next_;
};

#endif
