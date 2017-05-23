#ifndef PACKJPG_COMMON_UTILS_H
#define PACKJPG_COMMON_UTILS_H

#include <algorithm>

class common_utils {
private:
    common_utils();

    ~common_utils();

public:
    template<class T>
    static void sort(T *a, int n) {
        long i = 0, j = n;
        float pivot = a[n / 2]; // опорный элемент
        do {
            while (a[i] < pivot) i++;
            while (a[j] > pivot) j--;
            if (i <= j) {
                std::swap(a[i], a[j]);
                i++;
                j--;
            }
        } while (i <= j);
        if (n < 100) { // если размер массива меньше 100
            // сортировка выполняется в текущем потоке
            if (j > 0) sort(a, j);
            if (n > i) sort(a + i, n - i);
            return;
        }
#pragma omp task shared(a)
        if (j > 0) sort(a, j);
#pragma omp task shared(a)
        if (n > i) sort(a + i, n - i);
#pragma omp taskwait
    }

};

#endif //PACKJPG_COMMON_UTILS_H
