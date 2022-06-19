/**
 * @file maquina_estados.h
 * @brief Definição do componente Máquina de Estados
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef MAQUINA_ESTADOS_H
#define MAQUINA_ESTADOS_H

#include "controlador.h"
#include "interpretador_g.h"

#define NUM_ESTADOS 3
#define NUM_EVENTOS 7
#define QUEUE_EVENTOS_SIZE 5
#define FSM_DELAY 100

/**
 * @brief Enumerador dos estados da Máquina de Estados
 * 
 */
typedef enum Estado {
    IDLE,
    IMPRIMINDO,
    CALIBRANDO
} Estado;

/**
 * @brief Enumerados dos eventos da Máquina de Estados
 * 
 */
typedef enum Evento {
    NENHUM_EVENTO,
    CALIBRAR, // Botão Calibrar
    IMPRIMIR, // Botão Imprimir
    CANCELAR, // Botão Cancelar desenho
    TERMINADO, // Desenho terminado
    ORIGEM, // Caneta na origem
    CARREGAR // Botão Carregar programa
} Evento;

/**
 * @brief Enumerador das ações da Máquina de Estados
 * 
 */
typedef enum Acao {
    NENHUMA_ACAO,
    A01,
    A02,
    A03,
    A04,
    A05
} Acao;

/**
 * @brief Struct que armazena o próximo estado e a ação a ser realizada
 * 
 */
typedef struct ProxEstadoAcao {
    Estado estado;
    Acao acao;
} ProxEstadoAcao;

/**
 * @brief Classe do componente Máquina de Estados
 * 
 */
class MaquinaEstados {
    public:
        MaquinaEstados(Controlador& controlador, InterpretadorG& interpretadorG): 
            controlador(controlador), interpretadorG(interpretadorG) {} 

        /**
         * @brief Inicia a Máquina de Estados, criando a matriz de transição de estados e a task do FreeRTOS
         * 
         */
        void iniciarMaquinaEstados();

        /**
         * @brief Task do FreeRTOS responsável pelo ciclo de busca e execução da Máquina de Estados
         * 
         */
        void taskExecutar();

        /**
         * @brief Obtem o estado atual
         * 
         * @return Estado 
         */
        Estado getEstado();

    private:
        /**
         * @brief Verifica se algum evento ocorreu
         * 
         * @return O evento que ocorreu, ou NENHUM_EVENTO caso contrário
         */
        Evento obterEvento();
        ProxEstadoAcao obterProxEstadoAcao(Estado estado, Evento evento);
        void executarAcao(Acao acao);

        ProxEstadoAcao matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS]; // matriz de transicao de estados
        Estado estado; // estado atual
        Controlador controlador; // TODO precisa de Controlador& ?? 
        InterpretadorG interpretadorG;
};

/**
 * @brief Wrapper da função MaquinaEstados::taskExecutar para criação de task FreeRTOS
 * 
 * @param param Ponteiro para objeto MaquinaEstados
 */
void vTaskMaquinaEstados(void *param);

/**
 * @brief Fila de eventos
 * 
 */
extern QueueHandle_t xQueueEventos;

/**
 * @brief Máquina de Estados como uma variável global
 * 
 */
extern MaquinaEstados maquinaEstados;

#endif