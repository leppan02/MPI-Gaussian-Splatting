#include <mpi.h>

#include <vector>

/**
 * @brief A template struct representing a sorting engine for parallel sorting using MPI.
 * 
 * @tparam T The type of elements to be sorted.
 */
template <typename T>
struct SortEngine {
    std::vector<T> mydata, odata, tmp; // Input, output, and temporary vectors
    int world_rank, world_size, count; // MPI rank, size, and count

    /**
     * @brief Constructs a SortEngine object.
     * 
     * @param mydata_ The input data to be sorted.
     * @param world_rank_ The rank of the current MPI process.
     * @param world_size_ The total number of MPI processes.
     */
    SortEngine(std::vector<T> mydata_, int world_rank_, int world_size_)
        : odata(mydata_.size()),
          tmp(mydata_.size()),
          world_rank(world_rank_),
          world_size(world_size_) {
        mydata = std::move(mydata_);
        count = mydata.size() * sizeof(T);
    }

    /**
     * @brief Sends data to another MPI process.
     * 
     * @param orank The rank of the destination MPI process.
     */
    void send(int orank) {
        MPI_Send(mydata.data(), count, MPI_BYTE, orank, 0, MPI_COMM_WORLD);
    }

    /**
     * @brief Receives data from another MPI process.
     * 
     * @param orank The rank of the source MPI process.
     */
    void recv(int orank) {
        MPI_Status status;
        MPI_Recv(odata.data(), count, MPI_BYTE, orank, 0, MPI_COMM_WORLD, &status);
    }

    /**
     * @brief Exchanges data with another MPI process.
     * 
     * @param orank The rank of the other MPI process.
     */
    void exchange_data(int orank) {
        if (orank > world_rank) send(orank);
        recv(orank);
        if (orank < world_rank) send(orank);
    }

    /**
     * @brief Replaces with the smallest half of the vectors in ascending order.
     */
    void smallest_half() {
        int n = mydata.size(); // == a.size() == b.size()
        int ai = 0, bi = 0;
        for (int i = 0; i < n; i++)
            tmp[i] = (mydata[ai] < odata[bi]) ? mydata[ai++] : odata[bi++];
        swap(mydata, tmp);
    }

    /**
     * @brief Replaces with the largest half of the vectors in ascending order.
     */
    void largest_half() {
        int n = mydata.size(); // == a.size() == b.size()
        int ai = n - 1, bi = n - 1;
        for (int i = 0; i < n; i++)
            tmp[n - 1 - i] = (mydata[ai] > odata[bi]) ? mydata[ai--] : odata[bi--];
        swap(mydata, tmp);
    }

    /**
     * @brief Performs a step of the sorting algorithm.
     * 
     * @param step The current step of the sorting algorithm.
     */
    void step_sort(int step) {
        bool odd = step & 1;
        int orank = world_rank + 1 - ((world_rank ^ odd) & 1) * 2;
        if (orank < 0 || orank >= world_size) return;
        exchange_data(orank);
        if ((orank > world_rank) ^ odd)
            smallest_half();
        else
            largest_half();
    }

    /**
     * @brief Runs the parallel sorting algorithm.
     */
    void run_sort() {
        std::sort(mydata.begin(), mydata.end());
        for (int step = 0; step < world_size; step++) {
            step_sort(step);
        }
    }
};