
#include <iostream>
#include <vector>
#include <bitset>
#include <random>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <cstdint>

using namespace std;


const int TAM_POBLACION = 10;
const int BITS_VARIABLE = 16;
const int BITS_INDIVIDUO = 32;
const int GENERACIONES = 10;

const double LIMITE_INFERIOR = -5.12;
const double LIMITE_SUPERIOR = 5.12;

const double PROB_CRUZAMIENTO = 0.90;
const double PROB_MUTACION = 0.10;

const int TAM_TORNEO = 3;


struct Individuo {
    bitset<BITS_INDIVIDUO> cromosoma;
    double x = 0.0;
    double y = 0.0;
    double fitness = 0.0;
};



mt19937 generador(random_device{}());

int aleatorio_entero(int minimo, int maximo) {
    uniform_int_distribution<int> distribucion(minimo, maximo);
    return distribucion(generador);
}

double aleatorio_real(double minimo, double maximo) {
    uniform_real_distribution<double> distribucion(minimo, maximo);
    return distribucion(generador);
}



uint16_t decodificar_gen(const bitset<BITS_INDIVIDUO>& cromosoma,
    int inicio) {

    uint16_t decimal = 0;

    for (int i = 0; i < BITS_VARIABLE; i++) {
        decimal <<= 1;
        decimal |= cromosoma[inicio + i];
    }

    return decimal;
}

double convertir_a_real(uint16_t decimal) {

    double valor = LIMITE_INFERIOR +
        (static_cast<double>(decimal) /
            (pow(2, BITS_VARIABLE) - 1)) *
        (LIMITE_SUPERIOR - LIMITE_INFERIOR);

    return valor;
}



double funcion_rastrigin(double x, double y) {

    return 20.0
        + x * x
        - 10.0 * cos(2.0 * M_PI * x)
        + y * y
        - 10.0 * cos(2.0 * M_PI * y);
}



void evaluar_individuo(Individuo& individuo) {

    uint16_t decimal_x = decodificar_gen(
        individuo.cromosoma, 0);

    uint16_t decimal_y = decodificar_gen(
        individuo.cromosoma, BITS_VARIABLE);

    individuo.x = convertir_a_real(decimal_x);
    individuo.y = convertir_a_real(decimal_y);

    individuo.fitness = funcion_rastrigin(
        individuo.x, individuo.y);
}

vector<Individuo> inicializar_poblacion() {

    vector<Individuo> poblacion(TAM_POBLACION);

    for (int i = 0; i < TAM_POBLACION; i++) {

        for (int bit = 0; bit < BITS_INDIVIDUO; bit++) {
            poblacion[i].cromosoma[bit] =
                aleatorio_entero(0, 1);
        }

        evaluar_individuo(poblacion[i]);
    }

    return poblacion;
}


Individuo seleccion_torneo(const vector<Individuo>& poblacion) {

    int mejor = aleatorio_entero(0, TAM_POBLACION - 1);

    for (int i = 1; i < TAM_TORNEO; i++) {

        int candidato = aleatorio_entero(
            0, TAM_POBLACION - 1);

        if (poblacion[candidato].fitness <
            poblacion[mejor].fitness) {

            mejor = candidato;
        }
    }

    return poblacion[mejor];
}



void cruzamiento(const Individuo& padre1,
    const Individuo& padre2,
    Individuo& hijo1,
    Individuo& hijo2) {

    hijo1 = padre1;
    hijo2 = padre2;

    if (aleatorio_real(0.0, 1.0) <
        PROB_CRUZAMIENTO) {

        int punto = aleatorio_entero(
            1, BITS_INDIVIDUO - 1);

        for (int i = punto; i < BITS_INDIVIDUO; i++) {

            hijo1.cromosoma[i] = padre2.cromosoma[i];
            hijo2.cromosoma[i] = padre1.cromosoma[i];
        }
    }
}



void mutacion(Individuo& individuo) {

    for (int i = 0; i < BITS_INDIVIDUO; i++) {

        if (aleatorio_real(0.0, 1.0) <
            PROB_MUTACION) {

            individuo.cromosoma.flip(i);
        }
    }
}



void ordenar_poblacion(vector<Individuo>& poblacion) {

    sort(poblacion.begin(), poblacion.end(),
        [](const Individuo& a, const Individuo& b) {
            return a.fitness < b.fitness;
        });
}



void mostrar_individuo(const Individuo& individuo) {

    cout << "Cromosoma: "
        << individuo.cromosoma << '\n';

    cout << "x: " << individuo.x << '\n';
    cout << "y: " << individuo.y << '\n';
    cout << "Fitness: " << individuo.fitness << '\n';
}



int main() {

    cout << fixed << setprecision(8);


    cout << " ALGORITMO GENETICO BINARIO\n";
    cout << " FUNCION DE RASTRIGIN\n";


    cout << "Poblacion: " << TAM_POBLACION << '\n';
    cout << "Bits por individuo: " << BITS_INDIVIDUO << '\n';
    cout << "Generaciones: " << GENERACIONES << '\n';
    cout << "Probabilidad de cruzamiento: "
        << PROB_CRUZAMIENTO << '\n';

    cout << "Probabilidad de mutacion: "
        << PROB_MUTACION << '\n';

 

    vector<Individuo> poblacion =
        inicializar_poblacion();

    ordenar_poblacion(poblacion);

    

    Individuo mejor_historico = poblacion[0];



    for (int gen = 0; gen < GENERACIONES; gen++) {

        ordenar_poblacion(poblacion);

  

        if (poblacion[0].fitness <
            mejor_historico.fitness) {

            mejor_historico = poblacion[0];
        }

        double suma_fitness = 0.0;

        for (const Individuo& individuo : poblacion) {
            suma_fitness += individuo.fitness;
        }

        double promedio = suma_fitness / TAM_POBLACION;

        cout << "GENERACION " << gen + 1 << '\n';


        cout << "Mejor fitness: "
            << poblacion[0].fitness << '\n';

        cout << "Promedio fitness: "
            << promedio << '\n';

        cout << "Mejor x: "
            << poblacion[0].x << '\n';

        cout << "Mejor y: "
            << poblacion[0].y << '\n';



        vector<Individuo> nueva_poblacion;

   
        nueva_poblacion.push_back(poblacion[0]);



        while (nueva_poblacion.size() <
            TAM_POBLACION) {

  

            Individuo padre1 =
                seleccion_torneo(poblacion);

            Individuo padre2 =
                seleccion_torneo(poblacion);

            Individuo hijo1, hijo2;

     

            cruzamiento(padre1, padre2,
                hijo1, hijo2);

   

            mutacion(hijo1);
            mutacion(hijo2);



            evaluar_individuo(hijo1);
            evaluar_individuo(hijo2);



            nueva_poblacion.push_back(hijo1);

            if (nueva_poblacion.size() <
                TAM_POBLACION) {

                nueva_poblacion.push_back(hijo2);
            }
        }


        poblacion = nueva_poblacion;

        ordenar_poblacion(poblacion);


        if (poblacion[0].fitness <
            mejor_historico.fitness) {

            mejor_historico = poblacion[0];
        }
    }

    cout << " RESULTADO FINAL\n";


    mostrar_individuo(mejor_historico);

    cout << " ALGORITMO TERMINADO\n";


    return 0;
}
