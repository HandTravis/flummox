#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <pthread.h>
#include <chrono>

struct ThreadData {
    int* result;
    long double num_samples;
};

void* calculate_orthant(void *thread_args) {
    ThreadData* data = (ThreadData*)thread_args;
    int* result = data->result;
    long double num_samples = data->num_samples;

    int inside = 0;
    // Thread-safe random number generator
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_real_distribution<long double> distribution(0.0L, 1.0L);

    for (long long i = 0; i < num_samples; i++) {
        long double x_comp = distribution(generator);
        long double y_comp = distribution(generator);

        // Calculate norm of the vector
        long double norm = sqrt(x_comp * x_comp + y_comp * y_comp);
        if (norm <= 1.0L) {
            inside++;
        }
    }
    *result = inside;

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Assign amount of threads and samples, create vectors for points and threads
    if (argc != 3) {
        std::cout << "usage: " << argv[0] << " <num_threads> <num_samples>" << std::endl; 
        return 1;
    }

    int num_threads = std::stoi(argv[1]);
    long double num_samples = std::stoi(argv[2]);  // Increase number of samples for higher precision

    std::vector<int> flummoxes(num_threads);
    pthread_t threads[num_threads];
    std::vector<ThreadData> thread_data(num_threads);

    // create tools to time calculation of pi
    long double elapsed_time = 0.0L;
    std::vector<long double> t_performance;

    int total_inside = 0;

    // Create threads and pass indices of vectors by reference
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].result = &flummoxes[i];
        thread_data[i].num_samples = num_samples / num_threads; // Divide samples among threads

        auto start = std::chrono::high_resolution_clock::now();
        pthread_create(&threads[i], NULL, calculate_orthant, (void*)&thread_data[i]);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() * 1.e-9;
        elapsed_time += duration;
    }

    // Join threads
    for (int j = 0; j < num_threads; j++) {
        pthread_join(threads[j], NULL);
    }

    // Calculate end result
    for (int k = 0; k < num_threads; k++) {
        total_inside += flummoxes[k];
    }

    long double average_time = elapsed_time / (long) num_threads;
    
    long double pi = 4 * (static_cast<long double>(total_inside) / (num_samples));

    std::cout << "After " << num_samples << " iterations with \t" << num_threads << "\t threads across \t" << average_time << "\t seconds, the value of pi = " << pi << std::endl;

    return 0;
}
