#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <pthread.h>

struct ThreadData {
    int* result;
    long double num_samples;
};

void* calculate_orthant(void *thread_args) {

    ThreadData* data = (ThreadData*)thread_args;
    int* result = data->result;
    long double num_samples = data->num_samples;

    int inside;
    // threadsafe random number generator
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<long double> distribution(0, 1); // [0, 1) which is a small issue

    for (int i = 0; i < num_samples; i++) {
        long double x_comp = distribution(generator);
        long double y_comp = distribution(generator);
        
        // take norm of vector
        long double norm = sqrt(x_comp * x_comp + y_comp * y_comp);
        if (norm <= 1) {
            inside++;
        }
    }
    *result = inside;

    pthread_exit(NULL);
}

int main() {
    
    // assign amount of threads and samples, create vectors for points and threads
    int num_threads = 4;
    long double num_samples = 10000000.;
    std::vector<int> flummoxes(num_threads);
    pthread_t threads[num_threads];
    std::vector<ThreadData> thread_data(num_threads);

    int total_inside = 0;

    // Create threads and pass indices of vectors by reference
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].result = &flummoxes[i];
        thread_data[i].num_samples = num_samples;
        pthread_create(&threads[i], NULL, calculate_orthant, (void*)&thread_data[i]);
    }

    // join threads
    for (int j = 0; j < num_threads; j++) {
        pthread_join(threads[j], NULL); 
    }


    // calculate end result
    for (int k = 0; k < num_threads; k++) {
        total_inside += flummoxes[k];
    }
    
    long double pi = (static_cast<long double>(total_inside) / (num_samples * ((long) pow(2, (log2(num_threads) - 2)))));

    std::cout << "After " << num_samples << " iterations on each of " << num_threads << " threads, the value of pi = " << pi << std::endl;


    return 0;
}