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
    DISCIPLINA: Tópicos Especiais em Inteligência Computacional 
    DESCRICAO: Optimização por enxame de partículas | Rastrigin | Minização
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
        cout << "#############################################";
        cout << "\n";
        cout << "Geracao " << g;
        cout << endl;
        for(int i=0;i<tam_pop;i++){ //laco para o calculo do fitness e atualizacao da melhor solucao
            aptidao(&populacao[i].solucao);
            if(populacao[i].solucao.fx[0] < melhor.solucao.fx[0]){
                memcpy(&melhor, &populacao[i], sizeof(Particula)); 
                cout << endl; 
                cout << "[ 1.1 POP_SOLUCAO EH MELHOR ] -> " << populacao[i].solucao.fx[0];
                cout << endl << ">>>"; 
                for(int j=0;j<dimensoes_var;j++){
                        cout << melhor.solucao.x[j]; 
                }
                cout << endl;
            }else{
                cout << endl;
                cout << "[ 1.2 MELSOLUCAO EH MELHOR ] -> " << melhor.solucao.fx[0];
            }
        }
        
        for(int i=0;i<tam_pop;i++){ //laco para a atualizacao da populacao
            atualizarMelhorPessoal(&populacao[i]);
            calcularVelocidade(&populacao[i]);
            atualizarPosicao(&populacao[i]);
        }
        cout << endl;
        cout << "Melhor ate agora ->>>>> " << melhor.solucao.fx[0] << " ...";
        if (melhor.solucao.fx[0] == 0){
            break;
        }
    }
    cout << endl;
    cout << "[ END ] <<< SOLUCAO FINAL >>>: " << melhor.solucao.fx[0];
    cout << endl;
    for(int j=0;j<dimensoes_var;j++){
        cout << melhor.solucao.x[j];
    }
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
    cout << fx << " ";
}


void atualizarMelhorPessoal(Particula *part){   
    cout << endl;
    if (double(part->solucao.fx[0]) > double(part->melhorPessoal.fx[0])){
        cout << "[ 2.1 -  ATUALIZA MELHOR PESSOAL ] " << part->solucao.fx[0] << "   >   " << part->melhorPessoal.fx[0];
        part->solucao.fx[0] = part->melhorPessoal.fx[0];
        for (int j=0; j<dimensoes_var; j++){
            part->solucao.x[j] = part->melhorPessoal.x[j];
        }
    }else{
        cout << "[ 2.2 -  ATUALIZA MELHOR PESSOAL ] " << part->solucao.fx[0] << "   <   " << part->melhorPessoal.fx[0];
        part->melhorPessoal.fx[0] = part->solucao.fx[0];
        for (int j=0; j<dimensoes_var; j++){
            part->melhorPessoal.x[j] = part->solucao.x[j];
        }
    }
}

void calcularVelocidade(Particula *part){
    // nV = oldV + b * (pBest - pAtual) + c * (lBest - pAtual) + d * (gBest - pAtual);
    double extensao = limiteSuperior - limiteInferior;
    double newVelocidade;
    double temp;
    for (int j=0; j<dimensoes_var; j++){
        newVelocidade = part->velocidade[j] * alfa + beta * (part->solucao.x[j] - part->melhorPessoal.x[j]);
        newVelocidade = part->velocidade[j] + gama * (part->melhorLocal.x[j] - part->melhorPessoal.x[j]); 
        newVelocidade = part->velocidade[j] + delta * (melhor.solucao.x[j] - part->melhorPessoal.x[j]);
        // Se estiver no intervalo de limiteSuperior e limiteInferior 
        if (newVelocidade <= limiteSuperior && newVelocidade >= limiteInferior){
            part->velocidade[j] = newVelocidade;
            cout << endl;            
            cout << "[ 3.1 -  CAL. VEL.  (DENTRO DOS LIMITE)] DE ->   " << newVelocidade;
        }else if (newVelocidade < limiteInferior) {
            cout << endl;            
            cout << "[ 3.2 -  CAL. VEL.  (FORA DOS LIMITE)] DE ->  " << newVelocidade;
            part->velocidade[j] = limiteInferior-(rand()/(double)RAND_MAX)+0.01;
            cout << " PARA =>  " << part->velocidade[j];
        }else if (newVelocidade > limiteSuperior){
            cout << endl;            
            cout << "[ 3.3 -  CAL. VEL.  (FORA DOS LIMITE)] DE ->   " << newVelocidade;
            part->velocidade[j] = limiteSuperior+(rand()/(double)RAND_MAX)-0.01;
            cout << " PARA =>  " << part->velocidade[j];
        
        }
    }
}

void atualizarPosicao(Particula *part){
    // posAtual = (posAnterior + volocidade) * epsilon
    double extensao = limiteSuperior - limiteInferior; // qual o tamanho total
    double newPosicao;
    for (int j=0; j<dimensoes_var; j++){
        newPosicao = (part->solucao.x[j] + part->velocidade[j]) * epsilon;
        //  Se estiver no intervalo de limiteSuperior e limiteInferior
        if (newPosicao <= limiteSuperior && newPosicao >= limiteInferior){
            part->solucao.x[j] = newPosicao;
            cout << endl;            
            cout << "[ 4.1 -  ATU. POSI. (DENTRO DOS LIMITE)] DE ->  " << newPosicao;
        // Se for menor que o limite inferior
        }else if (newPosicao < limiteInferior) {
            cout << endl;            
            cout << "[ 4.2 -  ATU. POSI. (FORA DOS LIMITE)] DE ->  " << newPosicao;
            part->solucao.x[j] = limiteInferior-(rand()/(double)RAND_MAX)+0.01;
            cout << " PARA =>  " << part->solucao.x[j];
        // Se for maior que o limiteSuperior
        }else if (newPosicao > limiteSuperior){
            cout << endl;
            cout << "[ 4.3 -  ATU. POSI. (FORA DOS LIMITE)] DE ->  " << newPosicao;
            part->solucao.x[j] = limiteSuperior+(rand()/(double)RAND_MAX)-0.01;
            cout << " PARA =>  " << part->solucao.x[j];    
        }
    }
    aptidao(&part->solucao);
    if(part->solucao.fx[0] < melhor.solucao.fx[0]){
        memcpy(&melhor, &part, sizeof(Particula)); 
        cout << endl;
        cout << "[ 4.4.1 POP_SOLUCAO EH MELHOR ] -> " << part->solucao.fx[0];
        cout << endl << ">>>"; 
        for(int j=0;j<dimensoes_var;j++){
                cout << melhor.solucao.x[j]; 
        }
        cout << endl;
    }
}