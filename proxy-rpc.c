#include "claves.h"
#include "tuplas.h" // Generado por rpcgen


// Función auxiliar para crear/destruir cliente RPC y manejar errores comunes
static CLIENT *get_rpc_client(const char *host) {
    CLIENT *clnt = clnt_create(host, TUPLAS_PROG, TUPLAS_VERS, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror(host); 
    return clnt;
}
}

// Implementación de la API pública 

int destroy(void) {
    CLIENT *clnt;
    int *result;
    char *host;

    if ((host = getenv("IP_TUPLAS")) == NULL) {
        fprintf(stderr, "CLIENTE: Variable de entorno IP_TUPLAS no definida.\n");
        return ERROR_COMMUNICATION; // O un código de error específico
    }

    if ((clnt = get_rpc_client(host)) == NULL) {
        return ERROR_COMMUNICATION;
    }

    // Llama al stub RPC cliente, aquí está la información con respecto a la función que queremos ejecutar
    result = destroy_1(NULL, clnt); 
    if (result == (int *) NULL) {
        // Error en la llamada RPC
        clnt_perror(clnt, "CLIENTE: Fallo en llamada RPC a destroy");
        clnt_destroy(clnt);
        return ERROR_COMMUNICATION;
    }

    int retval = *result; // Obtener el valor devuelto por el servidor
    clnt_destroy(clnt);
    return retval;
}

int delete_key(int key) {
    CLIENT *clnt;
    int *result;
     char *host;

    if ((host = getenv("IP_TUPLAS")) == NULL) {  return ERROR_COMMUNICATION; }
    if ((clnt = get_rpc_client(host)) == NULL) {  return ERROR_COMMUNICATION; }

    result = delete_key_1(&key, clnt);
    if (result == (int *) NULL) {  clnt_destroy(clnt); return ERROR_COMMUNICATION; }

    int retval = *result;
    clnt_destroy(clnt);
    return retval;
}

int exist(int key) {
    CLIENT *clnt;
    int *result;
     char *host;

    if ((host = getenv("IP_TUPLAS")) == NULL) {  return ERROR_COMMUNICATION; }
    if ((clnt = get_rpc_client(host)) == NULL) {  return ERROR_COMMUNICATION; }

    result = exist_1(&key, clnt);
     if (result == (int *) NULL) { clnt_destroy(clnt); return ERROR_COMMUNICATION; }

    int retval = *result;
    clnt_destroy(clnt);
    return retval; // Devuelve 1 si existe, 0 si no, <0 si error
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    CLIENT *clnt;
    int *result;
    char *host;
    set_modify_args args; // Estructura para agrupar argumentos

    if ((host = getenv("IP_TUPLAS")) == NULL) { /* ... */ return ERROR_COMMUNICATION; }
    if ((clnt = get_rpc_client(host)) == NULL) { /* ... */ return ERROR_COMMUNICATION; }

    // Llenar la estructura de argumentos
    args.key = key;
    args.value1 = value1; // Puntero al string original
    args.value2.value2_len = N_value2;
    args.value2.value2_val = V_value2; // Puntero al array original
    args.value3 = value3;

    result = set_value_1(&args, clnt);
    if (result == (int *) NULL) { clnt_destroy(clnt); return ERROR_COMMUNICATION; }

    int retval = *result;
    clnt_destroy(clnt);
    return retval;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    CLIENT *clnt;
    get_value_res *result; // Puntero a la estructura de resultado
    char *host;

    // Validar punteros de salida
    if (!value1 || !N_value2 || !V_value2 || !value3) {
         // fprintf(stderr, "CLIENTE: Punteros de salida nulos en get_value.\n");
         return -1; // O un código de error apropiado
    }

    if ((host = getenv("IP_TUPLAS")) == NULL) { return ERROR_COMMUNICATION; }
    if ((clnt = get_rpc_client(host)) == NULL) {  return ERROR_COMMUNICATION; }

    result = get_value_1(&key, clnt);
    if (result == (get_value_res *) NULL) {
        // Error en la llamada RPC
        clnt_perror(clnt, "CLIENTE: Fallo en llamada RPC a get_value");
        clnt_destroy(clnt);
        return ERROR_COMMUNICATION;
    }

    // Copiar los resultados de la estructura devuelta a los punteros del usuario
    int status = result->status;

    if (status == 0) { // Éxito
        // Copiar value1 (¡cuidado con el tamaño del buffer!)
        if (result->value1 != NULL) {
            strncpy(value1, result->value1, 255); // Asume que value1 tiene tamaño 256
            value1[255] = '\0'; // Asegurar terminación null
        } else {
            value1[0] = '\0'; // String vacío si es NULL
        }

        // Copiar N_value2 y V_value2 (¡cuidado con el tamaño del buffer V_value2!)
        *N_value2 = result->value2.value2_len;
        int count = result->value2.value2_len;
        // Asumir que V_value2 tiene espacio suficiente (e.g., 32 doubles como en el servidor original)
        int max_ N_value2 = 32; // O obtenerlo de alguna manera si es variable
        if (count > max_N_value2) {
            fprintf(stderr, "CLIENTE: Advertencia - Buffer V_value2 demasiado pequeño en get_value. Recibidos %d, espacio %d\n", count, max_N_value2);
            count = max_N_value2; // Truncar para evitar desbordamiento
             *N_value2 = count; // Informar al usuario del número real copiado
             status = -1; // Indicar un error parcial o advertencia
        }
        if (count > 0 && result->value2.value2_val != NULL) {
             memcpy(V_value2, result->value2.value2_val, count * sizeof(double));
        }

        // Copiar value3
        *value3 = result->value3;

    } else {
        // Si hubo error en el servidor (status != 0), limpiar los valores de salida
        value1[0] = '\0';
        *N_value2 = 0;
        // value3 no necesita limpieza si no es puntero
    }

    // Liberar memoria asignada por la capa RPC cliente para el resultado
    // Esto es crucial para evitar fugas de memoria, especialmente para strings y arrays.
    xdr_free((xdrproc_t)xdr_get_value_res, (char *)result);

    clnt_destroy(clnt);
    return status; // Devuelve el código de estado del servidor
}


int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    CLIENT *clnt;
    int *result;
    char *host;
    set_modify_args args;

    if ((host = getenv("IP_TUPLAS")) == NULL) { return ERROR_COMMUNICATION; }
    if ((clnt = get_rpc_client(host)) == NULL) { return ERROR_COMMUNICATION; }

    args.key = key;
    args.value1 = value1;
    args.value2.value2_len = N_value2;
    args.value2.value2_val = V_value2;
    args.value3 = value3;

    result = modify_value_1(&args, clnt);
     if (result == (int *) NULL) { clnt_destroy(clnt); return ERROR_COMMUNICATION; }

    int retval = *result;
    clnt_destroy(clnt);
    return retval;
}