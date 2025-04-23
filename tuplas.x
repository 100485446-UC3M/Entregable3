/* Estructura para los argumentos de set_value y modify_value */
struct set_modify_args {
    int    key;
    string value1<>; 
    value2_t value2; 
    Coord  value3;
};

/* Estructura para el resultado de get_value. Incluye un código de estado y los valores si el estado es éxito. */
struct get_value_res {
    int    status;  
    string value1<>;
    value2_t value2;
    Coord  value3;
};

/* Definición del programa RPC */
program TUPLAS_PROG {
    version TUPLAS_VERS {
        /* Procedimientos RPC - coinciden con claves.h, pero adaptados a RPC */

        int DESTROY(void) = 0; 

        int DELETE_KEY(int key) = 1; 

        int EXIST(int key) = 2; 

        /* set_value usa la estructura de argumentos */
        int SET_VALUE(set_modify_args) = 3;

        /* get_value devuelve la estructura de resultado */
        get_value_res GET_VALUE(int key) = 4;

        /* modify_value usa la estructura de argumentos */
        int MODIFY_VALUE(set_modify_args) = 5;

    } = 1; /* Número de versión */
} = 0x20000001; /* Número de programa */
