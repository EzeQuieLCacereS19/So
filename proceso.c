#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For strdup
#include <proceso.h>
#include <commons/collections/list.h> // Include for t_list functions

void agregarProcesoLista(t_list* lista, Proceso* proceso) {
    if (lista == NULL) {
        fprintf(stderr, "Error: La lista de procesos no ha sido inicializada.\n");
        return;
    }
    list_add(lista, proceso);
}

Proceso* obtenerProcesoPorPID(t_list* lista, int pid) {
    if (lista == NULL) {
        fprintf(stderr, "Error: La lista de procesos no ha sido inicializada.\n");
        return NULL;
    }
    for (int i = 0; i < list_size(lista); i++) {
        Proceso* p = (Proceso*)list_get(lista, i);
        if (p->pcb->pid == pid) { // Access PID through pcb
            return p;
        }
    }
    return NULL; // Proceso no encontrado
}

void eliminarProcesoDeLista(t_list* lista, int pid) {
    if (lista == NULL) {
        fprintf(stderr, "Error: La lista de procesos no ha sido inicializada.\n");
        return;
    }
    for (int i = 0; i < list_size(lista); i++) {
        Proceso* p = (Proceso*)list_get(lista, i);
        if (p->pcb->pid == pid) { // Access PID through pcb
            Proceso* eliminado = (Proceso*)list_remove(lista, i);
            if (eliminado != NULL) {
                free(eliminado->instrucciones); // Free allocated instructions
                free(eliminado->pcb); // Free the PCB
                free(eliminado); // Free the process struct
            }
            return;
        }
    }
}

void destruirListaDeProcesos(t_list* lista) {
    if (lista == NULL) {
        return;
    }
    // Custom destructor to free Proceso, its instructions, and its PCB
    void _proceso_destroyer(void* element) {
        Proceso* p = (Proceso*)element;
        free(p->instrucciones);
        free(p->pcb);
        free(p);
    }
    list_destroy_and_destroy_elements(lista, _proceso_destroyer);
}
