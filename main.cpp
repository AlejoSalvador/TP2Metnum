#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cstring>
#include "cargadorDeImagenes.h"
#include "knn.hpp"
#include "moduloPCA.h"
#include "kfold.hpp"

void operacionkNN(std::string trainset, std::string testSet, std::string classif, int valorK);
void operacionkNNPCA(std::string trainset, std::string testSet, std::string classif, int valorAlfa, double valorTolerancia, int valorK, int limiteDeCiclos);

//Hacen el mismo proceso que las de arriba pero devuelven el accuracy, son usadas para el KFold
double operacionkNN2(std::vector< std::pair<std::vector<double>, int>> trainset, std::vector< std::pair<std::vector<double>, int> > testSet, int valorK);
double operacionkNNPCA2(std::vector< std::pair<std::vector<double>, int>> trainset, std::vector< std::pair<std::vector<double>, int> > testSet, int valorAlfa, double valorTolerancia, int valorK, int limiteDeCiclos);

void pruebaPCA(std::string trainset, std::string testSet, std::string classif, int valorAlfa, double valorTolerancia, int limiteDeCiclos);
void pruebaKFold(std::string trainset, int KDeFold, int valorK, int valorAlfa, double valorTolerancia, int limiteDeCiclos, int modo);

int main(int cantidadDeArgumentos, char** argumentos) {

    //argumentos: ./tp2 -m <METHOD> -i <TRAINSET> -q <TESTSET> -o <CLASSIF>

    std::cout << "ARGUMENTOS: " << " ";

    for(int i=0;i<cantidadDeArgumentos;i++){
        std::cout << argumentos[i] << " ";
    }

    std::cout << std::endl;

    std::string metodo;
    std::string trainSet;
    std::string testSet;
    std::string classif;

    if(cantidadDeArgumentos < 9){
        return 1;
    }

    //Procesamos los argumentos del programa
    //Los argumentos van a estar siempre en las posiciones impares
    //ya que el primer arg es el nombre del archivo
    for(int i = 1; i < cantidadDeArgumentos; i = i+2){

        if((i + 1 < cantidadDeArgumentos)){

            if(strcmp(argumentos[i], "-m") == 0){
                metodo = argumentos[i+1];
            }
            else if(strcmp(argumentos[i], "-i") == 0){
                trainSet = argumentos[i+1];
            }
            else if(strcmp(argumentos[i], "-q") == 0){
                testSet = argumentos[i+1];
            }
            else if(strcmp(argumentos[i], "-o") == 0){
                classif = argumentos[i+1];
            }
        }
    }

    //Modificar estos valores para poder ver otros resultados

    //k de kNN
    int valorK = 4;

    //alfa de la transformacion caracteristica (Solo se usa en kNNyPCA)
    int valorAlfa = 30;

    //Distancia minima que tienen que tener los autovalores calculados en PCA
    //con valores mas chicos se obtienen autovectores y autovalores mas precisos pero se tarda más
    double tolerancia = 0.0001;

    //Limite de ciclos usados para calcular autovectores y autovalores
    int limiteDeCiclos = 1000000;

    //K del Kfold
    int valorKFold = 5;

    //Ahora opero según lo ingresado
    if(metodo == "0"){
        //Solo se hace kNN
        operacionkNN(trainSet, testSet, classif, valorK);

    }else if(metodo == "1"){
        //Se hace kNN + PCA
        operacionkNNPCA(trainSet, testSet, classif, valorAlfa, tolerancia, valorK, limiteDeCiclos);

    }else if(metodo == "2"){
        //Se hace kNN + PCA
        pruebaPCA(trainSet, testSet, classif, valorAlfa, tolerancia, limiteDeCiclos);

        //Para probar el testSet
        //pruebaPCA(testSet, trainSet, classif, valorAlfa, tolerancia, limiteDeCiclos);

    }else if(metodo == "3"){
        //Se hace kNN + PCA
        pruebaKFold(trainSet, valorKFold, valorK, valorAlfa, tolerancia, limiteDeCiclos, 0);
    }else if (metodo=="4"){
         pruebaKFold(trainSet, valorKFold, valorK, valorAlfa, tolerancia, limiteDeCiclos, 1);
    }else {
        //No se hace nada por ahora
        std::cout << "Los metodos válidos son:" << std::endl;
        std::cout << "\t0 - Identificar imagenes usando KNN" << std::endl;
        std::cout << "\t1 - Identificar imagenes usando KNN y PCA" << std::endl;
        std::cout << "\t2 - Test para verificar si los autovalores encontrados son correctos" << std::endl;
        std::cout << "\t3 - Test de KFold sobre el conjunto de entrenamiento" << std::endl;
    }

    return 0;
}

void operacionkNN(std::string trainset, std::string testSet, std::string classif, int valorK){

    ofstream archivoOUT(classif, std::ofstream::out);
    std::stringstream conversor;
    conversor.str("");
    conversor.clear();

    cargadorDeImagenes baseDeEntrenamiento(trainset.c_str());
    cargadorDeImagenes baseDePrueba(testSet.c_str());

    std::vector<std::vector<double>> vecImagenesBase = baseDeEntrenamiento.vectoresDeImagenes();
    std::vector<int> clases = baseDeEntrenamiento.clases();

    int imagenesAcertadas = 0;

    //Vector que tiene las ID reales de las pruebas
    //IDrealesDeBaseDePrueba[i] es la ID del vector guardado en baseDePrueba.vectoresDeImagenes()[i]
    std::vector<int> IDrealesDeBaseDePrueba = baseDePrueba.clases();

    //Realizo el reconocimiento mediante kNN
    //Para cada imagen a reconocer su ID
    for(int imagen = 0; imagen < baseDePrueba.conjuntoDeImagenes().size(); imagen++){

        //Le averiguo el knn a la imagen
        int IDpropuesta = knn(vecImagenesBase, clases, baseDePrueba.vectoresDeImagenes()[imagen], valorK);

        //Aca puedo chequear el accuracy antes de seguir!

        if(IDpropuesta == IDrealesDeBaseDePrueba[imagen]){
            //La imagen se encontro bien!
            imagenesAcertadas++;
        }

        //Ahora tengo que escribir en el archivo

        std::string lineaSalida;

        conversor << baseDePrueba.rutas()[imagen] + ", ";
        conversor << IDpropuesta;
        conversor << ",\n";

        lineaSalida = conversor.str();

        archivoOUT << lineaSalida;

        //Limpio el conversor
        conversor.str("");
        conversor.clear();
    }

    archivoOUT.close();

    //Ahora muestro lo demas
    double accuracy =  ((double)imagenesAcertadas/baseDePrueba.conjuntoDeImagenes().size());

    std::cout << "-------------------------RESULTADOS-------------------------" << std::endl;
    std::cout << "Metodo elegido: 0 - Identificar imagenes usando KNN" << std::endl;
    std::cout << "Archivo usado para el trainset: " << trainset << std::endl;
    std::cout << "Archivo usado para el testset: " << testSet << std::endl;
    std::cout << "Nombre del archivo de salida: " << classif << std::endl;
    std::cout << "Valor de k utilizado en kNN: " << valorK << std::endl;
    std::cout << "ACCURACY: " << (accuracy * 100) << "% de imagenes acertadas" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
}

void operacionkNNPCA(std::string trainset, std::string testSet, std::string classif, int valorAlfa,
                     double valorTolerancia, int valorK, int limiteDeCiclos){

    int alfa = valorAlfa;
    double tolerancia = valorTolerancia;

    ofstream archivoOUT(classif, std::ofstream::out);
    std::stringstream conversor;
    conversor.str("");
    conversor.clear();

    cargadorDeImagenes baseDeEntrenamiento(trainset.c_str());
    cargadorDeImagenes baseDePrueba(testSet.c_str());

    std::vector<std::vector<double>> vecImagenesBase = baseDeEntrenamiento.vectoresDeImagenes();
    std::vector<int> clases = baseDeEntrenamiento.clases();

    //Preproceso con PCA las imagenes de la base
    PCA modPCA;

    std::vector<double> vectorMedia = modPCA.obtenerMedia(baseDeEntrenamiento.conjuntoDeImagenes());
    std::vector<std::vector<double>> matrizM = modPCA.obtenerMatrizM(baseDeEntrenamiento.conjuntoDeImagenes(), vectorMedia);
    std::vector<std::pair<std::vector<double>, double >> autoVecYAutoVal = modPCA.calcularAutovalYAutoVec(matrizM, alfa, tolerancia, limiteDeCiclos);

    std::vector<std::vector<double>> autovectoresBasePrueba(autoVecYAutoVal.size());
    for(int j = 0; j < autoVecYAutoVal.size(); j++){
        autovectoresBasePrueba[j] = autoVecYAutoVal[j].first;
    }

    //Aplico la transformacion a los vectores
    for(int i = 0; i < vecImagenesBase.size(); i++){
        vecImagenesBase[i] = modPCA.transformacionCaracteristica(autovectoresBasePrueba, vecImagenesBase[i]);
    }

    //Preproceso con PCA las imagenes de test
    std::vector<std::vector<double>> vecImagenesTest(baseDePrueba.conjuntoDeImagenes().size());

    //Aplico la transformacion a los vectores
    for(int i = 0; i < vecImagenesTest.size(); i++){
        vecImagenesTest[i] = modPCA.transformacionCaracteristica(autovectoresBasePrueba, baseDePrueba.vectoresDeImagenes()[i]);
    }

    int imagenesAcertadas = 0;
    std::vector<int> IDrealesDeBaseDePrueba = baseDePrueba.clases();

    //Realizo el reconocimiento mediante kNN
    //Para cada imagen a reconocer su ID
    for(int imagen = 0; imagen < baseDePrueba.conjuntoDeImagenes().size(); imagen++){

        //Le averiguo el knn a la imagen
        int IDpropuesta = knn(vecImagenesBase, clases, vecImagenesTest[imagen], valorK);

        if(IDpropuesta == IDrealesDeBaseDePrueba[imagen]){
            //La imagen se encontro bien!
            imagenesAcertadas++;
        }

        //Ahora tengo que escribir en el archivo

        std::string lineaSalida;

        conversor << baseDePrueba.rutas()[imagen] + ", ";
        conversor << IDpropuesta;
        conversor << ",\n";

        lineaSalida = conversor.str();

        archivoOUT << lineaSalida;

        //Limpio el conversor
        conversor.str("");
        conversor.clear();
    }

    archivoOUT.close();

    //Ahora muestro lo demas

    double accuracy =  ((double)imagenesAcertadas/baseDePrueba.conjuntoDeImagenes().size());

    std::cout << "-------------------------RESULTADOS-------------------------" << std::endl;
    std::cout << "Metodo elegido: 1 - Identificar imagenes usando KNN y PCA" << std::endl;
    std::cout << "Archivo usado para el trainset: " << trainset << std::endl;
    std::cout << "Archivo usado para el testset: " << testSet << std::endl;
    std::cout << "Nombre del archivo de salida: " << classif << std::endl;
    std::cout << "Valor de k utilizado en kNN: " << valorK << std::endl;
    std::cout << "Valor de alfa utilizado en PCA: " << alfa << std::endl;
    std::cout << "Tolerancia usada para calcular autovalores y autovectores en PCA: " << tolerancia << std::endl;
    std::cout << "ACCURACY: " << (accuracy * 100) << "% de imagenes acertadas" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
}

void pruebaPCA(std::string trainset, std::string testSet, std::string classif, int valorAlfa, double valorTolerancia, int limiteDeCiclos){

    //Acá le aplicas PCA a todo
    PCA modPCA;

    cargadorDeImagenes baseDeEntrenamiento(trainset.c_str());

    std::vector<double> mediaBase = modPCA.obtenerMedia(baseDeEntrenamiento.conjuntoDeImagenes());

    std::vector<std::vector<double>> matrizM = modPCA.obtenerMatrizM(baseDeEntrenamiento.conjuntoDeImagenes(), mediaBase);

    int alfa = valorAlfa;

    std::vector<std::pair<std::vector<double>, double >> autovecYAutoval = modPCA.calcularAutovalYAutoVec(matrizM, alfa, valorTolerancia, limiteDeCiclos);

    std::vector<std::vector<double>> autovectores(autovecYAutoval.size());

    for(int i = 0; i < autovecYAutoval.size(); i++){
        autovectores[i] = autovecYAutoval[i].first;
    }

    std::vector<double> autovalores(autovecYAutoval.size());

    for(int i = 0; i < autovecYAutoval.size(); i++){
        autovalores[i] = autovecYAutoval[i].second;
    }

    //Ordeno los autoval acá de mayor a menor
    std::sort(autovalores.rbegin(), autovalores.rend());

    //muestro las raices
    for(int i = 0; i < autovecYAutoval.size(); i++){
        std::cout << "AUTOVALOR " << i << ": " << sqrt(autovalores[i]) << std::endl;
    }
}

double operacionkNN2(std::vector< std::pair<std::vector<double>, int>> trainset, std::vector< std::pair<std::vector<double>, int> > testSet, int valorK){

    int imagenesAcertadas = 0;

    std::vector<std::vector<double>> vecImagenesBase(trainset.size());

    for(int i = 0; i < vecImagenesBase.size(); i++){
        vecImagenesBase[i] = trainset[i].first;
    }

    std::vector<int> clases(trainset.size());

    for(int i = 0; i < clases.size(); i++){
        clases[i] = trainset[i].second;
    }

    //Realizo el reconocimiento mediante kNN
    //Para cada imagen a reconocer su ID
    for(int imagen = 0; imagen < testSet.size(); imagen++){

        //Le averiguo el knn a la imagen
        int IDpropuesta = knn(vecImagenesBase, clases, testSet[imagen].first, valorK);

        //Aca puedo chequear el accuracy antes de seguir!

        if(IDpropuesta == testSet[imagen].second){
            //La imagen se encontro bien!
            imagenesAcertadas++;
        }
    }

    //Ahora muestro lo demas
    double accuracy =  ((double)imagenesAcertadas/testSet.size());

    return accuracy;
}

double operacionkNNPCA2(std::vector< std::pair<std::vector<double>, int>> trainset, std::vector< std::pair<std::vector<double>, int> > testSet, int valorAlfa, double valorTolerancia, int valorK, int limiteDeCiclos){
    int alfa = valorAlfa;
    double tolerancia = valorTolerancia;

    std::vector<std::vector<double>> vecImagenesBase(trainset.size());
    std::vector<int> clases(trainset.size());

    for(int i = 0; i < clases.size(); i++){
        clases[i] = trainset[i].second;
    }

    //Preproceso con PCA las imagenes de la base
    PCA modPCA;

    std::vector<double> vectorMedia = modPCA.obtenerMedia(trainset);
    std::vector<std::vector<double>> matrizM = modPCA.obtenerMatrizM(trainset, vectorMedia);
    std::vector<std::pair<std::vector<double>, double >> autoVecYAutoVal = modPCA.calcularAutovalYAutoVec(matrizM, alfa, tolerancia, limiteDeCiclos);

    std::vector<std::vector<double>> autovectoresBasePrueba(autoVecYAutoVal.size());
    for(int j = 0; j < autoVecYAutoVal.size(); j++){
        autovectoresBasePrueba[j] = autoVecYAutoVal[j].first;
    }

    //Aplico la transformacion a los vectores
    for(int i = 0; i < vecImagenesBase.size(); i++){
        vecImagenesBase[i] = modPCA.transformacionCaracteristica(autovectoresBasePrueba, trainset[i].first);
    }

    //Preproceso con PCA las imagenes de test
    std::vector<std::vector<double>> vecImagenesTest(testSet.size());

    //Aplico la transformacion a los vectores
    for(int i = 0; i < vecImagenesTest.size(); i++){
        vecImagenesTest[i] = modPCA.transformacionCaracteristica(autovectoresBasePrueba, testSet[i].first);
    }

    int imagenesAcertadas = 0;

    //Realizo el reconocimiento mediante kNN
    //Para cada imagen a reconocer su ID
    for(int imagen = 0; imagen < testSet.size(); imagen++){

        //Le averiguo el knn a la imagen
        int IDpropuesta = knn(vecImagenesBase, clases, vecImagenesTest[imagen], valorK);

        if(IDpropuesta == testSet[imagen].second){
            //La imagen se encontro bien!
            imagenesAcertadas++;
        }

    }

    //Ahora muestro lo demas
    double accuracy = ((double) imagenesAcertadas/testSet.size());

    return accuracy;
}

void pruebaKFold(std::string trainset, int KDeFold, int valorK, int valorAlfa, double valorTolerancia, int limiteDeCiclos, int modo){
    //Pruebo el trainset con KFold
    cargadorDeImagenes baseDeEntrenamiento(trainset.c_str());

    std::cout << "TAM CONJUNTO: " << baseDeEntrenamiento.conjuntoDeImagenes().size() << std::endl;

    //Parto el conjunto de imagenes en KDeFold partes iguales

    Kfold<std::vector<std::pair<std::vector<double>, int>>> kf(KDeFold, baseDeEntrenamiento.conjuntoDeImagenes());

    double sumAccu = 0;

    if(modo == 0){
        //KFold con kNN solamente
        for(int particion = 0; particion != KDeFold ; particion++){
            //Lleno train y test
            std::vector< std::pair<std::vector<double>, int> > train, test;
            kf.getFold(particion + 1, back_inserter(train), back_inserter(test));

            sumAccu += operacionkNN2(train, test, valorK);

            //Reinicio train y test para probar de nuevo
            train.clear();
            test.clear();
        }

        sumAccu = sumAccu / KDeFold;

        std::cout << "-------------------------RESULTADOS-------------------------" << std::endl;
        std::cout << "Metodo elegido: 3 - Test de KFold sobre el conjunto de entrenamiento (SOLO kNN)" << std::endl;
        std::cout << "Archivo usado para el trainset: " << trainset << std::endl;
        std::cout << "Valor de KFold: " << KDeFold << std::endl;
        std::cout << "Valor de k utilizado en kNN: " << valorK << std::endl;
        std::cout << "MEDIA DE ACCURACY: " << (sumAccu * 100) << "% de imagenes acertadas" << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;


    }else if(modo == 1){
        //KFold con kNN y PCA
        for(int particion = 0; particion != KDeFold ; particion++){
            //Lleno train y test
            std::vector< std::pair<std::vector<double>, int> > train, test;
            kf.getFold(particion + 1, back_inserter(train), back_inserter(test));

            sumAccu += operacionkNNPCA2(train, test, valorAlfa, valorTolerancia, valorK, limiteDeCiclos);

            //Reinicio train y test para probar de nuevo
            train.clear();
            test.clear();
        }

        sumAccu = sumAccu / KDeFold;

        std::cout << "ACCURACY CALCULADO CON KFOLD: " << (sumAccu * 100) << "%" << std::endl;

        std::cout << "-------------------------RESULTADOS-------------------------" << std::endl;
        std::cout << "Metodo elegido: 4 - Test de KFold sobre el conjunto de entrenamiento (kNN y PCA)" << std::endl;
        std::cout << "Archivo usado para el trainset: " << trainset << std::endl;
        std::cout << "Valor de KFold: " << KDeFold << std::endl;
        std::cout << "Valor de k utilizado en kNN: " << valorK << std::endl;
        std::cout << "Valor de alfa utilizado en PCA: " << valorAlfa << std::endl;
        std::cout << "Tolerancia usada para calcular autovalores y autovectores en PCA: " << valorTolerancia << std::endl;
        std::cout << "MEDIA DE ACCURACY: " << (sumAccu * 100) << "% de imagenes acertadas" << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
    }

}
