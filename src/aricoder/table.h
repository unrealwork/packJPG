#ifndef PACKJPG_TABLE_H
#define PACKJPG_TABLE_H
// table struct, used in in statistical models,
// holding all info needed for one context
struct table {
    // counts for each symbol contained in the table
    unsigned short *counts;
    // links to higher order contexts
    struct table **links;
    // link to lower order context
    struct table *lesser;
    // accumulated counts
    unsigned int scale;
};
#endif //PACKJPG_TABLE_H
