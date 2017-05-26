#include "symbol.h"
#include "coder_limits.h"
#include "table.h"
#include "table_s.h"
#include "model_s.h"
#include "model_b.h"

/* -----------------------------------------------
	class for arithmetic coding of data to/from iostream
	----------------------------------------------- */

class aricoder {
public:
    aricoder(iostream *stream, int iomode);

    ~aricoder(void);

    void encode(symbol *s);

    unsigned int decode_count(symbol *s);

    void decode(symbol *s);

private:
    // bitwise operations
    void write_bit(unsigned char bit);

    unsigned char read_bit(void);

    // i/o variables
    iostream *sptr;
    int mode;
    unsigned char bbyte;
    unsigned char cbit;

    // arithmetic coding variables
    unsigned int ccode;
    unsigned int clow;
    unsigned int chigh;
    unsigned int cstep;
    unsigned int nrbits;
};


/* -----------------------------------------------
	shift context x2 model_s function
	----------------------------------------------- */
static inline void shift_model(model_s *model, int ctx1, int ctx2) {
    model->shift_context(ctx1);
    model->shift_context(ctx2);
}


/* -----------------------------------------------
	shift context x3 model_s function
	----------------------------------------------- */
static inline void shift_model(model_s *model, int ctx1, int ctx2, int ctx3) {
    model->shift_context(ctx1);
    model->shift_context(ctx2);
    model->shift_context(ctx3);
}


/* -----------------------------------------------
	shift context x2 model_b function
	----------------------------------------------- */
static inline void shift_model(model_b *model, int ctx1, int ctx2) {
    model->shift_context(ctx1);
    model->shift_context(ctx2);
}


/* -----------------------------------------------
	shift context x3 model_b function
	----------------------------------------------- */
static inline void shift_model(model_b *model, int ctx1, int ctx2, int ctx3) {
    model->shift_context(ctx1);
    model->shift_context(ctx2);
    model->shift_context(ctx3);
}


/* -----------------------------------------------
	generic model_s encoder function
	----------------------------------------------- */
static inline void encode_ari(aricoder *encoder, model_s *model, int c) {
    static symbol s;
    static int esc;

    do {
        esc = model->convert_int_to_symbol(c, &s);
        encoder->encode(&s);
    } while (esc);
    model->update_model(c);
}

/* -----------------------------------------------
	generic model_s decoder function
	----------------------------------------------- */
static inline int decode_ari(aricoder *decoder, model_s *model) {
    static symbol s;
    static unsigned int count;
    static int c;

    do {
        model->get_symbol_scale(&s);
        count = decoder->decode_count(&s);
        c = model->convert_symbol_to_int(count, &s);
        decoder->decode(&s);
    } while (c == ESCAPE_SYMBOL);
    model->update_model(c);

    return c;
}

/* -----------------------------------------------
	generic model_b encoder function
	----------------------------------------------- */
static inline void encode_ari(aricoder *encoder, model_b *model, int c) {
    static symbol s;

    model->convert_int_to_symbol(c, &s);
    encoder->encode(&s);
    model->update_model(c);
}

/* -----------------------------------------------
	generic model_b decoder function
	----------------------------------------------- */
static inline int decode_ari(aricoder *decoder, model_b *model) {
    static symbol s;
    static unsigned int count;
    static int c;

    model->get_symbol_scale(&s);
    count = decoder->decode_count(&s);
    c = model->convert_symbol_to_int(count, &s);
    decoder->decode(&s);
    model->update_model(c);

    return c;
}
