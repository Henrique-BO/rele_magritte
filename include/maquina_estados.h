#ifndef MAQUINA_ESTADOS_H
#define MAQUINA_ESTADOS_H

#define NUM_ESTADOS 3
#define NUM_EVENTOS 7
#define FSM_DELAY 100

// ESTADOS
typedef enum Estado {
    IDLE,
    IMPRIMINDO,
    CALIBRANDO
} Estado;

// EVENTOS
typedef enum Evento {
    NENHUM_EVENTO,
    CALIBRAR, // Botão Calibrar
    IMPRIMIR, // Botão Imprimir
    CANCELAR, // Botão Cancelar desenho
    TERMINADO, // Desenho terminado
    ORIGEM, // Caneta na origem
    CARREGAR // Botão Carregar programa
} Evento;

// ACOES
typedef enum Acao {
    NENHUMA_ACAO,
    A01,
    A02,
    A03,
    A04
} Acao;

// struct que armazena os codigos de proximo estado e acao
typedef struct ProxEstadoAcao {
    Estado estado;
    Acao acao;
} ProxEstadoAcao;

void vTaskMaquinaEstados(void *param);

class MaquinaEstados {
    public:
        void iniciarMaquinaEstados();
        void taskExecutar();
        Estado getEstado();

    private:
        Evento obterEvento();
        ProxEstadoAcao obterProxEstadoAcao(Estado estado, Evento evento);
        void executarAcao(Acao acao);

        ProxEstadoAcao matrizTransicaoEstados[NUM_ESTADOS][NUM_EVENTOS]; // matriz de transicao de estados
        Estado estado; // estado atual
};

#endif