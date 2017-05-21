//
// Created by shmagrinskiy on 5/21/17.
//

#ifndef PACKJPG_CODER_LIMITS_H
#define PACKJPG_CODER_LIMITS_H

// defines for coder
#define CODER_USE_BITS        31 // must never be above 31
#define CODER_LIMIT100        ( (unsigned int) ( 1 << CODER_USE_BITS ) )
#define CODER_LIMIT025        ( ( CODER_LIMIT100 / 4 ) * 1 )
#define CODER_LIMIT050        ( ( CODER_LIMIT100 / 4 ) * 2 )
#define CODER_LIMIT075        ( ( CODER_LIMIT100 / 4 ) * 3 )
#define CODER_MAXSCALE        CODER_LIMIT025 - 1
#define ESCAPE_SYMBOL        CODER_LIMIT025

#endif //PACKJPG_CODER_LIMITS_H
