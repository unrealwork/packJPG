#ifndef PACKJPG_TABLE_S_H_H
#define PACKJPG_TABLE_S_H_H
// special table struct, used in in model_s,
// holding additional info for a speedier 'totalize_table'
struct table_s {
    // counts for each symbol contained in the table
    unsigned short *counts;
    // links to higher order contexts
    struct table_s **links;
    // link to lower order context
    struct table_s *lesser;
    // speedup info
    unsigned short max_count;
    unsigned short max_symbol;
    // unsigned short esc_prob;
};
#endif //PACKJPG_TABLE_S_H_H
