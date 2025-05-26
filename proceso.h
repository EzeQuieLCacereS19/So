#ifndef PROCESO_H
#define PROCESO_H

#include <commons/collections/list.h> // Include for t_list

enum EstadoProceso{
    NUEVO,
    LISTO,
    EN_PROCESO,
    EN_ESPERA,
    FINALIZADO
};

// Define the PCB structure
typedef struct {
    int pid; // Process ID
    int program_counter; // Program Counter
    // Add other PCB fields as needed, e.g.:
    // int base_direcciones;
    // t_list* tabla_segmentos;
    // t_list* recursos_asignados;
    // int estimacion_rafaga;
    // int tiempo_llegada_ready;
} t_pcb;

typedef struct Proceso {
    t_pcb* pcb; // Pointer to the PCB
    char* instrucciones; // Instructions for the process
    enum EstadoProceso estado; // Current state of the process
} Proceso;

// Function prototypes updated for list operations
void agregarProcesoLista(t_list* lista, Proceso* proceso);
Proceso* obtenerProcesoPorPID(t_list* lista, int pid);
void eliminarProcesoDeLista(t_list* lista, int pid);
void destruirListaDeProcesos(t_list* lista);

#endif
