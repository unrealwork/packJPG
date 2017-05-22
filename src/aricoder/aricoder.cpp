#include <stdlib.h>
#include "src/utils/bitops.h"
#include "aricoder.h"

#define ERROR_EXIT { error = true; exit( 0 ); }


/* -----------------------------------------------
	constructor for aricoder class
	----------------------------------------------- */

aricoder::aricoder( iostream* stream, int iomode )
{	
	// iomode (i/o mode)
	// 0 -> reading
	// 1 -> writing
	
	int i;
	
	// set initial values
	ccode	= 0;
	clow	= 0;
	chigh	= CODER_LIMIT100 - 1;
	cstep	= 0;
	bbyte	= 0;
	cbit	= 0;
	nrbits	= 0;
	
	// store pointer to iostream for reading/writing
	sptr = stream;
	
	// store i/o mode
	mode = iomode;
	
	if ( mode == 0 ) { // mode is reading / decoding
		// code buffer has to be filled before starting decoding
		for ( i = 0; i < CODER_USE_BITS; i++ )
			ccode = ( ccode << 1 ) | read_bit();
	} // mode is writing / encoding otherwise
}

/* -----------------------------------------------
	destructor for aricoder class
	----------------------------------------------- */

aricoder::~aricoder( void )
{
	if ( mode == 1 ) { // mode is writing / encoding
		// due to clow < CODER_LIMIT050, and chigh >= CODER_LIMIT050
		// there are only two possible cases
		if ( clow < CODER_LIMIT025 ) { // case a.) 
			write_bit( 0 );
			// write remaining bits
			write_bit( 1 );
			while ( nrbits-- > 0 )
				write_bit( 1 );
		}
		else { // case b.), clow >= CODER_LIMIT025
			write_bit( 1 );
		} // done, zeroes are auto-read by the decoder
		
		// pad code with zeroes
		while ( cbit > 0 ) write_bit( 0 );
	}
}

/* -----------------------------------------------
	arithmetic encoder function
	----------------------------------------------- */
	
void aricoder::encode( symbol* s )
{	
	// update steps, low count, high count
	cstep = ( ( chigh - clow ) + 1 ) / s->scale;
	chigh = clow + ( cstep * s->high_count ) - 1;
	clow  = clow + ( cstep * s->low_count );
	
	// e3 scaling is performed for speed and to avoid underflows
	// if both, low and high are either in the lower half or in the higher half
	// one bit can be safely shifted out
	while ( ( clow >= CODER_LIMIT050 ) || ( chigh < CODER_LIMIT050 ) ) {		
		if ( chigh < CODER_LIMIT050 ) {	// this means both, high and low are below, and 0 can be safely shifted out
			// write 0 bit
			write_bit( 0 );
			// shift out remaing e3 bits
			for ( ; nrbits > 0; nrbits-- )
				write_bit( 1 );
		}
		else { // if the first wasn't the case, it's clow >= CODER_LIMIT050
			// write 1 bit
			write_bit( 1 );
			clow  &= CODER_LIMIT050 - 1;
			chigh &= CODER_LIMIT050 - 1;
			// shift out remaing e3 bits
			for ( ; nrbits > 0; nrbits-- )
				write_bit( 0 );
		}
		clow  <<= 1;
		chigh <<= 1;
		chigh++;
	}
	
	// e3 scaling, to make sure that theres enough space between low and high
	while ( ( clow >= CODER_LIMIT025 ) && ( chigh < CODER_LIMIT075 ) ) {
		nrbits++;
		clow  &= CODER_LIMIT025 - 1;
		chigh ^= CODER_LIMIT025 + CODER_LIMIT050;
		// clow  -= CODER_LIMIT025;
		// chigh -= CODER_LIMIT025;
		clow  <<= 1;
		chigh <<= 1;
		chigh++;
	}
}

/* -----------------------------------------------
	arithmetic decoder get count function
	----------------------------------------------- */
	
unsigned int aricoder::decode_count( symbol* s )
{
	// update cstep, which is needed to remove the symbol from the stream later
	cstep = ( ( chigh - clow ) + 1 ) / s->scale;
	
	// return counts, needed to decode the symbol from the statistical model
	return ( ccode - clow ) / cstep;
}

/* -----------------------------------------------
	arithmetic decoder function
	----------------------------------------------- */
	
void aricoder::decode( symbol* s )
{
	// no actual decoding takes place, as this has to happen in the statistical model
	// the symbol has to be removed from the stream, though
	
	// alread have steps updated from decoder_count
	// update low count and high count
	chigh = clow + ( cstep * s->high_count ) - 1;
	clow  = clow + ( cstep * s->low_count );
	
	// e3 scaling is performed for speed and to avoid underflows
	// if both, low and high are either in the lower half or in the higher half
	// one bit can be safely shifted out
	while ( ( clow >= CODER_LIMIT050 ) || ( chigh < CODER_LIMIT050 ) ) {
		if ( clow >= CODER_LIMIT050 ) {
			clow  &= CODER_LIMIT050 - 1;
			chigh &= CODER_LIMIT050 - 1;
			ccode &= CODER_LIMIT050 - 1;
		} // if the first wasn't the case, it's chigh < CODER_LIMIT050
		clow  <<= 1;
		chigh <<= 1;
		chigh++;
		ccode <<= 1;
		ccode |= read_bit();
		nrbits = 0;
	}
	
	// e3 scaling, to make sure that theres enough space between low and high
	while ( ( clow >= CODER_LIMIT025 ) && ( chigh < CODER_LIMIT075 ) ) {
		nrbits++;
		clow  &= CODER_LIMIT025 - 1;
		chigh ^= CODER_LIMIT025 + CODER_LIMIT050;
		// clow  -= CODER_LIMIT025;
		// chigh -= CODER_LIMIT025;
		ccode -= CODER_LIMIT025;
		clow  <<= 1;
		chigh <<= 1;
		chigh++;
		ccode <<= 1;
		ccode |= read_bit();
	}	
}

/* -----------------------------------------------
	bit writer function
	----------------------------------------------- */
	
void aricoder::write_bit( unsigned char bit )
{
	// add bit at last position
	bbyte = ( bbyte << 1 ) | bit;
	// increment bit position
	cbit++;
	
	// write bit if done
	if ( cbit == 8 ) {
		sptr->write( (void*) &bbyte, 1, 1 );
		cbit = 0;
	}
}

/* -----------------------------------------------
	bit reader function
	----------------------------------------------- */
	
unsigned char aricoder::read_bit( void )
{
	// read in new byte if needed
	if ( cbit == 0 ) {
		if ( sptr->read( &bbyte, 1, 1 ) == 0 ) // read next byte if available
			bbyte = 0; // if no more data is left in the stream
		cbit = 8;
	}
	
	// decrement current bit position
	cbit--;	
	// return bit at cbit position
	return BITN( bbyte, cbit );
}


/* -----------------------------------------------
	universal statistical model for arithmetic coding
	----------------------------------------------- */
