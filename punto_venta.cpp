// ======================================================
// INCLUSIÓN DE LIBRERÍAS
// ======================================================

// Entrada y salida estándar (printf, scanf, FILE, fopen, etc.)
#include <stdio.h>

// Manejo de memoria dinámica (malloc, free, atoi, etc.)
#include <stdlib.h>

// Manejo de cadenas de texto (strcpy, strcat, strcmp, strtok, etc.)
#include <string.h>

// Manejo de fechas y tiempo del sistema
#include <time.h>

// Manejo de directorios (crear carpetas en Windows)
#include <direct.h>

// ======================================================
// CONSTANTES GLOBALES
// ======================================================

// Tamaño máximo de una línea leída de un archivo
#define MAX_LINEA 1024

// Número total de columnas que tiene el inventario
#define COLUMNAS 33

// Cantidad máxima de productos que puede manejar el sistema
#define MAX_PRODUCTOS 1000


// ======================================================
// VARIABLES GLOBALES (CONFIGURACIÓN GENERAL)
// ======================================================

class var_fun_GG {
public:
    // Caracteres de separación usados en los archivos de texto
    // Ejemplo: ID|Producto|Precio
    static const char* G_caracter_separacion[5];
    
    // Archivos principales del sistema
    // [archivo][0] = ruta
    // [archivo][1] = cabecera
    static const char* G_archivos[1][2];

    // Archivos de registros (ventas y compras)
    static const char* G_archivos_registros[2][2];

    // Índice del archivo de inventario
    static const int ARCH_INVENTARIO = 0;
};

// Inicialización de separadores
const char* var_fun_GG::G_caracter_separacion[5] = { "|", "°", "¬", "╦", "╔" };

// Archivo de inventario (ruta y cabecera)
const char* var_fun_GG::G_archivos[1][2] = {
    { 
      "C:\\Users\\qu1r30n\\Documents\\GitHub\\tienda_c-\\inventario.txt",
      "ID|Producto|Contenido|TipoMedida|PrecioVenta|CodBarras|Cantidad|CostoCompra|Proveedor|Col10|Col11|Col12|Col13|Col14|Col15|Col16|Col17|Fecha"
    }
};

// Archivos de registros de ventas y compras
const char* var_fun_GG::G_archivos_registros[2][2] = {
    { "C:\\Users\\qu1r30n\\Documents\\GitHub\\tienda_c-\\registros_ventas.txt", "" },
    { "C:\\Users\\qu1r30n\\Documents\\GitHub\\tienda_c-\\registros_compras.txt", "" }
};


// ======================================================
// UTILIDADES DE TEXTO
// ======================================================

class operaciones_textos {
public:

    // --------------------------------------------------
    // Divide un texto en partes usando un separador
    // Ejemplo: "A|B|C" → ["A","B","C"]
    // --------------------------------------------------
    static char** split(const char* txt, const char* sep, int* n) {

        // Copia del texto original (strtok modifica la cadena)
        char* c = _strdup(txt);

        // Reserva memoria para los fragmentos
        char** out = (char**)malloc(sizeof(char*) * 64);

        int i = 0;

        // Primer token
        char* tok = strtok(c, sep);

        // Mientras existan tokens
        while (tok) {
            out[i++] = _strdup(tok);
            tok = strtok(NULL, sep);
        }

        // Cantidad de fragmentos obtenidos
        *n = i;

        // Liberar copia temporal
        free(c);

        return out;
    }

    // --------------------------------------------------
    // Libera la memoria usada por split
    // --------------------------------------------------
    static void free_split(char** a, int n) {
        for (int i = 0; i < n; i++)
            free(a[i]);
        free(a);
    }
};


// ======================================================
// OPERACIONES DE FECHA Y TIEMPO
// ======================================================

class operaciones_compu {
public:

    // --------------------------------------------------
    // Obtiene la fecha actual con formato personalizado
    // --------------------------------------------------
    static void fechaActual(char* b, const char* f) {

        // Obtener tiempo actual
        time_t t = time(NULL);

        // Convertir tiempo a texto según formato
        strftime(b, 20, f, localtime(&t));
    }
};


// ======================================================
// BASE DE DATOS EN TEXTO (SIMULADA)
// ======================================================

class tex_bas {
public:

    // --------------------------------------------------
    // Crea el directorio donde irá el archivo
    // --------------------------------------------------
    static void crearDirectorio(const char* ruta) {

        char tmp[512];
        strcpy(tmp, ruta);

        // Busca el último '\'
        char* p = strrchr(tmp, '\\');

        // Si existe, corta la ruta y crea carpeta
        if (p) {
            *p = 0;
            _mkdir(tmp);
        }
    }

    // --------------------------------------------------
    // Crea un archivo si no existe
    // --------------------------------------------------
    static void crearArchivo(const char* ruta, const char* cabecera) {

        crearDirectorio(ruta);

        FILE* f = fopen(ruta, "r");

        // Si no existe, lo crea
        if (!f) {
            f = fopen(ruta, "w");
            if (cabecera)
                fprintf(f, "%s\n", cabecera);
        }

        if (f) fclose(f);
    }

    // --------------------------------------------------
    // Lee un archivo completo a memoria
    // --------------------------------------------------
    static int leer_archivo(const char* ruta, char l[][MAX_LINEA]) {

        FILE* f = fopen(ruta, "r");
        if (!f) return 0;

        int n = 0;

        while (fgets(l[n], MAX_LINEA, f)) {
            // Eliminar salto de línea
            l[n][strcspn(l[n], "\n")] = 0;
            n++;
        }

        fclose(f);
        return n;
    }

    // --------------------------------------------------
    // Guarda todas las líneas en un archivo
    // --------------------------------------------------
    static void guardar_archivo(const char* ruta, char l[][MAX_LINEA], int n) {

        FILE* f = fopen(ruta, "w");

        for (int i = 0; i < n; i++)
            fprintf(f, "%s\n", l[i]);

        fclose(f);
    }

    // --------------------------------------------------
    // Agrega una fila al final del archivo
    // --------------------------------------------------
    static void agregar_fila(const char* ruta, const char* fila) {

        FILE* f = fopen(ruta, "a");
        fprintf(f, "%s\n", fila);
        fclose(f);
    }

    // --------------------------------------------------
    // Inicialización del sistema
    // --------------------------------------------------
    void inicializacion() {

        // Crear inventario
        crearArchivo(
            var_fun_GG::G_archivos[var_fun_GG::ARCH_INVENTARIO][0],
            var_fun_GG::G_archivos[var_fun_GG::ARCH_INVENTARIO][1]
        );

        // Crear archivos de registros
        for (int i = 0; i < 2; i++)
            crearArchivo(
                var_fun_GG::G_archivos_registros[i][0],
                var_fun_GG::G_archivos_registros[i][1]
            );
    }
};


// ======================================================
// OPERACIONES DEL NEGOCIO (TIENDA)
// ======================================================

class operaciones_tienda {
public:

    // --------------------------------------------------
    // Lee el inventario completo a memoria
    // --------------------------------------------------
    static int leerInventario(char inventario[][COLUMNAS][256], int maxProductos) {

        char lineas[MAX_PRODUCTOS + 1][MAX_LINEA];

        // Leer archivo de inventario
        int nLineas = tex_bas::leer_archivo(
            var_fun_GG::G_archivos[var_fun_GG::ARCH_INVENTARIO][0],
            lineas
        );

        int fila = 0;

        // Saltar cabecera
        for (int i = 1; i < nLineas && fila < maxProductos; i++) {

            int n;
            char** partes = operaciones_textos::split(
                lineas[i],
                var_fun_GG::G_caracter_separacion[0],
                &n
            );

            // Copiar columnas
            for (int j = 0; j < COLUMNAS; j++)
                strcpy(inventario[fila][j], (j < n) ? partes[j] : "0");

            operaciones_textos::free_split(partes, n);
            fila++;
        }

        return fila;
    }

    // --------------------------------------------------
    // Guarda el inventario completo
    // --------------------------------------------------
    static void guardarInventario(char inventario[][COLUMNAS][256], int n) {

        char lineas[MAX_PRODUCTOS + 1][MAX_LINEA];

        // Cabecera
        strcpy(lineas[0],
            var_fun_GG::G_archivos[var_fun_GG::ARCH_INVENTARIO][1]
        );

        // Filas de productos
        for (int i = 0; i < n; i++) {
            lineas[i + 1][0] = 0;

            for (int j = 0; j < COLUMNAS; j++) {
                strcat(lineas[i + 1], inventario[i][j]);
                if (j < COLUMNAS - 1)
                    strcat(lineas[i + 1], "|");
            }
        }

        tex_bas::guardar_archivo(
            var_fun_GG::G_archivos[var_fun_GG::ARCH_INVENTARIO][0],
            lineas,
            n + 1
        );
    }

    // --------------------------------------------------
    // Busca un producto por código de barras
    // --------------------------------------------------
    static int buscarProducto(char inventario[][COLUMNAS][256], int n, const char* codigo) {

        for (int i = 0; i < n; i++)
            if (strcmp(inventario[i][5], codigo) == 0)
                return i;

        return -1;
    }

    // --------------------------------------------------
    // Realizar una venta
    // --------------------------------------------------
    static int venta(const char* codigo, int cantidad, const char* sucursal) {

        char inventario[MAX_PRODUCTOS][COLUMNAS][256];
        int n = leerInventario(inventario, MAX_PRODUCTOS);

        int idx = buscarProducto(inventario, n, codigo);
        if (idx == -1) return 0;

        int stock = atoi(inventario[idx][6]);
        if (stock < cantidad) return 0;

        // Descontar stock
        stock -= cantidad;
        sprintf(inventario[idx][6], "%d", stock);

        guardarInventario(inventario, n);

        // Registrar venta
        char registro[512], fecha[20];
        operaciones_compu::fechaActual(fecha, "%Y-%m-%d");

        sprintf(registro, "%s|%d|%s|%s",
                codigo, cantidad, sucursal, fecha);

        tex_bas::agregar_fila(
            var_fun_GG::G_archivos_registros[0][0],
            registro
        );

        return 1;
    }
};


// ======================================================
// FUNCIÓN PRINCIPAL
// ======================================================

int main() {

    // Inicializar archivos
    tex_bas db;
    db.inicializacion();

    int opcion;

    do {
        printf("\n--- POS TIENDA ---\n");
        printf("1. Venta\n2. Compra\n3. Agregar producto\n4. Editar precio\n0. Salir\nOpcion: ");
        scanf("%d", &opcion);
        getchar();

        if (opcion == 1) {
            char codigo[50], sucursal[50];
            int cantidad;

            printf("Codigo: "); scanf("%s", codigo);
            printf("Cantidad: "); scanf("%d", &cantidad);
            printf("Sucursal: "); scanf("%s", sucursal);

            if (operaciones_tienda::venta(codigo, cantidad, sucursal))
                printf("Venta realizada\n");
            else
                printf("Error en la venta\n");
        }

    } while (opcion != 0);

    return 0;
}
