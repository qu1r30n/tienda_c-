#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>

#define MAX_LINEA 1024
#define COLUMNAS 33
#define MAX_PRODUCTOS 1000

// ======================================================
// VARIABLES GLOBALES
// ======================================================
class var_fun_GG {
public:
    // Caracteres de separación
    static const char* G_caracter_separacion[5];
    
    // Archivos de inventario: [0] = ruta, [1] = cabecera
    static const char* G_archivos[1][2];

    // Archivos de registros: [0] = ruta, [1] = cabecera
    static const char* G_archivos_registros[2][2];

    static const int ARCH_INVENTARIO = 0;
};

const char* var_fun_GG::G_caracter_separacion[5] = { "|", "°", "¬", "╦", "╔" };
const char* var_fun_GG::G_archivos[1][2] = {
    { "C:\\Users\\qu1r30n\\Documents\\GitHub\\tienda_c-\\inventario.txt",
      "ID|Producto|Contenido|TipoMedida|PrecioVenta|CodBarras|Cantidad|CostoCompra|Proveedor|Col10|Col11|Col12|Col13|Col14|Col15|Col16|Col17|Fecha" }
};
const char* var_fun_GG::G_archivos_registros[2][2] = {
    { "C:\\Users\\qu1r30n\\Documents\\GitHub\\tienda_c-\\registros_ventas.txt", "" },
    { "C:\\Users\\qu1r30n\\Documents\\GitHub\\tienda_c-\\registros_compras.txt", "" }
};

// ======================================================
// UTILIDADES TEXTO
// ======================================================
class operaciones_textos {
public:
    static char** split(const char* txt, const char* sep, int* n) {
        char* c = _strdup(txt);
        char** out = (char**)malloc(sizeof(char*) * 64);
        int i = 0;

        char* tok = strtok(c, sep);
        while (tok) {
            out[i++] = _strdup(tok);
            tok = strtok(NULL, sep);
        }
        *n = i;
        free(c);
        return out;
    }

    static void free_split(char** a, int n) {
        for (int i = 0; i < n; i++) free(a[i]);
        free(a);
    }
};

// ======================================================
// FECHA
// ======================================================
class operaciones_compu {
public:
    static void fechaActual(char* b, const char* f) {
        time_t t = time(NULL);
        strftime(b, 20, f, localtime(&t));
    }
};

// ======================================================
// BASE DE DATOS TEXTO (tex_bas)
// ======================================================
class tex_bas {
public:
    // -------------------------------
    static void crearDirectorio(const char* ruta) {
        char tmp[512];
        strcpy(tmp, ruta);
        char* p = strrchr(tmp, '\\');
        if (p) { *p = 0; _mkdir(tmp); }
    }

    static void crearArchivo(const char* ruta, const char* cabecera) {
        crearDirectorio(ruta);
        FILE* f = fopen(ruta, "r");
        if (!f) {
            f = fopen(ruta, "w");
            if (cabecera) fprintf(f, "%s\n", cabecera);
        }
        if (f) fclose(f);
    }

    // -------------------------------
    static int leer_archivo(const char* ruta, char l[][MAX_LINEA]) {
        FILE* f = fopen(ruta, "r");
        if (!f) return 0;
        int n = 0;
        while (fgets(l[n], MAX_LINEA, f)) {
            l[n][strcspn(l[n], "\n")] = 0;
            n++;
        }
        fclose(f);
        return n;
    }

    static void guardar_archivo(const char* ruta, char l[][MAX_LINEA], int n) {
        FILE* f = fopen(ruta, "w");
        for (int i = 0; i < n; i++)
            fprintf(f, "%s\n", l[i]);
        fclose(f);
    }

    // ===============================
    // OPERACIONES TIPO BASE DE DATOS
    // ===============================

    static void agregar_fila(const char* ruta, const char* fila) {
        FILE* f = fopen(ruta, "a");
        fprintf(f, "%s\n", fila);
        fclose(f);
    }

    static void eliminar_fila(const char* ruta, int filaEliminar) {
        char l[1024][MAX_LINEA];
        int n = leer_archivo(ruta, l);
        int j = 0;
        for (int i = 0; i < n; i++)
            if (i != filaEliminar)
                strcpy(l[j++], l[i]);
        guardar_archivo(ruta, l, j);
    }

    static void editar_fila(const char* ruta, int fila, const char* nueva) {
        char l[1024][MAX_LINEA];
        int n = leer_archivo(ruta, l);
        strcpy(l[fila], nueva);
        guardar_archivo(ruta, l, n);
    }

    static void editar_celda(
        const char* ruta,
        int colBuscar,
        const char* valorBuscar,
        int colEditar,
        const char* nuevoValor
    ) {
        char l[1024][MAX_LINEA];
        int n = leer_archivo(ruta, l);

        for (int i = 1; i < n; i++) {
            int c;
            char** p = operaciones_textos::split(
                l[i], var_fun_GG::G_caracter_separacion[0], &c
            );
            if (strcmp(p[colBuscar], valorBuscar) == 0) {
                strcpy(p[colEditar], nuevoValor);
                l[i][0] = 0;
                for (int j = 0; j < c; j++) {
                    strcat(l[i], p[j]);
                    if (j < c - 1)
                        strcat(l[i], var_fun_GG::G_caracter_separacion[0]);
                }
            }
            operaciones_textos::free_split(p, c);
        }
        guardar_archivo(ruta, l, n);
    }

    static void incrementar_celda(
        const char* ruta,
        int colBuscar,
        const char* valorBuscar,
        int colEditar,
        int inc
    ) {
        char l[1024][MAX_LINEA];
        int n = leer_archivo(ruta, l);

        for (int i = 1; i < n; i++) {
            int c;
            char** p = operaciones_textos::split(l[i], "|", &c);
            if (!strcmp(p[colBuscar], valorBuscar)) {
                int v = atoi(p[colEditar]) + inc;
                sprintf(p[colEditar], "%d", v);
                l[i][0] = 0;
                for (int j = 0; j < c; j++) {
                    strcat(l[i], p[j]);
                    if (j < c - 1) strcat(l[i], "|");
                }
            }
            operaciones_textos::free_split(p, c);
        }
        guardar_archivo(ruta, l, n);
    }

    static void editar_columna_completa(
        const char* ruta,
        int col,
        const char* nuevo
    ) {
        char l[1024][MAX_LINEA];
        int n = leer_archivo(ruta, l);

        for (int i = 1; i < n; i++) {
            int c;
            char** p = operaciones_textos::split(l[i], "|", &c);
            strcpy(p[col], nuevo);
            l[i][0] = 0;
            for (int j = 0; j < c; j++) {
                strcat(l[i], p[j]);
                if (j < c - 1) strcat(l[i], "|");
            }
            operaciones_textos::free_split(p, c);
        }
        guardar_archivo(ruta, l, n);
    }

    static void eliminar_columna(const char* ruta, int col) {
        char l[1024][MAX_LINEA];
        int n = leer_archivo(ruta, l);

        for (int i = 0; i < n; i++) {
            int c;
            char** p = operaciones_textos::split(l[i], "|", &c);
            l[i][0] = 0;
            for (int j = 0; j < c; j++) {
                if (j == col) continue;
                strcat(l[i], p[j]);
                strcat(l[i], "|");
            }
            l[i][strlen(l[i]) - 1] = 0;
            operaciones_textos::free_split(p, c);
        }
        guardar_archivo(ruta, l, n);
    }

    static void agregar_columna(const char* ruta, const char* nombre) {
        char l[1024][MAX_LINEA];
        int n = leer_archivo(ruta, l);
        strcat(l[0], "|");
        strcat(l[0], nombre);

        for (int i = 1; i < n; i++)
            strcat(l[i], "|0");

        guardar_archivo(ruta, l, n);
    }

    // ================= INICIALIZACION =================
	void inicializacion() {
	    // Crear inventario
	    tex_bas::crearArchivo(
	        var_fun_GG::G_archivos[var_fun_GG::ARCH_INVENTARIO][0],
	        var_fun_GG::G_archivos[var_fun_GG::ARCH_INVENTARIO][1]
	    );
	    // Crear archivos de registros
	    for (int i = 0; i < 2; i++)
	        tex_bas::crearArchivo(
	            var_fun_GG::G_archivos_registros[i][0],
	            var_fun_GG::G_archivos_registros[i][1]
	        );
	}
    

};

// ======================================================
// OPERACIONES TIENDA (NEGOCIO)
// ======================================================
class operaciones_tienda {
public:

    // Leer inventario completo a memoria
    static int leerInventario(char inventario[][COLUMNAS][256], int maxProductos) {
        char lineas[MAX_PRODUCTOS + 1][MAX_LINEA];
        int nLineas = tex_bas::leer_archivo(var_fun_GG::G_archivos[var_fun_GG::ARCH_INVENTARIO][0], lineas);

        int fila = 0;
        for (int i = 1; i < nLineas && fila < maxProductos; i++) { // saltar cabecera
            int n;
            char** partes = operaciones_textos::split(lineas[i], var_fun_GG::G_caracter_separacion[0], &n);
            for (int j = 0; j < COLUMNAS; j++)
                strcpy(inventario[fila][j], (j < n) ? partes[j] : "0");
            operaciones_textos::free_split(partes, n);
            fila++;
        }
        return fila;
    }

    // Guardar inventario completo desde memoria
    static void guardarInventario(char inventario[][COLUMNAS][256], int n) {
        char lineas[MAX_PRODUCTOS + 1][MAX_LINEA];
        // Cabecera
        strcpy(lineas[0], var_fun_GG::G_archivos[var_fun_GG::ARCH_INVENTARIO][1]);
        // Filas
        for (int i = 0; i < n; i++) {
            lineas[i + 1][0] = 0;
            for (int j = 0; j < COLUMNAS; j++) {
                strcat(lineas[i + 1], inventario[i][j]);
                if (j < COLUMNAS - 1) strcat(lineas[i + 1], var_fun_GG::G_caracter_separacion[0]);
            }
        }
        tex_bas::guardar_archivo(var_fun_GG::G_archivos[var_fun_GG::ARCH_INVENTARIO][0], lineas, n + 1);
    }

    // Buscar producto por código de barras
    static int buscarProducto(char inventario[][COLUMNAS][256], int n, const char* codigo) {
        for (int i = 0; i < n; i++)
            if (strcmp(inventario[i][5], codigo) == 0)
                return i;
        return -1;
    }

    // Agregar producto nuevo
    static void agregarProducto(
        const char* id,
        const char* producto,
        const char* contenido,
        const char* tipo_medida,
        const char* precio_venta,
        const char* cod_barras,
        const char* cantidad,
        const char* costo_compra,
        const char* proveedor
    ) {
        char inventario[MAX_PRODUCTOS][COLUMNAS][256];
        int n = leerInventario(inventario, MAX_PRODUCTOS);

        char fila[COLUMNAS][256];
        for (int i = 0; i < COLUMNAS; i++) strcpy(fila[i], "0");

        strcpy(fila[0], id);
        strcpy(fila[1], producto);
        strcpy(fila[2], contenido);
        strcpy(fila[3], tipo_medida);
        strcpy(fila[4], precio_venta);
        strcpy(fila[5], cod_barras);
        strcpy(fila[6], cantidad);
        strcpy(fila[7], costo_compra);
        strcpy(fila[8], proveedor);
        operaciones_compu::fechaActual(fila[18], "%Y-%m-%d");

        for (int j = 0; j < COLUMNAS; j++)
            strcpy(inventario[n][j], fila[j]);

        guardarInventario(inventario, n + 1);
    }

    // Editar precio y proveedor
    static int editarPrecio(const char* codigo, const char* precio, const char* proveedor) {
        char inventario[MAX_PRODUCTOS][COLUMNAS][256];
        int n = leerInventario(inventario, MAX_PRODUCTOS);
        int idx = buscarProducto(inventario, n, codigo);
        if (idx == -1) return 0;

        strcpy(inventario[idx][4], precio);
        strcpy(inventario[idx][8], proveedor);
        operaciones_compu::fechaActual(inventario[idx][18], "%Y-%m-%d");
        guardarInventario(inventario, n);
        return 1;
    }

    
    // Venta
    static int venta(const char* codigo, int cantidad, const char* sucursal) {
        char inventario[MAX_PRODUCTOS][COLUMNAS][256];
        int n = leerInventario(inventario, MAX_PRODUCTOS);
        int idx = buscarProducto(inventario, n, codigo);
        if (idx == -1) return 0;
        int stock = atoi(inventario[idx][6]);
        if (stock < cantidad) return 0;
        stock -= cantidad;
        sprintf(inventario[idx][6], "%d", stock);
        operaciones_compu::fechaActual(inventario[idx][18], "%Y-%m-%d");
        guardarInventario(inventario, n);
        // registrar movimiento
        char registro[512], fecha[20];
        operaciones_compu::fechaActual(fecha, "%Y-%m-%d");
        sprintf(registro, "%s%s%d%s%s%s%s", codigo, var_fun_GG::G_caracter_separacion[0],
                cantidad, var_fun_GG::G_caracter_separacion[0],
                sucursal, var_fun_GG::G_caracter_separacion[0], fecha);
        tex_bas::agregar_fila(var_fun_GG::G_archivos_registros[0][0], registro);
        return 1;
    }
 // Compra
    static int compra(const char* codigo, int cantidad, const char* proveedor) {
        char inventario[MAX_PRODUCTOS][COLUMNAS][256];
        int n = leerInventario(inventario, MAX_PRODUCTOS);
        int idx = buscarProducto(inventario, n, codigo);
        if (idx == -1) return 0;
        int stock = atoi(inventario[idx][6]);
        stock += cantidad;
        sprintf(inventario[idx][6], "%d", stock);
        operaciones_compu::fechaActual(inventario[idx][18], "%Y-%m-%d");
        guardarInventario(inventario, n);
        // registrar movimiento
        char registro[512], fecha[20];
        operaciones_compu::fechaActual(fecha, "%Y-%m-%d");
        sprintf(registro, "%s%s%d%s%s%s%s", codigo, var_fun_GG::G_caracter_separacion[0],
                cantidad, var_fun_GG::G_caracter_separacion[0],
                proveedor, var_fun_GG::G_caracter_separacion[0], fecha);
        tex_bas::agregar_fila(var_fun_GG::G_archivos_registros[1][0], registro);
        return 1;
    }
};





// ======================================================
// MAIN (VISTA)
// ======================================================
int main() {
    // Inicializar inventario y archivos de registros
    tex_bas db;
	db.inicializacion();

    int opcion;
    do {
        printf("\n--- POS TIENDA ---\n");
        printf("1. Venta\n2. Compra\n3. Agregar producto\n4. Editar precio\n0. Salir\nOpcion: ");
        scanf("%d", &opcion);
        getchar();  // limpiar buffer

        if (opcion == 1) {
            char codigo[50], sucursal[50]; 
            int cantidad;
            printf("Codigo de barras: "); scanf("%s", codigo);
            printf("Cantidad: "); scanf("%d", &cantidad);
            printf("Sucursal: "); scanf("%s", sucursal);
            if (operaciones_tienda::venta(codigo, cantidad, sucursal))
                printf("Venta realizada\n");
            else
                printf("Stock insuficiente o codigo invalido\n");
        }
        else if (opcion == 2) {
            char codigo[50], proveedor[50]; 
            int cantidad;
            printf("Codigo de barras: "); scanf("%s", codigo);
            printf("Cantidad: "); scanf("%d", &cantidad);
            printf("Proveedor: "); scanf("%s", proveedor);
            if (operaciones_tienda::compra(codigo, cantidad, proveedor))
                printf("Compra realizada\n");
            else
                printf("Codigo invalido\n");
        }
        else if (opcion == 3) {
            char id[50], producto[100], contenido[50], tipo_medida[50];
            char precio_venta[50], cod_barras[50], cantidad[50], costo_comp[50], proveedor[50];
            printf("ID: "); scanf("%s", id);
            printf("Producto: "); scanf("%s", producto);
            printf("Contenido: "); scanf("%s", contenido);
            printf("Tipo medida: "); scanf("%s", tipo_medida);
            printf("Precio venta: "); scanf("%s", precio_venta);
            printf("Codigo barras: "); scanf("%s", cod_barras);
            printf("Cantidad: "); scanf("%s", cantidad);
            printf("Costo compra: "); scanf("%s", costo_comp);
            printf("Proveedor: "); scanf("%s", proveedor);
            operaciones_tienda::agregarProducto(id, producto, contenido, tipo_medida,
                                                precio_venta, cod_barras, cantidad, costo_comp, proveedor);
            printf("Producto agregado\n");
        }
        else if (opcion == 4) {
            char codigo[50], precio[50], proveedor[50];
            printf("Codigo de barras: "); scanf("%s", codigo);
            printf("Nuevo precio: "); scanf("%s", precio);
            printf("Proveedor: "); scanf("%s", proveedor);
            if (operaciones_tienda::editarPrecio(codigo, precio, proveedor))
                printf("Precio editado\n");
            else
                printf("Codigo invalido\n");
        }
    } while (opcion != 0);

    return 0;
}

