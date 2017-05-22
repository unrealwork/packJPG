#include "symbol.h"
#include "table_s.h"

#define ERROR_EXIT { error = true; exit( 0 ); }


#ifndef PACKJPG_MODEL_S_H
#define PACKJPG_MODEL_S_H
/* -----------------------------------------------
	universal statistical model for arithmetic coding
	----------------------------------------------- */

class model_s {
public:

    model_s(int max_s, int max_c, int max_o, int c_lim);

    ~model_s(void);

    void update_model(int symbol);

    void shift_context(int c);

    void flush_model(int scale_factor);

    void exclude_symbols(char rule, int c);

    int convert_int_to_symbol(int c, symbol *s);

    void get_symbol_scale(symbol *s);

    int convert_symbol_to_int(int count, symbol *s);

    bool error;


private:

    // unsigned short* totals;
    unsigned int *totals;
    char *scoreboard;
    int sb0_count;
    table_s **contexts;
    table_s **storage;

    int max_symbol;
    int max_context;
    int current_order;
    int max_order;
    int max_count;

    inline void totalize_table(table_s *context);

    inline void rescale_table(table_s *context, int scale_factor);

    inline void recursive_flush(table_s *context, int scale_factor);

    inline void recursive_cleanup(table_s *context);
};


#endif //PACKJPG_MODEL_S_H
