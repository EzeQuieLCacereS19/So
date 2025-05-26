#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h> // Para definiciones de sockets: AF_INET - SOCK_STREAM
#include <netdb.h> // Para definiciones de socekts: AI_PASSIVE
#include <unistd.h> // Para función close()
#include <string.h> // Para función memset() - memcpy()
#include <commons/collections/list.h> // Listas de commons para la recepción de información por sockets
#include <commons/log.h> // Logs

typedef enum // Definición de códigos de operación para indicar el tipo de comunicación entre sockets
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct // Definición de buffer para transferencia de informacioń por servidores
{
	int size;
	void* stream;
} t_buffer;

typedef struct // Definición de paquete para transferencia de informacioń por servidores
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

extern t_log* log; // Inicializo variable de logs

/**
* @brief Inicia un servidor en el puerto indicado
* @param puerto Puerto donde se iniciará el servidor (char*)
* @return FD (int) del servidor creado
*/
int iniciar_servidor(char* puerto);

/**
* @brief Espera la conexion de un cliente al servidor del socket dado
* @param socket_servidor socket del servidor
* @return FD (int) del cliente conectado
*/
int esperar_cliente(int socket_servidor);

/**
* @brief Recibe el codigo de operación del cliente dado
* @param socket_cliente socket del cliente que esperamos el código de operación
* @return int código de operación
*/
int recibir_operacion(int socket_cliente);

/**
* @brief Recibe un buffer del socket del cliente dado
* @param size tamaño del buffer esperado (int*)
* @param socket_cliente socket del cliente que esperamos el buffer
* @return void* puntero hacia el buffer recibido
*/
void* recibir_buffer(int* size, int socket_cliente);

/**
* @brief Recibe un paquete del socket de cliente dado y lo guarda en una lista que nos devuelve
* @param socket_cliente socket del cliente que esperamos un paquete
* @return t_list* con los valores recibidos
*/
void recibir_mensaje(int socket_cliente);

/**
* @brief Recibe un paquete del socket de cliente dado y lo guarda en una lista que nos devuelve
* @param socket_cliente socket del cliente que esperamos un paquete
* @return t_list* con los valores recibidos
*/
t_list* recibir_paquete(int socket_cliente);

/**
* @brief Funcion iteradora para pasar a list_iterate y nos muestre en consola cada valor dado
* @param value valor a mostrar (char*)
* @return void
*/
void iterator(char* value);

/**
* @brief Crea una conexión a la ip y puerto indicados
* @param ip IP a conectarse (char*)
* @param puerto Puerto a conectarse (char*)
* @return FD (int) de la conexión creada
*/
int crear_conexion(char* ip, char* puerto);

void* serializar_paquete(t_paquete* paquete, int bytes);
void enviar_mensaje(char* mensaje, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);

/**
* @brief Libera la conexión del socket dado
* @param socket socket de la conexión a terminar
* @return void
*/
void liberar_conexion(int socket);

/**
* @brief Realiza la recepcion de informacion - imprime en consola el mensaje y finaliza la conexion con el cliente
* @param socket socket del cliente a realizar operaciones
* @return void
*/
void manejar_cliente(int cliente);
void conectar_componete(char* ip, char* puerto, char* mensaje);
void iniciar_conecciones(char* puerto);

#endif