//Definir las librerías que serán utilizadas
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <float.h>
#include <rpc/rpc.h>

//Definir constantes globales que se van a utilizar con frecuencia
//#define DESTROY '0'
//#define DELETE_KEY '1'
//#define EXIST '2'
//#define SET_VALUE '3'
//#define GET_VALUE '4'
//#define MODIFY_VALUE '5'
#define ERROR_TUPLAS -1
#define ERROR_COMMUNICATION -2