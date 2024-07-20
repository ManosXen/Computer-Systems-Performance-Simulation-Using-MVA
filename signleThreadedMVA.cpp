#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

#define numDelayStations 3

int N, M;
int *clientPerClass;
float *D, ***Q;
int maxConc;
string* station_names;
int delayStations[numDelayStations] = {0, 4, 5};
// float ***p;  // This variable is not used, so it's commented out.

void calcTotalClients(int &counter, int* cl) {
    counter = 0;
    for (int i = 0; i < N; i++) counter += cl[i];
}

void MVA(int cl0, int cl1) {
    float *R = new float[N * M];
    float sum0 = 0, sum1 = 0;

    for (int station = 0; station < M; station++) {
        bool delayStation = false;
        for (int i = 0; i < numDelayStations; i++) {
            if (station == delayStations[i]) {
                delayStation = true;
                break;
            }
        }

        if (delayStation) {
            R[station * N] = D[station * N];
            R[station * N + 1] = D[station * N + 1];
        } else if (station == 12) {
            R[station * N] = 0; // Placeholder, replace with actual logic.
            R[station * N + 1] = 0; // Placeholder, replace with actual logic.
        } else {
            if (cl0 > 0) R[station * N] = D[station * N] * (1 + Q[cl0 - 1][cl1][station]);
            else R[station * N] = 0;

            if (cl1 > 0) R[station * N + 1] = D[station * N + 1] * (1 + Q[cl0][cl1 - 1][station]);
            else R[station * N + 1] = 0;
        }
        sum0 += R[station * N];
        sum1 += R[station * N + 1];
    }

    float X0 = 0, X1 = 0;
    if (sum0 != 0) X0 = clientPerClass[0] / (float)sum0;
    if (sum1 != 0) X1 = clientPerClass[1] / (float)sum1;

    for (int station = 0; station < M; station++) {
        Q[cl0][cl1][station] = X0 * R[station * N] + X1 * R[station * N + 1];
    }

    if (cl0 == clientPerClass[0] && cl1 == clientPerClass[1]) {
        cout << "X:" << endl;
        cout << "Class 1: " << X0 << endl;
        cout << "Class 2: " << X1 << endl;
    }

    delete[] R;
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

    D = new float[N * M]{
        25, 29, 32, 0, 0, 90, 48, 134, 25, 0, 0, 16, 59, 0, 70,
        0, 0, 28, 0, 25, 48, 58, 54, 66, 69, 106, 67, 72, 88, 96
    };
    
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
    for (int clients = 1; clients <= total_clients; clients++) {
        clients1 = 0;
        if (clients <= clientPerClass[0]) clients0 = clients;
        else {
            clients0 = clientPerClass[0];
            clients1 = clients - clients0;
        }

        while (true) {
            MVA(clients0, clients1);

            if (clients0 > 0) clients0 -= 1;
            clients1++;
            if (clients1 > clientPerClass[1] || clients1 > clients) {
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
    return 0;
}

