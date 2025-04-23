# Variables de compilación
CC = gcc
CFLAGS = -Wall -fPIC -g
# -lnsl es común en sistemas Linux más antiguos.
# -ltirpc si estás usando la biblioteca libtirpc (recomendado si está disponible). Elige una.
# LDFLAGS_RPC = -lnsl
LDFLAGS_RPC = -ltirpc
LDFLAGS = -lrt -lpthread $(LDFLAGS_RPC)

# ---- Biblioteca Compartida ----
LIBRARY_NAME = libclaves.so

# ---- RPC ----
RPCGEN = rpcgen
# Nombre del fichero de definición RPC
TUPLAS_X = tuplas.x
# Ficheros generados por rpcgen
TUPLAS_HDR = tuplas.h
TUPLAS_CLNT_SRC = tuplas_clnt.c
TUPLAS_SVC_SRC = tuplas_svc.c
TUPLAS_XDR_SRC = tuplas_xdr.c
# Objetos generados por rpcgen
TUPLAS_CLNT_OBJ = $(OBJ_DIR)/tuplas_clnt.o
TUPLAS_SVC_OBJ = $(OBJ_DIR)/tuplas_svc.o
TUPLAS_XDR_OBJ = $(OBJ_DIR)/tuplas_xdr.o
# Lista de todos los archivos generados por rpcgen para la limpieza
RPC_GENERATED = $(TUPLAS_HDR) $(TUPLAS_CLNT_SRC) $(TUPLAS_SVC_SRC) $(TUPLAS_XDR_SRC)

# Ejecutables finales
SERVIDOR_RPC_SRC = $(SRC_DIR)/servidor-rpc.c
SERVIDOR_RPC_EXEC = servidor-rpc
PROXY_RPC_SRC = $(SRC_DIR)/proxy-rpc.c

CLIENT_EXEC1 = app-cliente-1
CLIENT_EXEC2 = app-cliente-2
CLIENT_EXEC3 = app-cliente-3
CLIENT_EXEC4 = app-cliente-4

# Directorios
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Archivos fuente
CLAVES_SRC = $(SRC_DIR)/claves.c
SERVER_SRC = $(SRC_DIR)/servidor-rpc.c
PROXY_SRC = $(SRC_DIR)/proxy-rpc.c
CLIENT_SRC1 = $(SRC_DIR)/app-cliente-1.c
CLIENT_SRC2 = $(SRC_DIR)/app-cliente-2.c
CLIENT_SRC3 = $(SRC_DIR)/app-cliente-3.c
CLIENT_SRC4 = $(SRC_DIR)/app-cliente-4.c

# Headers
HEADER_SRC = $(SRC_DIR)/claves.h
INCL_DEF_SRC = $(SRC_DIR)/includes_y_defines.h

# Archivos objeto
CLAVES_OBJ = $(OBJ_DIR)/claves.o
SERVER_OBJ = $(OBJ_DIR)/servidor-rpc.o
PROXY_OBJ = $(OBJ_DIR)/proxy-rpc.o
CLIENT_OBJ1 = $(OBJ_DIR)/app-cliente-1.o
CLIENT_OBJ2 = $(OBJ_DIR)/app-cliente-2.o
CLIENT_OBJ3 = $(OBJ_DIR)/app-cliente-3.o
CLIENT_OBJ4 = $(OBJ_DIR)/app-cliente-4.o

# ---- Agrupación de Objetos ----
# Objetos necesarios para la biblioteca compartida libclaves.so
# Contiene la API cliente RPC y los stubs/xdr necesarios para el cliente.
LIB_OBJS = $(RPC_CLIENT_API_OBJ) $(TUPLAS_CLNT_OBJ) $(TUPLAS_XDR_OBJ)

# Objetos necesarios para el ejecutable del servidor
# Contiene el main del servidor, la lógica backend y los stubs/xdr necesarios para el servidor.
SERVER_OBJS = $(RPC_SERVER_OBJ) $(CLAVES_OBJ) $(TUPLAS_SVC_OBJ) $(TUPLAS_XDR_OBJ)

# Regla principal
all: $(LIBRARY_NAME) $(SERVIDOR_RPC_EXE) $(CLIENT_EXEC1) $(CLIENT_EXEC2) $(CLIENT_EXEC3)

# Crear la biblioteca dinámica
$(LIBRARY_NAME): $(PROXY_OBJ)
	@echo "Creando la biblioteca dinámica $(LIBRARY_NAME)"
	@mkdir -p $(BIN_DIR)
	$(CC) -shared -o $(BIN_DIR)/$(LIBRARY_NAME) $(PROXY_OBJ) $(LDFLAGS)

$(SERVIDOR_RPC_EXEC): $(CLAVES_OBJ) $(SERVER_OBJ)
	@mkdir -p $(BIN_DIR)
	@echo "Compilando el servidor $(SERVIDOR_RPC_EXEC)"
	$(CC) -o $(BIN_DIR)/$(SERVIDOR_RPC_EXEC) $(CLAVES_OBJ) $(SERVER_OBJ) $(LDFLAGS)

# Compilar los clientes
$(CLIENT_EXEC1): $(CLIENT_OBJ1) $(LIBRARY_NAME)
	@echo "Compilando el cliente $(CLIENT_EXEC1)"
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$(CLIENT_EXEC1) $(CLIENT_OBJ1) -L$(BIN_DIR) -lclaves $(LDFLAGS) -Wl,-rpath,$(BIN_DIR)

$(CLIENT_EXEC2): $(CLIENT_OBJ2) $(LIBRARY_NAME)
	@echo "Compilando el cliente $(CLIENT_EXEC2)"
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$(CLIENT_EXEC2) $(CLIENT_OBJ2) -L$(BIN_DIR) -lclaves $(LDFLAGS) -Wl,-rpath,$(BIN_DIR)

$(CLIENT_EXEC3): $(CLIENT_OBJ3) $(LIBRARY_NAME)
	@echo "Compilando el cliente $(CLIENT_EXEC3)"
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$(CLIENT_EXEC3) $(CLIENT_OBJ3) -L$(BIN_DIR) -lclaves $(LDFLAGS) -Wl,-rpath,$(BIN_DIR)

$(CLIENT_EXEC4): $(CLIENT_OBJ4) $(LIBRARY_NAME)
	@echo "Compilando el cliente $(CLIENT_EXEC3)"
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/$(CLIENT_EXEC4) $(CLIENT_OBJ4) -L$(BIN_DIR) -lclaves $(LDFLAGS) -Wl,-rpath,$(BIN_DIR)

# Compilación de los archivos .c a .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER_SRC) $(INCL_DEF_SRC) $(SERV_PROX_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar los archivos generados
clean:
	@echo "Limpiando los archivos generados"
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Eliminar todo, incluyendo los archivos generados
distclean: clean
	@echo "Limpiando todo"
	rm -f $(BIN_DIR)/$(LIBRARY_NAME)

.PHONY: all clean distclean
