#include <iostream>
#include <vector>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <iomanip>
#include <cmath>
#include <iostream>
#include <math.h>

/***
    AUTHOR: Marlon da Silva Rogério
    DATA: 25 ABR 2021
    DISCIPLINA: Tópicos Especiais em Inteligência Computacional https://github.com/msrogerio/pso_rastring/blob/main/pso.cpp
    DESCRICAO: Optimização por enxame de partículas | Rastrigin | Minimização
***/

using namespace std;


double MAX_DOUBLE = DBL_MAX;
long geracoes = 20; //numero de geracoes
const int tam_pop = 100; //tamanho da populacao/swarm
const int dimensoes_var = 10; //numero de variaveis de decisao
const int dimensoes_obj = 1; //numero de objetivos
double limiteInferior = -5.12; //limite inferior das variaveis de decisao do problema (funcao Rastrigin)
double limiteSuperior = 5.12; //limite superior das variaveis de decisao do problema (funcao Rastrigin)

double alfa = 0.1; //inercia
double beta = 2.5; //influencia do melhor pessoal - componente cognitivo
double gama = 0; //influencia do melhor local -- nao vamos usar
double delta = 2.5; //influencia do melhor global
double epsilon = 1; //aumento ou diminuicao da velocidade da particula

struct Individuo {
    double x[dimensoes_var]; //vetor de variaveis de decisao
    double fx[dimensoes_obj]; //vetor de objetivos
};

struct Particula {
        Individuo solucao; //solucao atual da particula
        double velocidade[dimensoes_var]; //vetor de velocidade da particula
        Individuo melhorPessoal; //melhor solucao encontrada ate agora por essa particula -- nao vamos usar, por isso gama e 0
        Individuo melhorLocal; //melhor solucao encontrada ate agora pelos vizinhos dessa particula
};

Particula populacao[tam_pop]; //Populacao ou enxame
Particula melhor; //vai conter a melhor solucao obtida ate agora / lider global

//prototipacao das funcoes
void inicializacao(); //inicializa todas as solucoes aleatoriamente
void aptidao(Individuo *ind); //calcula o fitness ou valor objetivo
void atualizarMelhorPessoal(Particula *part); //atualiza a melhor solucao ja encontrada pela propria particula
void calcularVelocidade(Particula *part); //calcula a nova velocidade
void atualizarPosicao(Particula *part); //calcula a nova posicao

int main(const int argc, const char* argv[]){
    srand (time(NULL)); //inicializa a semente aleatória com o tempo atual

    inicializacao();
    populacao[0].solucao.fx[0]=MAX_DOUBLE;
    memcpy(&melhor, &populacao[0], sizeof(Particula));//inicialia a melhor solucao com uma solucao qualquer
    
    for(long g=0;g<geracoes;g++){//laco principal
        cout << "\n";
        cout << "################### [ GERACAO " << g << "] ###################";
        cout << endl;
        for(int i=0;i<tam_pop;i++){ //laco para o calculo do fitness e atualizacao da melhor solucao
            aptidao(&populacao[i].solucao);
            if(populacao[i].solucao.fx[0] < melhor.solucao.fx[0]){
                memcpy(&melhor, &populacao[i], sizeof(Particula)); 
                // cout << endl; 
                // cout << "[ 1.1 POP_SOLUCAO EH MELHOR ] -> " << populacao[i].solucao.fx[0];
                // cout << endl << ">>>"; 
                // for(int j=0;j<dimensoes_var;j++){
                //         cout << melhor.solucao.x[j]; 
                // }
                // cout << endl;
            // }else{
            //     cout << endl;
            //     cout << "[ 1.2 MELSOLUCAO EH MELHOR ] -> " << melhor.solucao.fx[0];
            }
        }
        
        for(int i=0;i<tam_pop;i++){ //laco para a atualizacao da populacao
            atualizarMelhorPessoal(&populacao[i]);
            // cout << endl;
            // cout << " --- **** solucao < melhorpessoal [ " << populacao[i].melhorPessoal.fx[0] << " ]";

            calcularVelocidade(&populacao[i]);
            // cout << endl;
            // cout << " ---- **** VELOCIDADE [";
            // for (int j=0; j<dimensoes_var; j++){
            //     cout << " " << populacao[i].velocidade[j];
            // }
            // cout << " ]";
            
            atualizarPosicao(&populacao[i]);     
            // cout << endl;
            // cout << " ---- **** POSICAO [";
            // for (int j=0; j<dimensoes_var; j++){
            //     cout << " " << populacao[i].solucao.x[j];
            // }
            // cout << " ]";
        }
        cout << endl;
        cout << "Melhor ate agora ->>>>> " << melhor.solucao.fx[0] << " ...";
        cout << endl;
        cout << "[";
        for(int j=0;j<dimensoes_var;j++){
            cout << " " << melhor.solucao.x[j];
        }
        cout << " ]";
        if (melhor.solucao.fx[0] == 0){
            break;
        }
    }
    cout << endl;
    cout << "[ END ] <<< SOLUCAO FINAL >>>: " << melhor.solucao.fx[0];
    cout << endl;
    cout << "[";
    for(int j=0;j<dimensoes_var;j++){
        cout << " " << melhor.solucao.x[j];
    }
    cout << " ]";
}   

void inicializacao(){
    double extensao = limiteSuperior - limiteInferior; // qual o tamanho total
    for(int i=0;i<tam_pop;i++){
        populacao[i].melhorPessoal.fx[0]=MAX_DOUBLE;
        populacao[i].melhorLocal.fx[0]=MAX_DOUBLE;
        for(int j=0;j<dimensoes_var;j++){
            populacao[i].solucao.x[j]=limiteInferior+(rand()/(double)RAND_MAX)*extensao; //limite inferior + aleatorio na extensao
            populacao[i].velocidade[j]=limiteInferior+(rand()/(double)RAND_MAX)*extensao; //limite inferior + aleatorio na extensao
        }
    }
}

void aptidao(Individuo *ind){ //funcao Rastrigin
    int A=10;
    double fx=0;
    for(int j=0;j<dimensoes_var;j++){
        fx+=ind->x[j]*ind->x[j]-A*cos(2*M_PI*ind->x[j]);
    }
    fx+=A*dimensoes_var;
    ind->fx[0]=fx;
}


void atualizarMelhorPessoal(Particula *part){ 
    // cout << endl;
    if (part->solucao.fx[0] < part->melhorPessoal.fx[0]){
        part->melhorPessoal = part->solucao;
    }
}

void calcularVelocidade(Particula *part){
    double newVelocidade=0;
    // cout << endl;
    // cout << " ---- func VELOCIDADE [";
    for (int j=0; j<dimensoes_var; j++){
        double b = (rand()/(double)RAND_MAX)*beta;
        double c = (rand()/(double)RAND_MAX)*gama;
        double d = (rand()/(double)RAND_MAX)*delta;
        newVelocidade = alfa * part->velocidade[j]  + b * (part->melhorPessoal.x[j]-part->solucao.x[j]);
        newVelocidade = part->velocidade[j] + c * (part->melhorPessoal.x[j] - part->solucao.x[j]); 
        newVelocidade = part->velocidade[j] + d * (melhor.solucao.x[j] - part->solucao.x[j]); 
        part->velocidade[j] = newVelocidade;
        // cout << " " << part->velocidade[j];
    }
    // cout << " ]";
}

void atualizarPosicao(Particula *part){
    double newPosicao=0;
    // cout << endl;
    // cout << " ---- func POSICAO [";
    for (int j=0; j<dimensoes_var; j++){
        newPosicao = (part->solucao.x[j] + part->velocidade[j]) * epsilon;
        if (newPosicao <= limiteSuperior && newPosicao >= limiteInferior){
            part->solucao.x[j] = newPosicao;
        }else if (newPosicao < limiteInferior) {
            part->solucao.x[j] = limiteInferior;
        }else if (newPosicao > limiteSuperior){
            part->solucao.x[j] = limiteSuperior;
        }
        // cout << " " << part->solucao.x[j];
    }
    // cout << " ]";
}