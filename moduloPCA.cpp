#include <math.h>
#include <random>
#include <stdexcept>
#include <iostream>
#include "moduloPCA.h"

PCA::PCA() : _hizoLaMAlterna(false) {
    //
}

std::vector<double> PCA::obtenerMedia(const std::vector<std::pair<std::vector<double>, int>> &conjuntoDeImagenes) const{
    int cantidadDeVectores = conjuntoDeImagenes.size();
    std::vector<double> promedioVectores(conjuntoDeImagenes[0].first.size(), 0);

    //Hago la suma de los vectores sobre la cantidad total de vectores
    for(int i = 0; i < cantidadDeVectores; i++){
        for(int j = 0; j < conjuntoDeImagenes[i].first.size(); j++){
            promedioVectores[j] += (conjuntoDeImagenes[i].first[j] / cantidadDeVectores);
        }
    }

    return promedioVectores;
}

std::vector<std::vector<double>> PCA::obtenerMatrizM(
        const std::vector<std::pair<std::vector<double>, int>> &conjuntoDeImagenes, const std::vector<double> &vectorMedia) {
    unsigned int cantidadDeVectores = conjuntoDeImagenes.size();

    //Cant elementos de cada vector
    unsigned int cantidadDeColumnas = conjuntoDeImagenes[0].first.size();
    std::vector<std::vector<double>> matrizX;

    //Lleno la matriz X con los datos que corresponden
    for(int fila = 0; fila < cantidadDeVectores; fila++){
        std::vector<double> filaActual(cantidadDeColumnas, 0);

        for(int columna = 0; columna < cantidadDeColumnas; columna++){
            filaActual[columna] = (conjuntoDeImagenes[fila].first[columna] - vectorMedia[columna]) / sqrt((double) cantidadDeVectores - 1);
        }

        matrizX.push_back(filaActual);
    }

    //Deberia estar formada la matrizX

    //Ahora calculo la transpuesta
    //Si matrizX es cantidadDeVectores * cantidadDeColumnas -> matrizXt es cantidadDeColumnas * cantidadDeVectores
    std::vector<std::vector<double>> matrizXt(transponerMatriz(matrizX));

    //Me guardo la matrizXt ya calculada
  _matXt = std::vector<std::vector<double>>(matrizXt);
//    _matXt= std::vector<std::vector<double>>(matrizX);

    //Ahora si cantidadDeColumnas es mas grande tengo que multiplicar al reves asi la matriz M es más chica
    unsigned int cantFilasMatrzM = 0;
    unsigned int cantColumnasMatrizM = 0;

    if(cantidadDeColumnas > cantidadDeVectores){
        //Hay que hacerlo al revez
        _hizoLaMAlterna = true;
        cantFilasMatrzM = cantidadDeVectores;
        cantColumnasMatrizM = cantidadDeVectores;

    }else{
        //Hay que hacerla normal
        _hizoLaMAlterna = false;
        cantFilasMatrzM = cantidadDeColumnas;
        cantColumnasMatrizM = cantidadDeColumnas;
    }

    //Ahora puedo calcular la matriz M haciendo Xt * X
    //M va a ser de tamaño cantidadDeColumnas * cantidadDeColumnas

    //std::cout << "FILAS: " << cantFilasMatrzM << std::endl;
    //std::cout << "COLUMNAS: " << cantColumnasMatrizM << std::endl;

    std::vector<std::vector<double>> matrizM(cantFilasMatrzM, std::vector<double>(cantColumnasMatrizM, 0));

    //Multiplico Xt * X o X * Xt
    for(int fila = 0; fila < cantFilasMatrzM; fila++){
        for(int columna = 0; columna < cantColumnasMatrizM; columna++){
            for(int k = 0; k < ((_hizoLaMAlterna)? cantidadDeColumnas : cantidadDeVectores); k++){
                //
                if(_hizoLaMAlterna){
                    matrizM[fila][columna] += matrizX[fila][k] * matrizXt[k][columna];
                }
                else{
                    matrizM[fila][columna] += matrizXt[fila][k] * matrizX[k][columna];
                }
            }
        }
    }

    //Ya esta formada la matriz M
    return matrizM;
}

std::vector<std::pair<std::vector<double>, double >> PCA::calcularAutovalYAutoVec(
        const std::vector<std::vector<double>> &matrizM, int alfa, double tolerancia, int limiteCiclos){
    //Solo calculo los necesarios
    std::vector<std::pair<std::vector<double>, double >> autoVecYAutoval(alfa);

    //Copio la matriz pasada para poder aplicar el metodo de las potencias
    std::vector<std::vector<double>> matrizAux(matrizM);

    //Para cada autovalor
    for(int i = 0; i < alfa; i++){

        //Guardo la cantidad de filas
        int filas = matrizM.size();

        //Creo un vector al azar
        std::vector<double> vectorX = generarVectorAlAzar(filas);

        //Creo un vector para poder comparar las distintas iteraciones
        std::vector<double> vectorAnterior = vectorX;

        //La distancia inicial de los dos vectores
        double distanciaVectores = tolerancia;

        int ciclosRealizados = 0;

        //Cuando se acerquen los vectores se habra llegado al autovector
        while(distanciaVectores >= tolerancia && ciclosRealizados < limiteCiclos){

            vectorX = multiplicarMatrizVector(matrizAux, vectorX);

            double normaVector = 1 / (calcularNorma(vectorX));

            for(int j = 0; j < vectorX.size(); j++){
                vectorX[j] = vectorX[j] * normaVector;
            }

            std::vector<double> resta(vectorX.size());

            for(int j = 0; j < vectorX.size(); j++){
                resta[j] = vectorX[j] - vectorAnterior[j];
            }

            distanciaVectores = calcularNorma(resta);

            vectorAnterior = vectorX;

            ciclosRealizados++;
        }

        //Tengo el autovector, ahora calculo el autovalor
        double autovalor = calcularAutovalor(matrizAux, vectorX);
        //std::cout<<vectorX[0]<<" "<<vectorX[1]<<" "<<vectorX[2]<<" "<<vectorX[3]<<" "<<std::endl;
        //Los guardo
        //Si la M se hizo al revez que que hacer Xt * vectorX
        if(_hizoLaMAlterna){
            std::vector<double> eigenvector = multiplicarMatrizVector(_matXt, vectorX);

            double normaVector = 1 / (calcularNorma(eigenvector));

            for(int j = 0; j < eigenvector.size(); j++){
                eigenvector[j] = eigenvector[j] * normaVector;
            }

            autoVecYAutoval[i].first = eigenvector;
            //std::cout<<"vector"<<eigenvector[0]<<" "<<eigenvector[1]<<" "<<eigenvector[2]<<" "<<eigenvector[3]<<" "<<std::endl;
            autoVecYAutoval[i].second = autovalor;
            //std::cout<<autovalor<<std::endl;
        }
        else{
            autoVecYAutoval[i].first = vectorX;
              //   std::cout<<"vector"<<vectorX[0]<<" "<<vectorX[1]<<" "<<vectorX[2]<<" "<<vectorX[3]<<" "<<std::endl;
            autoVecYAutoval[i].second = autovalor;
            //std::cout<<autovalor<<std::endl;
        }

        //Uso deflacion para poder buscar los proximos autovec y autoval
        //A tengo que hacer matrizAux = matrizAux - autovalor * autovector * autovector^t

        //Construyo autovalor autovector * autovector^t
        std::vector<std::vector<double>> matrizResta(vectorX.size(), std::vector<double>(vectorX.size()));

        for(int fila = 0; fila < matrizResta.size() ; fila++){
            for(int columna = 0; columna < matrizResta[fila].size(); columna++){
                matrizResta[fila][columna] = autovalor * vectorX[fila] * vectorX[columna];

            }
        }

        //Se lo resto a matrizAux
        for(int fila = 0; fila < matrizAux.size() ; fila++){
            for(int columna = 0; columna < matrizAux[fila].size(); columna++){
                matrizAux[fila][columna] = matrizAux[fila][columna] - matrizResta[fila][columna];
            }
        }

        //Deberia funcionar
    }

    return autoVecYAutoval;
}

std::vector<double> PCA::generarVectorAlAzar(int &dimension) {
    std::vector<double> vecAlAzar(dimension, 0);

    //De 0 a 255 (no deberia importar mucho este rango)
    for(int i = 0; i < dimension; i++){
        vecAlAzar[i] = rand() % 256;
    }

    return vecAlAzar;
}

double PCA::calcularNorma(std::vector<double> vector) {
    double suma = 0;

    for(int i = 0; i < vector.size(); i++){
        suma += vector[i] * vector[i];
    }

    suma = sqrt(suma);

    return suma;
}

std::vector<std::vector<double>> PCA::transponerMatriz(const std::vector<std::vector<double>> &matriz) const{
    //Calculo la transpuesta

    //Si matriz es matriz.size * matriz[i].size (para cualquier i) -> matrizT es matriz[i].size * matriz.size
    std::vector<std::vector<double>> matrizT(matriz[0].size(), std::vector<double>(matriz.size()));

    for(int fila = 0; fila < matriz.size(); fila++){
        //
        for(int columna = 0; columna < matriz[0].size(); columna++){
            matrizT[columna][fila] = matriz[fila][columna];
        }
    }

    return matrizT;
}

//Hace la multiplicacion A*x donde A es matriz y x vector
std::vector<double> PCA::multiplicarMatrizVector(const std::vector<std::vector<double>> &matriz, const std::vector<double> &vec) const{
    if(matriz[0].size() != vec.size()){
        throw std::runtime_error("EL TAMAÑO DE LA MATRIZ Y EL VECTOR NO COINCIDEN");
    }

    //Se puede multiplicar
    //std::vector<double> resultado(vec.size(), 0);

    std::vector<double> resultado(matriz.size(), 0);

    for(int elem = 0; elem < matriz.size(); elem++){

        double sum = 0;

        for(int k = 0; k < matriz[0].size(); k++){
            sum += matriz[elem][k] * vec[k];
        }

        resultado[elem] = sum;
    }

    return resultado;
}

//Calcula estimado del autovalor con el Rayleigh Quotient
//si autovector es autovector de matrizM entonces esta funcion devuelve el autovalor exacto
double PCA::calcularAutovalor(const std::vector<std::vector<double>> &matrizM, const std::vector<double> &autovector) const{
    //Tengo que calcular:
    // (matrizM * autovector)^t * autovector
    // autovector^t * autvector

    //(matrizM * autovector)
    std::vector<double> mulAv = multiplicarMatrizVector(matrizM, autovector);

    double vtv = 0;
    // autovector^t * autvector
    for(int i = 0; i < autovector.size(); i++){
        vtv += autovector[i] * autovector[i];
    }

    //(matrizM * autovector)^t * autovector

    double Avtv = 0;
    for(int i = 0; i < mulAv.size(); i++){
        Avtv += mulAv[i] * autovector[i];
    }

    // ((matrizM * autovector)^t * autovector) / (autovector^t * autvector)
    return (Avtv / vtv);
}

std::vector<double> PCA::transformacionCaracteristica(std::vector<std::vector<double>> &autovectores,
                                                      std::vector<double> &imagenVectorizada) {
    std::vector<double> transformacionCaracteristica(autovectores.size(), 0);

    for(int i = 0; i < autovectores.size(); i++){
        for(int k = 0; k < imagenVectorizada.size(); k++){
            transformacionCaracteristica[i] += autovectores[i][k] * imagenVectorizada[k];
        }
    }

    return transformacionCaracteristica;
}
