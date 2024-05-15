#include <mpi.h>

#include <vector>

template <typename T>
struct SortEngine {
    std::vector<T> mydata, odata, tmp;
    int world_rank, world_size, count;

    SortEngine(std::vector<T> mydata_, int world_rank, int world_size)
        : world_rank(world_rank),
          world_size(world_size),
          odata(mydata_.size()),
          tmp(mydata_.size()) {
        mydata = std::move(mydata_);
        count = mydata.size() * sizeof(T);
    }
    void send(int orank) {
        MPI_Status status;
        MPI_Send(mydata.data(), count, MPI_BYTE, orank, 0, MPI_COMM_WORLD);
    }
    void recv(int orank) {
        MPI_Status status;
        MPI_Recv(odata.data(), count, MPI_BYTE, orank, 0, MPI_COMM_WORLD,
                 &status);
    }
    void exchange_data(int orank) {
        if (orank > world_rank) send(orank);
        recv(orank);
        if (orank < world_rank) send(orank);
    }
    void smallest_half() {
        // Stores smallest half of ascending sorted vectors a, b in x
        int n = mydata.size();  // == a.size() == b.size()
        int ai = 0, bi = 0;
        for (int i = 0; i < n; i++)
            tmp[i] = (mydata[ai] < odata[bi]) ? mydata[ai++] : odata[bi++];
        swap(mydata, tmp);
    }

    void largest_half() {
        // Stores largest half of ascending sorted vectors a, b in x
        int n = mydata.size();  // == a.size() == b.size()
        std::cout << mydata.size() << " " << odata.size() << " " << tmp.size()
                  << std::endl;
        int ai = n - 1, bi = n - 1;
        for (int i = 0; i < n; i++)
            tmp[n - 1 - i] =
                (mydata[ai] > odata[bi]) ? mydata[ai--] : odata[bi--];
        swap(mydata, tmp);
    }
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
    void run_sort() {
        std::sort(mydata.begin(), mydata.end());
        for (int step = 0; step < world_size; step++) {
            step_sort(step);
        }
    }
};