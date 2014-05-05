#include "func.hpp"

double plus2(const Params& args) {
    return args[0] + args[1];
}

double minus2(const Params& args) {
    return args[0] - args[1];
}

double mult2(const Params& args) {
    return args[0] * args[1];
}

double mult3(const Params& args) {
    return args[0] * args[1] * args[2];
}

double safe_div2(const Params& args) {
    return (args[1] == 0.0) ? 0.0 : args[0] / args[1];
}

double sin1(const Params& args) {
    return std::sin(args[0]);
}

double cos1(const Params& args) {
    return std::cos(args[0]);
}

double rlog1(const Params& args) {
    return (args[0] == 0.0)
        ? 0.0 :
        std::log(std::fabs(args[0]));
}

double exp1(const Params& args) {
    return std::exp(args[0]);
}
