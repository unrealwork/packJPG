//
// Created by shmagrinskiy on 5/21/17.
//

#ifndef PACKJPG_SYMBOL_H
#define PACKJPG_SYMBOL_H
// symbol struct, used in arithmetic coding
struct symbol {
    unsigned int low_count;
    unsigned int high_count;
    unsigned int scale;
};
#endif //PACKJPG_SYMBOL_H
