
#ifndef TRABAJOPRACTICO2METNUM_CARGADORDEIMAGENES_H
#define TRABAJOPRACTICO2METNUM_CARGADORDEIMAGENES_H

#include <vector>
#include <stdio.h>
#include "lib/ppmloader.h"

typedef unsigned char uchar;

//REVISAR ESTAS FUNCIONES
//Copiadas del ppmloader
//unsigned char obtenerPixel(uchar* datos, int i, int j, int alto, int ancho);

double obtenerPixel(uchar* datos, int i, int j, int alto, int ancho);

void leerImagen(std::string filename, uchar** datos, int* ancho, int* alto);

class cargadorDeImagenes{
    public:
        //Toma el archivo con la lista de las imagenes de la base
        cargadorDeImagenes(const char *archivo);

        //Devuelve el conjunto de las imagenes de la base con el siguiente formato:
        //  <(vector que representa la imagen),(ID de la persona)>
        //std::vector<std::pair<std::vector<unsigned char>, int>> conjuntoDeImagenes();
        std::vector<std::pair<std::vector<double>, int>> conjuntoDeImagenes();
        std::vector<std::vector<double>> vectoresDeImagenes();
        std::vector<int> clases();
        std::vector<std::string> rutas();

    private:
        //Carga la imagen pasada en el vector
        void cargarImagen(std::string rutaArchivo, int IDPersona);

        //Vector con todas las imagenes
        //std::vector< std::pair<std::vector<unsigned char>, int> > _imagenes;
        std::vector< std::pair<std::vector<double>, int> > _imagenes;
        std::vector<std::string> _rutasImagenes;
};

#endif //TRABAJOPRACTICO2METNUM_CARGADORDEIMAGENES_H
