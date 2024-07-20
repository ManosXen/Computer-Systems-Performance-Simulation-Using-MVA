#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

#define N 2
#define M 15
#define numDelayStations 3



int *clientPerClass;
float *X, *D, *Q;
int delayStations[numDelayStations]={0, 4, 5};
int Nnorm=sqrt(pow(304, 2) + pow(240, 2));
float *R=new float[N*M];
string *stationNames = new string[M];

float a(int n){
    return n<65? 0.4+0.6*((float)n): 38.8;
}


float p0Calc(){
    float sumtemp=0;
    for(int i=0; i<N; i++){
        sumtemp+=X[i]*D[24+i];
    }

    float sum=0;
    for(int k=1; k<=Nnorm; k++){
        float multemp=1;
        for(int l=1; l<=k; l++){
            multemp*=sumtemp/a(l);
        }
        sum+=multemp;
    }
    sum++;
    return 1/sum;
}


void apprMVA(){

    //Load Dependent
    float *p=new float[Nnorm+1];
    p[0] = p0Calc();
    float sumtemp=0;
    for(int i=0; i<N; i++){
        sumtemp+=X[i]*D[24+i];
    }
    for(int k=1; k<=Nnorm; k++){
        p[k]=p[k-1]*sumtemp/a(k);
    }
    float Rsum=0;
    for(int k=1; k<=Nnorm; k++){
        Rsum+=p[k-1]*(float)k/a(k);
    }
    R[24]=Rsum*D[24];
    R[25]=Rsum*D[25];

    //Delay Stations
    for(int i=0; i<numDelayStations; i++){
        R[delayStations[i]*2] = D[delayStations[i]*2];
        R[delayStations[i]*2+1] = D[delayStations[i]*2+1];
    }

    //Load Independent

    for(int station=0; station<M; station++){
        bool LI=true;
        if(station==12) LI=false;
        for(int i=0; i<numDelayStations; i++){
            if(station==delayStations[i]) LI=false;
        }
        if(LI){
            R[station*2]=D[station*2]*(1+(float)(clientPerClass[0]-1)/(float)clientPerClass[0]*Q[2*station]+Q[2*station+1]);
            R[station*2+1]=D[station*2+1]*(1+(float)(clientPerClass[1]-1)/(float)clientPerClass[1]*Q[2*station+1]+Q[2*station]);

        }
    }

    float *RsumTotal= new float[N];
    RsumTotal[0]=0;
    RsumTotal[1]=0;
    for(int i=0; i<M; i++){
        RsumTotal[0]+=R[i*2];
        RsumTotal[1]+=R[i*2+1];
    }

    //Calculate X
    X[0]=(float)clientPerClass[0]/RsumTotal[0];
    X[1]=(float)clientPerClass[1]/RsumTotal[1];

    //Recalculate Q
    for(int i=0; i<M; i++){
        if(i==12) continue;
        Q[2*i]=X[0]*R[2*i];
        Q[2*i+1]=X[1]*R[2*i+1];
    }

    delete[] p;
    delete[] RsumTotal;
}


void printSeparator(int length) {
        cout << "+";
        for (int i = 0; i < length; ++i) cout << "-";
        cout << "+" << endl;
    };


int main(){

    clientPerClass = new int[N];
    clientPerClass[0]=304;
    clientPerClass[1]=240;

    D = new float[N*M];
    D[0]=25000;
    D[1]=29000;
    D[2]=32;
    D[3]=0;
    D[4]=0;
    D[5]=90;
    D[6]=48;
    D[7]=134;
    D[8]=25;
    D[9]=0;
    D[10]=0;
    D[11]=16;
    D[12]=59;
    D[13]=0;
    D[14]=70;
    D[15]=0;
    D[16]=0;
    D[17]=28;
    D[18]=0;
    D[19]=25;
    D[20]=48;
    D[21]=58;
    D[22]=54;
    D[23]=66;
    D[24]=69;
    D[25]=106;
    D[26]=67;
    D[27]=72;
    D[28]=88;
    D[29]=96;

    stationNames[0] = "Clients";
    stationNames[1] = "LAN1";
    stationNames[2] = "LAN2";
    stationNames[3] = "LAN3";
    stationNames[4] = "R1";
    stationNames[5] = "R2";
    stationNames[6] = "WS1 CPU";
    stationNames[7] = "WS1 Disk";
    stationNames[8] = "WS2 CPU";
    stationNames[9] = "WS2 Disk";
    stationNames[10] = "AS CPU";
    stationNames[11] = "AS Disk";
    stationNames[12] = "DdS CPU";
    stationNames[13] = "DdS Disk1";
    stationNames[14] = "DdS Disk2";

    Q= new float[N*M];
    for(int station=0; station<M; station++){
        Q[2*station] = (float)clientPerClass[0]/(float)M;
        Q[2*station+1] = (float)clientPerClass[1]/(float)M;
    }

    X=new float[N];
    float *Dmax=new float[N];
    Dmax[0]=0;
    Dmax[1]=0;

    float *Dsum=new float[N];
    Dsum[0]=0;
    Dsum[1]=0;

    for(int i=0; i<M; i++){
        if(Dmax[0]<D[2*i]) Dmax[0]=D[2*i];
        if(Dmax[1]<D[2*i+1]) Dmax[1]=D[2*i+1];
        Dsum[0]+=D[2*i];
        Dsum[1]+=D[2*i+1];
    }

    for(int i=0; i<N; i++){
        X[i] = min(1/Dmax[i], (float)clientPerClass[i]/Dsum[i]);
    }


    float *prevX=new float[N];
    for(int i=0; i<N; i++){
        prevX[i]=-100;
    }

    while(1){
        apprMVA();
        float diff0=X[0]-prevX[0];
        float diff1=X[1]-prevX[1];
        if(abs(diff0)<0.0000001 && abs(diff1)<0.0000001) break;
        prevX[0]=X[0];
        prevX[1]=X[1];
    }

    //Calculating Q for station 12 that is Load Dependent
    Q[24] = X[0] * R[24];
    Q[25] = X[1] * R[25];

    cout<<Q[24]<<endl;

    cout << fixed << setprecision(5);  // Set precision for floating point numbers

    

    const int columnWidth = 15;

    // Throughput
    cout << "Throughput" << endl;
    printSeparator(22);
    cout << "| CL1: " << setw(columnWidth) << X[0] << " |" << endl;
    cout << "| CL2: " << setw(columnWidth) << X[1] << " |" << endl;
    printSeparator(22);
    cout << endl;

    // Residence time
    cout << "Residence time" << endl;
    printSeparator(53);
    cout << "| " << setw(15) << left << "Station" << " | " << setw(columnWidth) << "CL1" << " | " << setw(columnWidth) << "CL2" << " |" << endl;
    printSeparator(53);
    for (int i = 0; i < M; i++) {
        cout << "| " << setw(15) << left << stationNames[i]
             << " | " << setw(columnWidth) << R[2 * i]
             << " | " << setw(columnWidth) << R[2 * i + 1] << " |" << endl;
    }
    printSeparator(53);
    cout << endl;

    // Number of Customers
    cout << "Number of Customers" << endl;
    printSeparator(71);
    cout << "| " << setw(15) << left << "Station" << " | " << setw(columnWidth) <<"Aggregate"<<" | " << setw(columnWidth) << "CL1" << " | " << setw(columnWidth) << "CL2" << " |" << endl;
    printSeparator(71);
    for (int i = 0; i < M; i++) {
        cout << "| " << setw(15) << left << stationNames[i]
             << " | " << setw(columnWidth) << Q[2 * i] + Q[2*i+1]
             << " | " << setw(columnWidth) << Q[2 * i]
             << " | " << setw(columnWidth) << Q[2 * i + 1] << " |" << endl;
    }
    printSeparator(71);
    cout << endl;

    // Utilization
    cout << "Utilization" << endl;
    printSeparator(71);
    cout << "| " << setw(15) << left << "Station" << " | " << setw(columnWidth) <<"Aggregate"<<" | " << setw(columnWidth)  << "CL1" << " | " << setw(columnWidth) << "CL2" << " |" << endl;
    printSeparator(71);
    for (int i = 0; i < M; i++) {
        cout << "| " << setw(15) << left << stationNames[i]
             << " | " << setw(columnWidth) << X[0] * D[2 * i] + X[1] * D[2 * i + 1]
             << " | " << setw(columnWidth) << X[0] * D[2 * i]
             << " | " << setw(columnWidth) << X[1] * D[2 * i + 1] << " |" << endl;
    }
    printSeparator(71);
    cout << endl;

    delete[] clientPerClass;
    delete[] X;
    delete[] D;
    delete[] Q;
    
}