#include <vector>
#include <string>
#include <math.h>
#include <utility>
#include <algorithm>
#include <iostream>

using namespace std;

int knn(vector<vector<double>> &conjunto, vector<int> &clase, vector<double> &imagen, int k)//paso mucho por referencia para no tener que hacer copias de cosas enormes
{
    //conjunto es las imagenes del conjunto de teaching
    //clase tiene en el elmento i la clase de la imagen i de teaching
    //imagen es la imagen que paso para determinar su clase
    //k es la cantidad de vecinos cercanos que se ve
    vector<pair<double,int>> distancia;

     for(int i=0;i<conjunto.size();i++)
     {
        double aux=0;
        for(int j=0;j<conjunto[i].size();j++)
        {
            double aux2=(conjunto[i][j]-imagen[j]);
            aux=aux+aux2*aux2;
        }
        aux = sqrt(aux);//aca tengo la distancia entre la imagen actual y la imagen i

        distancia.push_back(make_pair(aux,i));
     }

     //ahora el vector distancia ya tiene las distancias euclideanas. Solo queda ordenar estas distancias y obtener los k mas cercanos
    sort(distancia.begin(), distancia.end());

    //ahora que de aca es facil obtener los k mas cercanos con second es clave. la forma mas eficiente de contar apariciones de los elementos del string es hacer un sort primero para hacerlo
    //en k log k
    vector<int> ClCer;//clase de los cercanos
    for(int i=0;i<k;i++)
    {
        ClCer.push_back(clase[distancia[i].second]);
    }
    sort(ClCer.begin(), ClCer.end());

     //solo resta encontrar el que tiene mayor cantidad de elementos consecutivos

    int CanMax=1;
    int mayor=ClCer[0];
    int CanAct=1;
// cout<<ClCer[0]<<endl;
    for(int i = 1; i < k; i++)
    {
     //   cout<<ClCer[i]<<endl;
        if (ClCer[i]==ClCer[i-1])
        {
            CanAct++;
        } else
        {
            if (CanAct>CanMax)
            {
                CanMax=CanAct;
                mayor=ClCer[i-1];
            }
            CanAct=1;
        }

    }

    if (CanAct>CanMax)
    {
        CanMax=CanAct;
        mayor=ClCer[k-1];
    }

    return mayor;

}
