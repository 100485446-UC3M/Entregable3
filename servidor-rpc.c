#include "claves.h"
#include "tuplas.h"  // Generado por rpcgen

//Definir mutex, variable condicional y variable global de sincronización 'busy'
pthread_mutex_t mutex2;
pthread_cond_t cond;
int busy;

//Implementación de los procedimientos _svc generados por rpcgen 
//Se espera el formato <nombre_procedimiento>_<numero_version>_svc
int * destroy_1_svc(void *argp, struct svc_req *rqstp) {
    static int result; // RPC requiere que el puntero devuelto apunte a dato estático o malloc'd
    printf("SERVIDOR RPC: Recibida petición DESTROY\n");
    result = destroy(); // Llama a la función backend
    return &result;
}

int * delete_key_1_svc(int *key, struct svc_req *rqstp) {
    static int result;
    printf("SERVIDOR RPC: Recibida petición DELETE_KEY para key %d\n", *key);
    result = delete_key(*key); // Llama a la función backend
    return &result;
}

int * exist_1_svc(int *key, struct svc_req *rqstp) {
    static int result;
    printf("SERVIDOR RPC: Recibida petición EXIST para key %d\n", *key);
    result = exist(*key); // Llama a la función backend
    return &result;
}

int * set_value_1_svc(set_modify_args *args, struct svc_req *rqstp) {
    static int result;
    printf("SERVIDOR RPC: Recibida petición SET_VALUE para key %d\n", args->key);
    // Extraer datos de la estructura de argumentos RPC
    // value2.value2_len y value2.value2_val son generados por rpcgen para el array variable
    result = set_value(args->key,
                       args->value1,
                       args->value2.value2_len, // N_value2
                       args->value2.value2_val, // V_value2 (puntero al array)
                       args->value3);          // value3
    return &result;
}

get_value_res * get_value_1_svc(int *key, struct svc_req *rqstp) {
    // RPC requiere devolver un puntero a estático o malloc'd.
    static get_value_res result;
    // Liberar memoria de la llamada anterior si era dinámica (strings, arrays)
    // xdr_free puede ser útil aquí si se usó malloc en la anterior llamada.
    // Con 'static result', debemos limpiar los punteros internos manualmente si apuntan a memoria dinámica.
    // Si value1 y value2_val apuntaban a memoria dinámica, hay que liberarla ANTES de reutilizar 'result'.
    if (result.value1 != NULL) {
       free(result.value1); // Si backend devolvió malloc'd string
       result.value1 = NULL;
    }
     if (result.value2.value2_val != NULL) {
       free(result.value2.value2_val); // Si backend devolvió malloc'd array
       result.value2.value2_val = NULL;
       result.value2.value2_len = 0;
    }
    printf("SERVIDOR RPC: Recibida petición GET_VALUE para key %d\n", *key);

    // Necesitamos buffers temporales para llamar a la función backend get_value
    // ya que la firma original usa punteros para devolver valores.
    char temp_value1[256]; 
    int temp_n_value2;
    double temp_v_value2[32]; 
    struct Coord temp_value3;

    result.status = get_value(*key, temp_value1, &temp_n_value2, temp_v_value2, &temp_value3);

    if (result.status == 0) { 
        // Copiar datos a la estructura de resultado RPC. ¡Cuidado con la memoria!
        // Para strings y arrays variables, RPC necesita que asignemos memoria que él pueda gestionar/liberar (o usar static).
        // Si usamos 'static result', debemos asignar memoria para los punteros internos.
        result.value1 = strdup(temp_value1); // strdup usa malloc
        if (!result.value1) { result.status = -1; }

        result.value2.value2_len = temp_n_value2;
        if (temp_n_value2 > 0) {
            result.value2.value2_val = (double *)malloc(temp_n_value2 * sizeof(double));
            if (!result.value2.value2_val) { result.status = -1; free(result.value1); }
            else {
               memcpy(result.value2.value2_val, temp_v_value2, temp_n_value2 * sizeof(double));
            }
        } else {
            result.value2.value2_val = NULL; /
        }
        result.value3 = temp_value3;

        // Si hubo error de memoria al copiar, limpiar lo asignado
        if (result.status != 0) {
             if(result.value1) free(result.value1);
             if(result.value2.value2_val) free(result.value2.value2_val);
             result.value1 = NULL;
             result.value2.value2_val = NULL;
             result.value2.value2_len = 0;
        }

    } else { // Error en get_value backend
        // Asegurarse de que los punteros en 'result' son NULL si status != 0
        result.value1 = NULL;
        result.value2.value2_len = 0;
        result.value2.value2_val = NULL;
        // result.value3 no es puntero, no necesita limpieza
    }

    return &result;
}

int * modify_value_1_svc(set_modify_args *args, struct svc_req *rqstp) {
    static int result;
    printf("SERVIDOR RPC: Recibida petición MODIFY_VALUE para key %d\n", args->key);
    result = modify_value(args->key,
                          args->value1,
                          args->value2.value2_len,
                          args->value2.value2_val,
                          args->value3);
    return &result;
}

/* main del servidor - Puede ser el generado por rpcgen o uno simple */
int main (int argc, char **argv) {
    printf("SERVIDOR RPC: Iniciando...\n");

    // Eliminar registro previo (útil durante desarrollo)
	(void)pmap_unset (TUPLAS_PROG, TUPLAS_VERS);

	// Registrar el servicio con el portmapper (rpcbind)
    // El código generado en tuplas_svc.c a menudo tiene una función main
    // que hace esto y llama a svc_run(). Se puede usar esa o hacerlo manualmente.
	register SVCXPRT *transp;
	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s: cannot create udp service.\n", argv[0]);
		exit(1);
	}
	if (!svc_register(transp, TUPLAS_PROG, TUPLAS_VERS, tuplas_prog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s: unable to register (TUPLAS_PROG, TUPLAS_VERS, udp).\n", argv[0]);
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s: cannot create tcp service.\n", argv[0]);
		exit(1);
	}
	if (!svc_register(transp, TUPLAS_PROG, TUPLAS_VERS, tuplas_prog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s: unable to register (TUPLAS_PROG, TUPLAS_VERS, tcp).\n", argv[0]);
		exit(1);
	}

    printf("SERVIDOR RPC: Servicio registrado y esperando peticiones.\n");
	// Entrar en el bucle de atención de RPCs
	svc_run (); // Esta función nunca retorna normalmente
	fprintf (stderr, "%s: svc_run returned\n", argv[0]);
	exit (1);  /* should never reach here */
}