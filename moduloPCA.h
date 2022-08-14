#ifndef TRABAJOPRACTICO2METNUM_MODULOPCA_H
#define TRABAJOPRACTICO2METNUM_MODULOPCA_H

#include <vector>

class PCA{
public:
    //template <class T>
    PCA();

    //Devuelve el vector mu con la media de los vectores del conjunto
    std::vector<double> obtenerMedia(const std::vector<std::pair<std::vector<double>, int>> &conjuntoDeImagenes) const;

    //Calcula y Devuelve la matriz M (calcula los vectores que la forman)
    std::vector<std::vector<double>> obtenerMatrizM(const std::vector<std::pair<std::vector<double>, int>> &conjuntoDeImagenes, const std::vector<double> &vectorMedia);

    //Devuelve la lista de autovectores con los autovalores relacionados como
    //<autovector, autovalor>
    std::vector<std::pair<std::vector<double>, double >> calcularAutovalYAutoVec(const std::vector<std::vector<double>> &matrizM, int alfa, double tolerancia, int limiteCiclos);

    //Calcula la transformacion caracteristica de la imagen (ya vectorizada) con los autovectores de la matriz
    std::vector<double> transformacionCaracteristica(std::vector<std::vector<double>> &autovectores, std::vector<double> &imagenVectorizada);

private:

    std::vector<double> multiplicarMatrizVector(const std::vector<std::vector<double>> &matriz, const std::vector<double> &vec) const;

    std::vector<std::vector<double>> transponerMatriz(const std::vector<std::vector<double>> &matriz) const;

    std::vector<double> generarVectorAlAzar(int &dimension);

    double calcularNorma(std::vector<double> vector);

    //Calcula los autovalores de una matriz con el metodo de la potencia
    double calcularAutovalor(const std::vector<std::vector<double>> &matrizM, const std::vector<double> &autovector) const;

    //Indica si se hizo
    bool _hizoLaMAlterna;

    //Guardo la matriz Xt por si la multiplicación fue al revés
    std::vector< std::vector<double >> _matXt;

    //std::vector< std::vector<double >> _matM;
};

#endif //TRABAJOPRACTICO2METNUM_MODULOPCA_H
