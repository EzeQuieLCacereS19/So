#include <kernel.h>
#include <string.h>
#include <stdlib.h> // For malloc, free, strdup
#include <unistd.h> // For sleep (simulation)

extern t_log* log; // Declare the global log variable

void inicializarListaCpu() {
    lista_cpus = NULL;
    contador_id = 1;
}

void agregarCpuLista(char *ip, char *puerto) {
    struct nodoCpu *nuevo = malloc(sizeof(struct nodoCpu)); // Use struct nodoCpu
    if (!nuevo) {
        perror("Error al agregar CPU");
        return;
    }

    nuevo->cpu.id = contador_id++;
    nuevo->cpu.ip = strdup(ip); // Duplicate string to manage memory
    nuevo->cpu.puerto = strdup(puerto); // Duplicate string
    nuevo->cpu.estado = CONECTADO;
    nuevo->sgte = NULL;

    if (lista_cpus == NULL) {
        lista_cpus = nuevo;
    } else {
        struct nodoCpu *aux = lista_cpus;
        while (aux->sgte != NULL) {
            aux = aux->sgte;
        }
        aux->sgte = nuevo;
    }

    log_debug(log, "CPU agregada: ID=%d, IP=%s, PUERTO=%s", nuevo->cpu.id, nuevo->cpu.ip, nuevo->cpu.puerto); // Use log
}

void inicializarListasDeProcesos() {
    lista_procesos_nuevos = list_create();
    lista_procesos_listos = list_create();
    next_pid = 1; // Initialize PID counter
}

void crearNuevoProceso(char* instrucciones) {
    Proceso* nuevoProceso = malloc(sizeof(Proceso));
    if (!nuevoProceso) {
        perror("No se pudo asignar memoria para el nuevo proceso");
        return;
    }

    t_pcb* nuevoPcb = malloc(sizeof(t_pcb));
    if (!nuevoPcb) {
        perror("No se pudo asignar memoria para el PCB del nuevo proceso");
        free(nuevoProceso);
        return;
    }

    // Initialize PCB fields
    nuevoPcb->pid = next_pid++; // Assign PID
    nuevoPcb->program_counter = 0; // Initialize PC to 0
    // Initialize other PCB fields as needed

    nuevoProceso->pcb = nuevoPcb;
    nuevoProceso->instrucciones = strdup(instrucciones); // Duplicate instructions
    nuevoProceso->estado = NUEVO; // Set initial state to NEW

    agregarProcesoLista(lista_procesos_nuevos, nuevoProceso); // Add to the new processes list
    log_debug(log, "Nuevo proceso creado y agregado a la lista NUEVOS: PID=%d, Instrucciones=%s", nuevoProceso->pcb->pid, nuevoProceso->instrucciones);
}

// Long-Term Scheduler (example: FIFO for moving from NUEVO to LISTO)
void planificarProceso() {
    if (!list_is_empty(lista_procesos_nuevos)) {
        // Simple FIFO: get the first process from the 'NUEVO' list
        Proceso* proceso_a_admitir = (Proceso*)list_remove(lista_procesos_nuevos, 0);

        if (proceso_a_admitir != NULL) {
            proceso_a_admitir->estado = LISTO; // Change state to LISTO
            agregarProcesoLista(lista_procesos_listos, proceso_a_admitir); // Add to 'LISTO' list
            log_debug(log, "Proceso PID %d admitido de NUEVO a LISTO.", proceso_a_admitir->pcb->pid);
        }
    }

    // Short-Term Scheduler (example: FIFO for moving from LISTO to CPU)
    if (!list_is_empty(lista_procesos_listos)) {
        // Find an available CPU
        struct nodoCpu *cpu_disponible = NULL;
        struct nodoCpu *aux = lista_cpus;
        while (aux != NULL) {
            if (aux->cpu.estado == CONECTADO) { // Assuming CONECTADO means available for now
                cpu_disponible = aux;
                break;
            }
            aux = aux->sgte;
        }

        if (cpu_disponible != NULL) {
            // Get the first process from the 'LISTO' list (FIFO)
            Proceso* proceso_a_ejecutar = (Proceso*)list_remove(lista_procesos_listos, 0);

            if (proceso_a_ejecutar != NULL) {
                proceso_a_ejecutar->estado = EN_PROCESO; // Change state to EN_PROCESO
                cpu_disponible->cpu.estado = OCUPADO; // Mark CPU as busy
                log_debug(log, "Enviando proceso PID %d a CPU ID %d para ejecución.", proceso_a_ejecutar->pcb->pid, cpu_disponible->cpu.id);

                // Simulate sending the PCB to the CPU
                enviarProcesoACPU(proceso_a_ejecutar, -1 /* Placeholder for CPU socket */); // Will implement actual sending logic later

                // In a real scenario, the CPU would send back a response,
                // and the kernel would update the process state based on that.
                // For now, simulate completion or yield.
                // For demonstration, let's assume it finishes or yields.
                // In a real system, you'd wait for a message from the CPU.
                log_debug(log, "Proceso PID %d finalizado/cedido por CPU ID %d (simulado).", proceso_a_ejecutar->pcb->pid, cpu_disponible->cpu.id);
                proceso_a_ejecutar->estado = FINALIZADO; // Or LISTO if it yields
                cpu_disponible->cpu.estado = CONECTADO; // Make CPU available again
                eliminarProcesoDeLista(lista_procesos_listos, proceso_a_ejecutar->pcb->pid); // Remove from list if finished
                // If it yielded, you'd add it back to the LISTO queue or another appropriate queue.
            }
        } else {
            log_debug(log, "No hay CPU disponible para ejecutar procesos.");
        }
    }
}

// Simulates sending the PCB to a CPU
// In a real scenario, this would involve serializing the PCB and sending it over a socket
void enviarProcesoACPU(Proceso* proceso, int cpu_socket) {
    log_debug(log, "Simulando envío de PCB del PID %d a la CPU (socket %d).", proceso->pcb->pid, cpu_socket);
    // Here you would serialize proceso->pcb and send it via cpu_socket
    // For now, it's just a log message.
    sleep(1); // Simulate some work
}


// Function to handle incoming client connections and operations
void manejarCliente(int cliente) {
    int cod_op;
    while (1) {
        cod_op = recibir_operacion(cliente);
        switch (cod_op) {
            case MENSAJE:
                log_debug(log, "manejarCliente: Recibiendo MENSAJE de %i, llamando a recibir_mensaje", cliente);
                recibir_mensaje(cliente);
                break;
            case PAQUETE: {
                t_list* lista_recibida = recibir_paquete(cliente);
                // Assuming the first item in the package is the instructions for a new process
                if (list_size(lista_recibida) > 0) {
                    char* instrucciones = (char*)list_get(lista_recibida, 0);
                    crearNuevoProceso(instrucciones); // Create and add to 'NUEVOS' list
                    planificarProceso(); // Attempt to schedule after a new process arrives
                }
                list_destroy_and_destroy_elements(lista_recibida, free); // Free list and its elements
                break;
            }
            case -1:
                close(cliente);
                log_debug(log, "manejarCliente: Cliente %d desconectado.", cliente);
                return;
            default:
                log_debug(log, "manejarCliente: Operacion desconocida %d de cliente %d", cod_op, cliente);
                break;
        }
    }
}
