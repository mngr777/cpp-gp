#ifndef GPTEST_EXPR_HPP_
#define GPTEST_EXPR_HPP_

#include <iostream>

#include <cassert>
#include <functional>
#include <iterator>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

typedef std::vector<double> Params;

class Expr;
class Term;
class Func;

typedef std::vector<std::shared_ptr<Term>> TermList;
typedef std::vector<std::shared_ptr<Func>> FuncList;

template <typename C>
const typename C::value_type& random_element(const C& c) {
    auto it = c.cbegin();
    if (it == c.cend())
        throw std::logic_error("Container is empty");
    std::random_device rd;
    std::uniform_int_distribution<int> distr(0, std::distance(it, c.cend()) - 1);
    std::advance(it, distr(rd));
    return *it;
}

class Expr {
public:
    Expr(const std::string& name)
        : name_(name) {}

    virtual ~Expr() = 0;

    virtual unsigned arity() const = 0;

    virtual bool is_term() const = 0;

    virtual bool is_func() const = 0;

    virtual double eval(const Params&) const = 0;

    virtual std::string get_name() {
        return name_;
    }

protected:
    std::string name_;
};


class Term : public Expr {
public:
    Term(int id, const std::string& name)
        : Expr(name),
          id_(id) {}

    virtual unsigned arity() const {
        return 0;
    }

    virtual bool is_term() const {
        return true;
    }

    virtual bool is_func() const {
        return false;
    }

    virtual double eval(const Params& params) const {
        return params[id_];
    }

private:
    int id_;
    std::string name_;
};

class Func : public Expr {
public:
    Func(
        std::function<double(const Params&)> func,
        unsigned arity,
        std::string name)
        : Expr(name),
          func_(func),
          arity_(arity) {}

    virtual unsigned arity() const {
        return arity_;
    }

    virtual bool is_term() const {
        return false;
    }

    virtual bool is_func() const {
        return true;
    }

    virtual double eval(const Params& args) const {
        return func_(args);
    }

private:
    std::function<double(const Params&)> func_;
    int arity_;
};

#endif
