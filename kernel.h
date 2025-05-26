#ifndef KERNEL_H
#define KERNEL_H

#include <utils/utils.h>
#include <proceso.h> // Include proceso.h to use Proceso struct and list functions
#include <commons/collections/list.h> // Ensure t_list is available

const char *puertoServidor = "8000";
// log = log_create("kernel.log", "Kernel", true, LOG_LEVEL_DEBUG); // This should be initialized in a C file, not a header

enum Estado { //
    CONECTADO, //
    DESCONECTADO, //
    OCUPADO //
}; //

enum Componentes{ //
    KERNEL, //
    MEMORIA, //
    CPU, //
    IO //
}; //

struct CPU { //
    int id; //
    char *ip; //
    char *puerto; //
    enum Estado estado; //
}; //

struct nodoCpu { //
    struct CPU cpu; //
    struct nodoCpu *sgte; //
}; //

struct nodoCpu *lista_cpus; //
int contador_id; // Used for CPU IDs

// Global process lists and PID counter
t_list* lista_procesos_nuevos; // Processes waiting to be admitted
t_list* lista_procesos_listos; // Processes ready for CPU execution
int next_pid;

void agregarCpuLista(char *ip, char *puerto);
void inicializarListasDeProcesos();
void crearNuevoProceso(char* instrucciones); // Creates and adds a process to the 'new' list
void manejarCliente(int cliente); // Handles incoming messages from clients
void planificarProceso(); // New function for scheduling (e.g., long-term scheduler)
void enviarProcesoACPU(Proceso* proceso, int cpu_socket); // Simulates sending PCB to CPU

#endif
