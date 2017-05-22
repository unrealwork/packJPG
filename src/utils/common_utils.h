//
// Created by shmagrinskiy on 5/22/17.
//

#ifndef PACKJPG_COMMON_UTILS_H
#define PACKJPG_COMMON_UTILS_H
class common_utils {
private:
    common_utils();
    ~common_utils();

public:
    template <class T>
    static void insertion_sort(T* values, int size )
    {
        bool done;
        T swap;
        int i;


        // sort data first
        done = false;
        while ( !done ) {
            done = true;
            for ( i = 1; i < size; i++ )
                if ( values[ i ] < values[ i - 1 ] ) {
                    swap = values[ i ];
                    values[ i ] = values[ i - 1 ];
                    values[ i - 1 ] = swap;
                    done = false;
                }
        }

    }
};
#endif //PACKJPG_COMMON_UTILS_H
