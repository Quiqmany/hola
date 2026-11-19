
/*
COMPILAR:
g++ -std=c++17 -O2 comprimir.cpp -lz -o comprimir

EJECUTAR:
./comprimir archivo.txt archivo.gz
*/

#include <iostream>
#include <fstream>
#include <cstdint>
#include <array>
#include <zlib.h>

using namespace std;

void escribir32(ofstream& archivo, uint32_t valor) {
    for (int i = 0; i < 4; i++) {
        archivo.put((valor >> (8 * i)) & 0xFF);
    }
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cout << "Uso: ./comprimir archivo_original archivo.gz\n";
        return 1;
    }

    ifstream entrada(argv[1], ios::binary);
    ofstream salida(argv[2], ios::binary);

    if (!entrada || !salida) {
        cout << "Error al abrir los archivos\n";
        return 1;
    }

    // Cabecera GZIP

    salida.put(0x1F);
    salida.put(0x8B);
    salida.put(8);
    salida.put(0);

    escribir32(salida, 0);

    salida.put(0);
    salida.put(255);

    // Inicializar compresion DEFLATE

    z_stream strm = {};

    if (deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
        -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        cout << "Error al iniciar la compresion\n";
        return 1;
    }

    array<unsigned char, 4096> datosEntrada;
    array<unsigned char, 4096> datosSalida;

    uint32_t crc = crc32(0L, Z_NULL, 0);
    uint32_t tamano = 0;

    int resultado;
    int modo;

    do {
        entrada.read((char*)datosEntrada.data(), datosEntrada.size());

        streamsize leidos = entrada.gcount();

        if (leidos > 0) {
            crc = crc32(crc, datosEntrada.data(), (uInt)leidos);
            tamano += (uint32_t)leidos;
        }

        modo = entrada.eof() ? Z_FINISH : Z_NO_FLUSH;

        strm.next_in = datosEntrada.data();
        strm.avail_in = (uInt)leidos;

        do {
            strm.next_out = datosSalida.data();
            strm.avail_out = (uInt)datosSalida.size();

            resultado = deflate(&strm, modo);

            if (resultado != Z_OK && resultado != Z_STREAM_END) {
                cout << "Error durante la compresion\n";
                deflateEnd(&strm);
                return 1;
            }

            size_t escritos = datosSalida.size() - strm.avail_out;

            salida.write((char*)datosSalida.data(), escritos);

            if (!salida) {
                cout << "Error al escribir el archivo\n";
                deflateEnd(&strm);
                return 1;
            }

        } while (strm.avail_out == 0);

    } while (modo != Z_FINISH);

    deflateEnd(&strm);

    // Trailer GZIP

    escribir32(salida, crc);
    escribir32(salida, tamano);

    entrada.close();
    salida.close();

    cout << "Archivo comprimido correctamente\n";
    cout << "CRC32: " << hex << crc << '\n';
    cout << "Tamano original: " << dec << tamano << " bytes\n";

    return 0;
}
