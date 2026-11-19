
/*
COMPILAR:
g++ -std=c++17 -O2 descomprimir.cpp -lz -o descomprimir

EJECUTAR:
./descomprimir archivo.gz archivo_recuperado.txt
*/

#include <iostream>
#include <fstream>
#include <cstdint>
#include <array>
#include <cstring>
#include <zlib.h>

using namespace std;

uint32_t leer32(const unsigned char* datos) {
    return (uint32_t)datos[0] |
        ((uint32_t)datos[1] << 8) |
        ((uint32_t)datos[2] << 16) |
        ((uint32_t)datos[3] << 24);
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cout << "Uso: ./descomprimir archivo.gz archivo_salida\n";
        return 1;
    }

    ifstream entrada(argv[1], ios::binary);
    ofstream salida(argv[2], ios::binary);

    if (!entrada || !salida) {
        cout << "Error al abrir los archivos\n";
        return 1;
    }

    // Leer cabecera GZIP

    unsigned char cabecera[10];

    entrada.read((char*)cabecera, 10);

    if (entrada.gcount() != 10) {
        cout << "Cabecera incompleta\n";
        return 1;
    }

    if (cabecera[0] != 0x1F || cabecera[1] != 0x8B ||
        cabecera[2] != 8 || cabecera[3] != 0) {
        cout << "Formato GZIP no valido\n";
        return 1;
    }

    // Inicializar descompresion

    z_stream strm = {};

    if (inflateInit2(&strm, -MAX_WBITS) != Z_OK) {
        cout << "Error al iniciar la descompresion\n";
        return 1;
    }

    array<unsigned char, 4096> datosEntrada;
    array<unsigned char, 4096> datosSalida;

    uint32_t crc = crc32(0L, Z_NULL, 0);
    uint32_t tamano = 0;

    int resultado = Z_OK;

    while (resultado != Z_STREAM_END) {

        if (strm.avail_in == 0) {

            entrada.read((char*)datosEntrada.data(), datosEntrada.size());

            streamsize leidos = entrada.gcount();

            if (leidos <= 0) {
                cout << "Archivo GZIP incompleto\n";
                inflateEnd(&strm);
                return 1;
            }

            strm.next_in = datosEntrada.data();
            strm.avail_in = (uInt)leidos;
        }

        strm.next_out = datosSalida.data();
        strm.avail_out = (uInt)datosSalida.size();

        resultado = inflate(&strm, Z_NO_FLUSH);

        if (resultado != Z_OK && resultado != Z_STREAM_END) {
            cout << "Error al descomprimir\n";
            inflateEnd(&strm);
            return 1;
        }

        size_t escritos = datosSalida.size() - strm.avail_out;

        if (escritos > 0) {

            crc = crc32(crc, datosSalida.data(), (uInt)escritos);
            tamano += (uint32_t)escritos;

            salida.write((char*)datosSalida.data(), escritos);

            if (!salida) {
                cout << "Error al escribir el archivo\n";
                inflateEnd(&strm);
                return 1;
            }
        }
    }

    // Recuperar los bytes que quedaron despues de DEFLATE

    unsigned char trailer[8];

    size_t disponibles = strm.avail_in;

    if (disponibles > 8) {
        cout << "Datos adicionales no compatibles\n";
        inflateEnd(&strm);
        return 1;
    }

    size_t obtenidos = disponibles;

    if (disponibles > 0) {
        memcpy(trailer, strm.next_in, disponibles);
    }

    inflateEnd(&strm);

    if (obtenidos < 8) {

        entrada.read((char*)trailer + obtenidos, 8 - obtenidos);

        if (entrada.gcount() != (streamsize)(8 - obtenidos)) {
            cout << "Trailer incompleto\n";
            return 1;
        }
    }

    // Leer CRC32 y tamano original

    uint32_t crcArchivo = leer32(trailer);
    uint32_t tamanoArchivo = leer32(trailer + 4);

    if (crc != crcArchivo) {
        cout << "Error: CRC32 incorrecto\n";
        return 1;
    }

    if (tamano != tamanoArchivo) {
        cout << "Error: tamano incorrecto\n";
        return 1;
    }

    entrada.close();
    salida.close();

    cout << "Archivo descomprimido correctamente\n";
    cout << "CRC32 verificado: " << hex << crc << '\n';
    cout << "Tamano recuperado: " << dec << tamano << " bytes\n";

    return 0;
}
