#include <utils/utils.h>

t_log* log;

int iniciar_servidor(char* puerto){
	int socket_servidor;
	int err;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (socket_servidor == -1) log_debug(log, "iniciar_servidor: ERROR AL CREAR EL SOCKET DEL SERVIDOR EN EL PUERTO %s", puerto);
	
	err = bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	if (err) log_debug(log, "iniciar_servidor: ERROR DE BIND EN SERVIDOR: %i", err);

	err = listen(socket_servidor, SOMAXCONN);
	if (err) log_debug(log, "iniciar_servidor: ERROR DE LISTEN EN SERVIDOR: %i EN SERVIDOR PUERTO", err);

	freeaddrinfo(servinfo);
	return socket_servidor;
}

int esperar_cliente(int socket_servidor){
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	return socket_cliente;
}

void recibir_mensaje(int socket_cliente){	
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_debug(log, "recibir_mensaje: Me llego el mensaje: %s", buffer);
	free(buffer);
}

int crear_conexion(char* ip, char* puerto){
	int err;
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	err = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	if(err == -1){
		log_debug(log, "crear_conexion: ERROR AL CONECTAR");
		return err;
	}
	else{
		freeaddrinfo(server_info);
		return socket_cliente;
	}
}

void enviar_mensaje(char* mensaje, int socket_cliente){	
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void manejar_cliente(int cliente){
	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente);
		switch (cod_op) {
		case MENSAJE:
			log_debug(log, "manejar_cliente: Recibiendo MENSAJE de %i, llamando a recibir_mensaje", cliente);
			recibir_mensaje(cliente);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente);
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			close(cliente);
			return;
		default:
			break;
		}
	}
}


int recibir_operacion(int socket_cliente){
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0){
		return cod_op;
	}else{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente){
	void * buffer;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
	return buffer;
}

t_list* recibir_paquete(int socket_cliente){
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size){
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

void iterator(char* value) {
	log_debug(log, "%s", value);
}

void* serializar_paquete(t_paquete* paquete, int bytes){
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void eliminar_paquete(t_paquete* paquete){
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket){
	close(socket);
}

//EXTRAS


void iniciar_conecciones(char* puerto){
	int conecciones = 0;
	int servidor = iniciar_servidor(puerto);
	while (conecciones != 3){
		int cliente = esperar_cliente(servidor);	
		manejar_cliente(cliente);
		conecciones++;
	}
}

void conectar_componete(char* ip, char* puerto, char* mensaje){
	int conexion;
	//log_debug(log, "main: Llamando crear_conexion(%s, %s)", ip, puerto);
	conexion = crear_conexion(ip, puerto);
	//log_debug(log, "main: Conexion realizada en: %i", conexion);
	enviar_mensaje(mensaje, conexion);
	liberar_conexion(conexion);
}