# Multi-Thread K-Means

This project, "Multi-Thread K-Means," was developed as a course project for Object-Oriented Programming (C++) in the Winter of 2022.

## Description

The Multi-Thread K-Means project aims to implement a parallelized version of the K-Means clustering algorithm using multi-threading techniques in C++. The K-Means algorithm is a popular unsupervised machine learning algorithm used for clustering analysis. By leveraging multi-threading, the project seeks to enhance the efficiency and performance of the algorithm by leveraging the capabilities of modern multi-core processors.

## Features

- Parallelized implementation of the K-Means clustering algorithm using multi-threading techniques
- Efficient data partitioning and workload distribution across multiple threads
- Support for customizable number of clusters and convergence criteria
- Handling of large datasets through optimized memory management
- Evaluation and visualization of clustering results

## Getting Started

### Prerequisites

To compile and run the Multi-Thread K-Means project, the following prerequisites should be met:

- C++ compiler that supports C++11 or higher
- Operating system that supports multi-threading

### Compilation

Follow these steps to compile the project:

1. Clone the project repository: `git clone https://github.com/your-username/multi-thread-kmeans.git`
2. Navigate to the project directory: `cd multi-thread-kmeans`
3. Compile the project using your C++ compiler:

```
g++ -std=c++11 -pthread -o multi_thread_kmeans main.cpp kmeans.cpp utils.cpp
```

### Usage

After successfully compiling the project, you can run the executable to perform multi-threaded K-Means clustering on your dataset. Use the following command:

```
./multi_thread_kmeans <input_file> <num_clusters> <num_threads> <max_iterations>
```

- `<input_file>`: Path to the input dataset file.
- `<num_clusters>`: Number of clusters to generate.
- `<num_threads>`: Number of threads to use for parallelization.
- `<max_iterations>`: Maximum number of iterations for the K-Means algorithm.

## Contributing

Contributions to the Multi-Thread K-Means project are welcome. If you encounter any issues, have suggestions for improvements, or would like to contribute new features, please submit a pull request.





