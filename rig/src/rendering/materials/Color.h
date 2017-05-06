#ifndef Color_h
#define Color_h

#include "../base/cvec.h"

class Color {

public:
    static const Cvec3f RED;
    static const Cvec3f YELLOW;
    static const Cvec3f DARK_YELLOW;
    static const Cvec3f BLACK;
    static const Cvec3f WHITE;
};

const Cvec3f Color::RED = Cvec3f(1.0, 0.0, 0.0);
const Cvec3f Color::YELLOW = Cvec3f(1.0, 1.0, 0.0);
const Cvec3f Color::DARK_YELLOW = Cvec3f(0.2, 0.2, 0.0);
const Cvec3f Color::BLACK = Cvec3f(0.0, 0.0, 0.0);
const Cvec3f Color::WHITE = Cvec3f(1.0, 1.0, 1.0);

#endif /* Color_h */
