#include "tree.hpp"

// TODO: use initialization list
Tree::Tree(const Tree& other) {
    parent_ = nullptr;
    expr_ = other.expr_;
    children_ = other.children_;
}

// TODO: use initialization list
Tree::Tree(Tree&& other) {
    parent_ = nullptr;
    expr_ = std::move(other.expr_);
    children_ = std::move(other.children_);
}

Tree& Tree::operator=(const Tree& other) {
    parent_ = nullptr;
    expr_ = other.expr_;
    children_ = other.children_;
    return *this;
}

Tree& Tree::operator=(Tree&& other) {
    parent_ = nullptr;
    expr_ = std::move(other.expr_);
    children_ = std::move(other.children_);
    return *this;
}

bool Tree::valid() const {
    if (empty()) {
        return false;
    } else if (expr_->is_term()) {
        return true;
    } else if (expr_->is_func()) {
        // TODO: replace with std::any_of
        auto it = std::find_if(
            children_.cbegin(),
            children_.cend(),
            [](const Tree& t) {
                return !t.valid();
            });
        return it == children_.cend();
    }
    assert(false);
}

double Tree::get_value(const Params& params) const {
    assert(valid());

    if (expr_->is_term()) {
        return expr_->eval(params);

    } else if (expr_->is_func()) {
        Params args(children_.size());
        std::transform(
            children_.cbegin(), children_.cend(),
            args.begin(),
            [=](const Tree& c) {
                return c.get_value(params);
            });
        return expr_->eval(args);
    }
    assert(false);
}

std::size_t Tree::term_num() const {
    assert(!empty());

    if (expr_->is_term()) {
        return 1;
    } else {
        std::size_t num = 0;
        for (const auto& c : children_)
            num += c.term_num();
        return num;
    }
}

std::size_t Tree::func_num() const {
    assert(!empty());

    if (expr_->is_term()) {
        return 0;
    } else {
        std::size_t num = 1;
        for (const auto& c : children_)
            num += c.func_num();
        return num;
    }
}

void Tree::set_child(std::size_t index, Tree subtree) {
    if (index >= child_num())
        throw std::out_of_range("Invalid node child index");
    children_[index] = std::move(subtree);
    children_[index].parent_ = this;
}

Tree& Tree::random_subtree(float p_term) {
    return const_cast<Tree&>(
        const_cast<const Tree*>(this)->random_subtree(p_term));
}

const Tree& Tree::random_subtree(float p_term) const {
    // std::cout << "random_subtree: P[term] = " << p_term << std::endl;
    assert(term_num() > 0);
    assert(p_term < 1.0);
    // std::cout << "func num = " << func_num() << std::endl;
    // std::cout << "term num = " << term_num() << std::endl;

    // check if tree is terminal
    if (func_num() == 0)
        return *this;

    std::random_device rd;
    std::uniform_real_distribution<float> distr(0, 1.0);
    return (distr(rd) < p_term)
        ? random_term()
        : random_func();
}

Tree& Tree::random_term() {
    return const_cast<Tree&>(
        const_cast<const Tree*>(this)->random_term());
}

const Tree& Tree::random_term() const {
    assert(term_num() > 0);

    std::random_device rd;
    std::uniform_int_distribution<std::size_t> distr(
        0, term_num() - 1);
    std::size_t tn = distr(rd);
    // std::cout << "random term: " << tn << std::endl;
    return nth_term(tn);
}

Tree& Tree::nth_term(std::size_t n) {
    return const_cast<Tree&>(
        const_cast<Tree*>(this)->nth_term(n));
}

const Tree& Tree::nth_term(std::size_t n) const {
    assert(!empty());
    if (n + 1 > term_num())
        throw std::out_of_range("Terminal number is out of range");

    if (n == 0) {
        return expr_->is_term()
            ? *this
            : children_[0].nth_term(0);
    } else {
        std::size_t skipped = 0;
        for (auto& c : children_) {
            if (skipped + c.term_num() > n)
                return c.nth_term(n - skipped);
            skipped += c.term_num();
        }
        assert(false);
    }
}

Tree& Tree::random_func() {
    return const_cast<Tree&>(
        const_cast<Tree*>(this)->random_func());
}

const Tree& Tree::random_func() const {
    assert(func_num() > 0);

    std::random_device rd;
    std::uniform_int_distribution<std::size_t> distr(
        0, func_num() - 1);
    std::size_t fn = distr(rd);
    // std::cout << "random func: " << fn << " from " << func_num() << std::endl;
    return nth_func(fn);
}

Tree& Tree::nth_func(std::size_t n) {
    return const_cast<Tree&>(
        const_cast<Tree*>(this)->nth_func(n));
}

const Tree& Tree::nth_func(std::size_t n) const {
    assert(!empty());

    if (n + 1 > func_num())
        throw std::out_of_range("Function number is out of range");

    if (n == 0) {
        return *this;
    } else {
        std::size_t skipped = 1; // skip oneself
        for (auto& c : children_) {
            if (skipped + c.func_num() > n)
                return c.nth_func(n - skipped);
            skipped += c.func_num();
        }
        // std::cout << "n = " << n << std::endl;
        // std::cout << "func num: " << func_num() << std::endl;
        // std::cout << as_pretty_string() << std::endl;
        assert(false);
    }
}

std::string Tree::as_string() const {
    if (empty()) return "[empty]";

    if (expr_->is_term()) {
        return expr_->get_name();

    } else if (expr_->is_func()) {
        std::string s = "(" + expr_->get_name();
        for (const auto& c : children_)
            s += " " + c.as_string();
        s += ")";
        return s;
    }
    assert(false);
}

std::string Tree::do_as_pretty_string(std::size_t offset) const {
    static const std::size_t arg_width_max = 10;

    if (empty()) return "[empty]";

    if (expr_->is_term()) {
        return expr_->get_name();

    } else if (expr_->is_func()) {
        std::string s = "(" + expr_->get_name();
        std::size_t newline_offset = s.size() + 1; // func name + 1 space

        // collect children strings
        bool add_newlines = false;
        std::vector<std::string> subs;
        for (const auto& c : children_) {
            std::string sub = c.do_as_pretty_string(offset + newline_offset);
            if (!add_newlines) {
                add_newlines =
                    (sub.find('\n') != std::string::npos)
                    || (sub.size() > arg_width_max + newline_offset);
            }
            subs.push_back(sub);
        }

        // compose pretty string
        bool first = true;
        for (const auto& sub : subs) {
            if (add_newlines) {
                if (first) {
                    s += " " + sub;
                    first = false;
                } else {
                    s += "\n"
                        + std::string(
                            newline_offset + offset, ' ')
                        + sub;
                }
            } else {
                s += " " + sub;
            }
        }
        return s + ")";
    }
    assert(false);
}

Tree full(
    const TermList& term_list,
    const FuncList& func_list,
    unsigned depth)
{
    if (depth == 0) {
        return Tree(random_element(term_list));

    } else {
        Tree t(random_element(func_list));
        for (unsigned i = 0; i < t.child_num(); ++i)
            t.set_child(i, full(term_list, func_list, depth - 1));
        return t;
    }
}

Tree grow(
    const TermList& term_list,
    const FuncList& func_list,
    unsigned depth)
{
    if (depth == 0) {
        return Tree(random_element(term_list));

    } else {
        std::random_device rd;
        std::uniform_int_distribution<unsigned> distr(
            0, term_list.size() + func_list.size());
        if (distr(rd) < term_list.size()) {
            return Tree(random_element(term_list));
        } else {
            Tree t(random_element(func_list));
            for (unsigned i = 0; i < t.child_num(); ++i)
                t.set_child(i, grow(term_list, func_list, depth - 1));
            return t;
        }
    }
}
