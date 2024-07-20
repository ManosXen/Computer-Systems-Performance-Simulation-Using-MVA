#include <iostream>
#include <thread>
#include <cmath>
#include <utility>
#include <vector>
#include <mutex>
using namespace std;

int N, M;
int *p, *clientPerClass;
float *D, ***Q;
int maxConc;
string* station_names;
mutex mtx;

void calcTotalClients(int &counter, int* cl){
    counter = 0;
    for (int i = 0; i < N; i++) counter += cl[i];
}

void MVA(int cl0, int cl1){
    float R[N * M];
    float sum0 = 0, sum1 = 0;
    for (int station = 0; station < M; station++) {
        if (cl0 > 0) R[station * N] = D[station * N] * (1 + Q[cl0 - 1][cl1][station]);
        else R[station * N] = 0;
        sum0 += R[station * N];

        if (cl1 > 0) R[station * N + 1] = D[station * N + 1] * (1 + Q[cl0][cl1 - 1][station]);
        else R[station * N + 1] = 0;
        sum1 += R[station * N + 1];
    }

    float X0 = 0, X1 = 0;
    if (sum0 != 0) X0 = clientPerClass[0] / (float)sum0;
    if (sum1 != 0) X1 = clientPerClass[1] / (float)sum1;

    //mtx.lock();
    for (int station = 0; station < M; station++) {
        Q[cl0][cl1][station] = X0 * R[station * N] + X1 * R[station * N + 1];
    }
    //mtx.unlock();

    if (cl0 == clientPerClass[0] && cl1 == clientPerClass[1]) {
        cout << "X:" << endl;
        cout << "Class 1: " << X0 << endl;
        cout << "Class 2: " << X1 << endl;
    }
}

int main() {
    N = 2;
    M = 15;

    station_names = new string[M];
    station_names[0] = "CPU";
    station_names[1] = "Disk";

    clientPerClass = new int[N];
    clientPerClass[0] = 304;
    clientPerClass[1] = 240;

    D = new float[N * M];
    D[0] = 25;
    D[1] = 29;
    D[2] = 32;
    D[3] = 0;
    D[4] = 0;
    D[5] = 90;
    D[6] = 48;
    D[7] = 134;
    D[8] = 25;
    D[9] = 0;
    D[10] = 0;
    D[11] = 16;
    D[12] = 59;
    D[13] = 0;
    D[14] = 70;
    D[15] = 0;
    D[16] = 0;
    D[17] = 28;
    D[18] = 0;
    D[19] = 25;
    D[20] = 48;
    D[21] = 58;
    D[22] = 54;
    D[23] = 66;
    D[24] = 69;
    D[25] = 106;
    D[26] = 67;
    D[27] = 72;
    D[28] = 88;
    D[29] = 96;
    maxConc = min(clientPerClass[1], clientPerClass[0]) + 1;

    Q = new float**[clientPerClass[0] + 1];
    for (int i = 0; i <= clientPerClass[0]; i++) {
        Q[i] = new float*[clientPerClass[1] + 1];
        for (int j = 0; j <= clientPerClass[1]; j++) {
            Q[i][j] = new float[M];
            for (int k = 0; k < M; k++) {
                Q[i][j][k] = 0;
            }
        }
    }

    int total_clients;
    calcTotalClients(total_clients, clientPerClass);

    int clients0, clients1;
    thread *threads = new thread[total_clients];
    for (int clients = 1; clients <= total_clients; clients++) {
        clients1 = 0;
        if (clients <= clientPerClass[0]) clients0 = clients;
        else {
            clients0 = clientPerClass[0];
            clients1 = clients - clients0;
        }
        //vector<thread> threads;
        int i=0;
        while (true) {
            //threads[i]= thread(MVA, clients0, clients1);
            MVA(clients0, clients1);
            if (clients0 > 0) clients0 -= 1;
            clients1++;
            i++;
            if (clients1 > clientPerClass[1] || clients1 > clients) {
                /*for(int j=0; j<i; j++){
                    threads[j].join();
                }*/ 
                break;
            }
        }
    }

    for (int i = 0; i <= clientPerClass[0]; i++) {
        for (int j = 0; j <= clientPerClass[1]; j++) {
            delete[] Q[i][j];
        }
        delete[] Q[i];
    }
    delete[] Q;
    delete[] D;
    delete[] clientPerClass;
    delete[] station_names;
}
