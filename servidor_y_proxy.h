#include "includes_y_defines.h"
//Entregable 3

// Definir funciones para mandar y recibir mensajes

// Función para leer una línea desde un socket
ssize_t readLine(int socket, char * buffer, size_t n)
{
    if (n <= 0 || buffer == NULL) {
        perror("Error al leer el mensaje");
        return -2;
    }

    // Número de bytes leídos por el último fetch
    ssize_t numRead;   
    // Total de bytes leídos hasta el momento
    size_t totRead = 0;   
    char * buf = buffer;
    char ch;

    for (;;) {
        // Leer un byte del socket
        numRead = read(socket, &ch, 1);   
        if (numRead == -1) {
            // reiniciar read al ser interrumpido
            if (errno == EINTR) continue;
            else{
                //Cualquier otro error
                close(socket);
                perror("Error al leer el mensaje");
                return -2;                  
            }
        } else if (numRead == 0) { //EOF
            // ningún byte leído, lo que implica que el archivo está vacío
            if (totRead == 0)          
                return 0;
            else
                break;
        } else { 
            // numRead debe ser 1 si hemos llegado hasta aquí
            if (ch == '\n')
                break;
            if (ch == '\0')
                break;
            // descartar n-1 bytes
            if (totRead < n - 1) {     
                totRead++;
                *buf++ = ch;
            }
        }
    }

    *buf = '\0';
    return totRead;
}

// Función para enviar un mensaje carácter por carácter
int sendMessage(int socket, char * buffer, int len){
    int r;
    int l = len;
    do {
        r = write(socket, buffer, 1);
        l -= r;
        buffer += r;
    } while ((l>0) && (r>=0));

    if (r < 0){
        close(socket);
        perror("Error al enviar la petición");
        return -2;
    }
    else {
        return 0;
    }

}

// Función para recibir un mensaje carácter por carácter
int recvMessage(int socket, char * buffer, int len){
    int r;
    int l = len;
    // leer los datos byte a byte
    do {
        r = read(socket, buffer, 1);
        l -= r;
        buffer += r;
    } while ((l>0) && (r>=0));

    if (r < 0){
        close(socket);
        perror("Error al recuperar la respuesta");
        return -2;
    }
    else {
        return 0;
    }
}

//Definir funciones para convertir strings a números
int strtol_handling(char * str, int * result){
    char * endPtr;
    long int n;
    errno = 0;
    n = (int) strtol(str, &endPtr, 10);
    // Manejo de errores
    if ((errno == ERANGE && (n == LONG_MAX || n == LONG_MIN)) || (errno != 0 && n == 0)){
        perror("La función strtol ha fallado");
        return ERROR_COMMUNICATION;
    }
    else if (endPtr == str) {
        perror("La función strtol no ha encontrado digitos");
        return ERROR_COMMUNICATION;
    }

    if (n > INT_MAX || n < INT_MIN){
        perror("La función strtol ha encontrado un valor que supera el límite de un entero");
        return ERROR_COMMUNICATION;
    }

    *result = (int) n;

    return 0;
}

// Conversión de string a doublee usando strtod
double strtod_handling(char * str, double * result){
    char * endPtr;
    double n;
    errno = 0;
    n = strtod(str, &endPtr);
    if ((errno == ERANGE && (n == DBL_MAX || n == DBL_MIN)) || (errno != 0 && n == 0)){
        perror("La función strtod ha fallado");
        return ERROR_COMMUNICATION;
    }
    else if (endPtr == str) {
        perror("La función strtod no ha encontrado digitos");
        return ERROR_COMMUNICATION;
    }

    *result = n;

    return 0;
}

// Definir función para leer números desde un socket
int read_num_from_socket(int s_local, char * buffer, int* num) {
    if (readLine(s_local, buffer, 256) < 0) return ERROR_COMMUNICATION;
    if (strtol_handling(buffer, num) < 0) return ERROR_COMMUNICATION;
    return 0;
}