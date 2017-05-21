#define ERROR_EXIT { error = true; exit( 0 ); }

#ifndef PACKJPG_MODEL_B_H
#define PACKJPG_MODEL_B_H
/* -----------------------------------------------
	special version of model_s for binary coding
	----------------------------------------------- */
class model_b {
public:

    model_b(int max_c, int max_o, int c_lim);

    ~model_b(void);

    void update_model(int symbol);

    void shift_context(int c);

    void flush_model(int scale_factor);

    int convert_int_to_symbol(int c, symbol *s);

    void get_symbol_scale(symbol *s);

    int convert_symbol_to_int(int count, symbol *s);

    bool error;
private:
    table **contexts;
    table **storage;

    int max_context;
    int max_order;
    int max_count;

    inline void check_counts(table *context);

    inline void rescale_table(table *context, int scale_factor);

    inline void recursive_flush(table *context, int scale_factor);

    inline void recursive_cleanup(table *context);
};

#endif //PACKJPG_MODEL_B_H
