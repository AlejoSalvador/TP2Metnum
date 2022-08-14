//
// Created by Gonzalo on 17/05/2018.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "cargadorDeImagenes.h"

//REVISAR ESTAS FUNCIONES
//Copiadas del ppmloader
double obtenerPixel(uchar* datos, int i, int j, int alto, int ancho){
    if(i > alto)
        throw std::runtime_error("LA COORDENADA i (la primera) SUPERA LA ALTURA VALIDA");

    if(j > ancho)
        throw std::runtime_error("LA COORDENADA j (la segunda) SUPERA EL ANCHO VALIDA");

//    unsigned int red = (unsigned int)(datos[i*ancho*3 + j*3 + 0]);
//    unsigned int green = (unsigned int)(datos[i*ancho*3 + j*3 + 1]);
//    unsigned int blue = (unsigned int)(datos[i*ancho*3 + j*3 + 2]);
//
//    //std::cout << "COLORES EN RGB: (" << red << ", " << green << ", " << blue << ")" << std::endl;
//
//    return (unsigned int)((red+green+blue) / 3);

//    uchar pixel = (datos[i*ancho + j]);

    double pixel = (double) (datos[i*ancho + j]);

    return pixel;
}

void leerImagen(std::string filename, uchar** datos, int* ancho, int* alto){
    *datos = NULL;
    *ancho = 0;
    *alto = 0;
    PPM_LOADER_PIXEL_TYPE pt = PPM_LOADER_PIXEL_TYPE_INVALID;

    //std::cout << "ARCHIVO A CARGAR: " << filename.c_str() << std::endl;

    bool ret = LoadPPMFile(datos, ancho, alto, &pt, filename.c_str());

    if (!ret || ancho == 0|| alto == 0|| pt == PPM_LOADER_PIXEL_TYPE_INVALID){
        //Predicado anterior: (!ret || ancho == 0|| alto == 0|| pt!=PPM_LOADER_PIXEL_TYPE_RGB_8B)
        throw std::runtime_error("Fallo al leer la imagen.");
    }
}

cargadorDeImagenes::cargadorDeImagenes(const char *archivo) {
    std::ifstream listaDeNombres(archivo);

    if(listaDeNombres.eof()){
        std::cout << "EL ARCHIVO ESTA VACIO O NO EXISTE" << std::endl;
        return;
    }

    std::string nombreArchivoDeImagen;
    int IDPersona;

    //HAY QUE VER SI ESTO FUNCA

    //std::cout << "CONSTRUCTOR ANTES DE CICLO" << std::endl;

    std::stringstream conversor;

    std::string delimiter = ",";

    for(std::string linea; std::getline(listaDeNombres, linea); ){
        //std::cout << linea << std::endl;

        //Vacio el conversor
        conversor.clear();
        conversor.str("");

        //Separamos la linea y copio la direccion del archivo
        conversor << linea.substr(0, linea.find(delimiter));
        conversor >> nombreArchivoDeImagen;

        //Borro el nombre del archivo de la line actual
        linea.erase(0, linea.find(delimiter) + delimiter.length());

        //Vacio el conversor
        conversor.clear();
        conversor.str("");

        //Pongo la ID de la persona en el conversor
        conversor << linea.substr(0, linea.find(delimiter));

        //Convierto el ID a INT
        conversor >> IDPersona;

        //std::cout << "ARCHIVO: " << nombreArchivoDeImagen << std::endl;
        //std::cout << "ID DE PERSONA: " << IDPersona << std::endl;

        //Cargo la Imagen
        cargarImagen(nombreArchivoDeImagen, IDPersona);
        _rutasImagenes.push_back(nombreArchivoDeImagen);
    }

    listaDeNombres.close();
}

void cargadorDeImagenes::cargarImagen(std::string rutaArchivo, int IDPersona) {
    //std::cout << "CARGANDO IMAGEN: " << rutaArchivo << "; ID: " << IDPersona  << std::endl;

    //UNA CUENTA SE DEFASA Y CARGA MAL LAS COSAS, POR EJEMPLO SI CARGAS EL MISMO ARCHIVO DOS VECES (PARA VER
    // SI EL VECTOR ES EL MISMO) ENTOCES EN LA POSICION 3437 HAY US DEFASAJE Y A PARTIR DE AHI SE ANULA EL VECTOR   
    
    //Uso el ppmloader para cargar el archivo dado
    uchar *datos = NULL;
    int ancho = 0, alto = 0;

    //rutaArchivo = "../" + rutaArchivo;

    leerImagen(rutaArchivo, &datos, &ancho, &alto);

    //std::cout << "ANCHO DE LA IMAGEN: " << ancho << std::endl;
    //std::cout << "ALTO DE LA IMAGEN: " << alto << std::endl;

    std::vector<double> vectorPixeles(alto*ancho, 0);

//    int numPixel = 0;
//    //Obtengo la info de cada pixel
//    for(int h = 0; h < alto; ++h){
//        for(int w = 0; w < ancho; ++w){
//
//            vectorPixeles.push_back(obtenerMediaDePixel(datos, h, w, alto, ancho));
//            numPixel++;
//        }
//    }

    for(int p = 0; p < alto*ancho; p++) {
        //vectorPixeles[p] = datos[p];
        vectorPixeles[p] = (double) datos[p];
    }

    //std::cout << "TAMAÑO DEL VECTOR DE PIXELES: " << vectorPixeles.size() << std::endl;

//    std::cout << "CONTENIDO DE PIXELES DEL VECTOR: " << vectorPixeles.size() << std::endl;
//
//    for(int i = 0; i < vectorPixeles.size(); i++){
//        std::cout << "PIXEL: " << i << ", VALOR: " << vectorPixeles[i] << std::endl;
//    }

//    std::cout << "MUESTRO EL VECTOR PARA LA PRUEBA CHICA: " << std::endl;
//    std::cout << "[";
//
//    for(int i = 0; i < vectorPixeles.size(); i++){
//        std::cout << "(" << vectorPixeles[i] << ", PIXEL: " << i << ")" << ((i + 1 == vectorPixeles.size())? (""): (", "));
//    }
//
//    std::cout << "]" << std::endl;

    //Ahora el vector esta cargado con los pixeles de la imagen (en escala de grises)
    //entonces lo pongo en el vector de imagenes
    _imagenes.push_back(std::make_pair(vectorPixeles, IDPersona));

    //Borro lo contenido en datos
    delete [] datos;

}

std::vector< std::pair<std::vector<double>, int> > cargadorDeImagenes::conjuntoDeImagenes() {
    return _imagenes;
}

std::vector<int> cargadorDeImagenes::clases() {
    std::vector<int> _clases(_imagenes.size());

    //Consiguo el arreglo de clases
    for(int i = 0; i < _clases.size(); i++){
        _clases[i] = _imagenes[i].second;
    }

    return _clases;
}

std::vector<std::vector<double>> cargadorDeImagenes::vectoresDeImagenes() {
    std::vector<std::vector<double>> _vectores(_imagenes.size());

    //Consiguo el arreglo de clases
    for(int i = 0; i < _vectores.size(); i++){
        _vectores[i] = _imagenes[i].first;
    }

    return _vectores;
}

std::vector<std::string> cargadorDeImagenes::rutas() {
    return _rutasImagenes;
}