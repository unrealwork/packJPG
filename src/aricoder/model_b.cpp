#include "table.h"
#include "symbol.h"
#include "model_b.h"
#include "stdlib.h"

/* -----------------------------------------------
	binary statistical model for arithmetic coding
	----------------------------------------------- */
model_b::model_b( int max_c, int max_o, int c_lim )
{
    // boundaries of this model:
    // ... (maximum symbol) -> 2 (0 or 1 )
    // max_c (maximum context) -> 1 <= max_c <= 1024 (???)
    // max_o (maximum order) -> -1 <= max_o <= 4

    table* null_table;
    table* start_table;
    int i;


    // set error false
    error = false;

    // copy settings into model
    max_context = max_c;
    max_order   = max_o;
    max_count   = c_lim;


    // set up null table
    null_table = ( table* ) calloc( 1, sizeof( table ) );
    if ( null_table == NULL ) ERROR_EXIT;

    null_table->counts = ( unsigned short* ) calloc( 2, sizeof( short ) );
    if ( null_table->counts == NULL ) ERROR_EXIT;
    null_table->counts[ 0 ] = 1;
    null_table->counts[ 1 ] = 1;
    null_table->scale = 2;

    // set up start table
    start_table = ( table* ) calloc( 1, sizeof( table ) );
    if ( start_table == NULL ) ERROR_EXIT;
    start_table->links = ( table** ) calloc( max_context, sizeof( table* ) );
    if ( start_table->links == NULL ) ERROR_EXIT;
    start_table->scale = 0;

    // build links for start table & null table
    start_table->lesser = null_table;
    null_table->links = ( table** ) calloc( max_context, sizeof( table* ) );
    if ( null_table->links == NULL ) ERROR_EXIT;
    for ( i = 0; i < max_context; i++ )
        null_table->links[ i ] = start_table;

    // alloc memory for storage & contexts
    storage = ( table** ) calloc( max_order + 3, sizeof( table* ) );
    if ( storage == NULL ) ERROR_EXIT;
    contexts = storage + 1;

    // integrate tables into contexts
    contexts[ -1 ] = null_table;
    contexts[  0 ] = start_table;

    // build initial 'normal' tables
    for ( i = 1; i <= max_order; i++ ) {
        // set up current order table
        contexts[ i ] = ( table* ) calloc( 1, sizeof( table ) );
        if ( contexts[ i ] == NULL ) ERROR_EXIT;
        contexts[ i ]->scale = 0;
        // build forward and backward links
        contexts[ i ]->lesser = contexts[ i - 1 ];
        if ( i < max_order ) {
            contexts[ i ]->links = ( table** ) calloc( max_context, sizeof( table* ) );
            if ( contexts[ i ]->links == NULL ) ERROR_EXIT;
        }
        else {
            contexts[ i ]->links = NULL;
        }
        contexts[ i - 1 ]->links[ 0 ] = contexts[ i ];
    }
}


/* -----------------------------------------------
	model class destructor - recursive cleanup of memory is done here
	----------------------------------------------- */

model_b::~model_b( void )
{
    table* context;


    // clean up each 'normal' table
    context = contexts[ 0 ];
    recursive_cleanup ( context );

    // clean up null table
    context = contexts[ -1 ];
    if ( context->links  != NULL )
        free( context->links  );
    if ( context->counts != NULL ) free( context->counts );
    free ( context );

    // free everything else
    free( storage );
}


/* -----------------------------------------------
	updates statistics for a specific symbol / resets to highest order
	----------------------------------------------- */

void model_b::update_model( int symbol )
{
    // use -1 if you just want to reset without updating statistics

    table* context = contexts[ max_order ];

    // only contexts, that were actually used to encode
    // the symbol get their counts updated
    if ( ( symbol >= 0 ) && ( max_order >= 0 ) ) {
        // update count for specific symbol & scale
        context->counts[ symbol ]++;
        context->scale++;
        // if counts for that symbol have gone above the maximum count
        // the table has to be resized (scale factor 2)
        if ( context->counts[ symbol ] >= max_count )
            rescale_table( context, 1 );
    }
}


/* -----------------------------------------------
	shift in one context (max no of contexts is max_c)
	----------------------------------------------- */

void model_b::shift_context( int c )
{
    table* context;
    int i;

    // shifting is not possible if max_order is below 1
    // or context index is negative
    if ( ( max_order < 1 ) || ( c < 0 ) ) return;

    // shift each orders' context
    for ( i = max_order; i > 0; i-- ) {
        // this is the new current order context
        context = contexts[ i - 1 ]->links[ c ];

        // check if context exists, build if needed
        if ( context == NULL ) {
            // reserve memory for next table
            context = ( table* ) calloc( 1, sizeof( table ) );
            if ( context == NULL ) ERROR_EXIT;
            // set internal counts NULL
            context->counts = NULL;
            context->scale  = 0;
            // link lesser context later if not existing, this is done below
            context->lesser = contexts[ i - 2 ]->links[ c ];
            // finished here if this is a max order context
            if ( i == max_order ) {
                context->links = NULL;
            }
            else {
                // build links to higher order tables otherwise
                context->links = ( table** ) calloc( max_context, sizeof( table* ) );
                if ( context->links == NULL ) ERROR_EXIT;
                // add lesser link for higher context (see above)
                contexts[ i + 1 ]->lesser = context;
            }
            // put context to its right place
            contexts[ i - 1 ]->links[ c ] = context;
        }

        // switch context
        contexts[ i ] = context;
    }
}


/* -----------------------------------------------
	flushes the whole model by dividing through a specific scale factor
	----------------------------------------------- */

void model_b::flush_model( int scale_factor )
{
    recursive_flush( contexts[ 0 ], scale_factor );
}


/* -----------------------------------------------
	converts an int to a symbol, needed only when encoding
	----------------------------------------------- */

int model_b::convert_int_to_symbol( int c, symbol *s )
{
    table* context = contexts[ max_order ];

    // check if counts are available
    check_counts( context );

    // finding the scale is easy
    s->scale = context->scale;

    // return high and low count for current symbol
    if ( c == 0 ) { // if 0 is to be encoded
        s->low_count  = 0;
        s->high_count = context->counts[ 0 ];
    }
    else { // if 1 is to be encoded
        s->low_count  = context->counts[ 0 ];
        s->high_count = context->scale;
    }

    return 1;
}


/* -----------------------------------------------
	returns the current context scale needed only when decoding
	----------------------------------------------- */

void model_b::get_symbol_scale( symbol *s )
{
    table* context = contexts[ max_order ];

    // check if counts are available
    check_counts( context );

    // getting the scale is easy
    s->scale = context->scale;
}


/* -----------------------------------------------
	converts a count to an int, called after get_symbol_scale
	----------------------------------------------- */

int model_b::convert_symbol_to_int( int count, symbol *s )
{
    table* context = contexts[ max_order ];
    unsigned short counts0 = context->counts[ 0 ];

    // set up the current symbol
    if ( count < counts0 ) {
        s->low_count  = 0;
        s->high_count = counts0;
        return 0;
    }
    else {
        s->low_count  = counts0;
        s->high_count = s->scale;
        return 1;
    }
}


/* -----------------------------------------------
	this function checks if counts exist, and, if they exist and are below max
	----------------------------------------------- */

inline void model_b::check_counts( table *context )
{
    unsigned short* counts = context->counts;

    // check if counts are available
    if ( counts == NULL ) {
        // setup counts for current table
        counts = ( unsigned short* ) calloc( 2, sizeof( short ) );
        if ( counts == NULL ) ERROR_EXIT;
        counts[ 0 ] = 1;
        counts[ 1 ] = 1;
        // set scale
        context->counts = counts;
        context->scale = 2;
    }
}


/* -----------------------------------------------
	resizes one table by bitshifting each count using a specific value
	----------------------------------------------- */

inline void model_b::rescale_table( table* context, int scale_factor )
{
    unsigned short* counts = context->counts;

    // return now if counts not set
    if ( counts == NULL ) return;

    // now scale the table by bitshifting each count, be careful not to set any count zero
    counts[ 0 ] >>= scale_factor;
    counts[ 1 ] >>= scale_factor;
    if ( counts[ 0 ] == 0 ) counts[ 0 ] = 1;
    if ( counts[ 1 ] == 0 ) counts[ 1 ] = 1;
    context->scale = counts[ 0 ] + counts[ 1 ];
}


/* -----------------------------------------------
	a recursive function to go through each context and rescale the counts
	----------------------------------------------- */

inline void model_b::recursive_flush( table* context, int scale_factor )
{
    int i;

    // go through each link != NULL
    if ( context->links != NULL )
        for ( i = 0; i < max_context; i++ )
            if ( context->links[ i ] != NULL )
                recursive_flush( context->links[ i ], scale_factor );

    // rescale specific table
    rescale_table( context, scale_factor );
}


/* -----------------------------------------------
	frees all memory for all contexts starting at a given table
	----------------------------------------------- */

inline void model_b::recursive_cleanup( table *context )
{
    int i;

    // go through each link != NULL
    if ( context->links != NULL ) {
        for ( i = 0; i < max_context; i++ )
            if ( context->links[ i ] != NULL )
                recursive_cleanup( context->links[ i ] );
        free ( context->links );
    }

    // clean up table
    if ( context->counts != NULL ) free ( context->counts );
    free( context );
}
