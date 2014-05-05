#ifndef GPTEST_TREE_HPP_
#define GPTEST_TREE_HPP_

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "expr.hpp"

class Tree  {
public:
    Tree()
        : parent_(nullptr),
          expr_(nullptr),
          children_(0) {}

    explicit Tree(const std::shared_ptr<Expr>& expr)
        : parent_(nullptr),
          expr_(expr),
          children_(expr_ ? expr_->arity() : 0) {}

    Tree(const Tree& other);

    Tree(Tree&& other);

    Tree& operator=(const Tree& other);

    Tree& operator=(Tree&& other);

    bool empty() const {
        return !expr_;
    }

    bool valid() const;

    bool root() const {
        return !parent_;
    }

    double get_value(const Params& params) const;

    std::size_t child_num() const {
        return children_.size();
    }

    std::size_t size() {
        return term_num() + func_num();
    }

    std::size_t term_num() const;

    std::size_t func_num() const;

    void set_child(std::size_t index, Tree subtree);

    Tree& random_subtree(float p_term = 0.1);

    const Tree& random_subtree(float p_term = 0.1) const;

    Tree& random_term();

    const Tree& random_term() const;

    Tree& nth_term(std::size_t n);

    const Tree& nth_term(std::size_t n) const;

    Tree& random_func();

    const Tree& random_func() const;

    Tree& nth_func(std::size_t n);

    const Tree& nth_func(std::size_t n) const;

    std::string as_string() const;

    std::string as_pretty_string() const {
        return do_as_pretty_string(0);
    }

private:
    std::string do_as_pretty_string(std::size_t offset) const;

    Tree* parent_;
    std::shared_ptr<Expr> expr_;
    std::vector<Tree> children_;
};

Tree full(const TermList& term_list, const FuncList& func_list, unsigned depth);

Tree grow(const TermList& term_list, const FuncList& func_list, unsigned depth);

#endif
