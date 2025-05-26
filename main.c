#include <utils/utils.h>

int main(int argc, char* argv[]) {

    log = log_create("cpu.log", "CPU", true, LOG_LEVEL_DEBUG);

    conectar_componete("127.0.0.1", "8000", "Hola al kernel desde la cpu");
    
    iniciar_conecciones("8001");
    
    conectar_componete("127.0.0.1", "8002", "Hola al memoria desde la cpu");
    
    conectar_componete("127.0.0.1", "8003", "Hola al io desde la cpu");

    return 0;
}
