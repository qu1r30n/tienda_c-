#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>  // para _mkdir en Windows
#include <errno.h>

#define MAX_LINEA 1024
#define COLUMNAS 33
#define MAX_PRODUCTOS 1000

// ================= VARIABLES GLOBALES =================
class var_fun_GG 
{
public:
    static const char* G_caracter_separacion[5];
    static const char* G_archivos[1][2];
    static const int G_cantidad_archivos;
};

const char* var_fun_GG::G_caracter_separacion[5] = { "|", "°", "¬", "╦", "╔" };

const char* var_fun_GG::G_archivos[1][2] = {
    { 
        "C:\\Users\\qu1r30n\\Documents\\GitHub\\tienda_c-\\inventario.txt",
        "ID|Producto|Contenido|TipoMedida|PrecioVenta|CodBarras|Cantidad|CostoCompra|Proveedor|Col10|Col11|Col12|Col13|Col14|Col15|Col16|Col17|Fecha"
    }
};

const int var_fun_GG::G_cantidad_archivos = 1;

// ================= OPERACIONES DE TEXTO =================
class operaciones_textos 
{
public:
    char** split(const char* texto, const char* sep, int* count) {
        char* copia = strdup(texto);
        int capacidad = 10;
        char** resultados = (char**)malloc(sizeof(char*) * capacidad);
        int n = 0;

        char* token = strstr(copia, sep);
        char* inicio = copia;
        int len_sep = strlen(sep);

        while (token != NULL) {
            int len = token - inicio;
            char* parte = (char*)malloc(len + 1);
            strncpy(parte, inicio, len);
            parte[len] = '\0';
            if (n >= capacidad) {
                capacidad *= 2;
                resultados = (char**)realloc(resultados, sizeof(char*) * capacidad);
            }
            resultados[n++] = parte;
            inicio = token + len_sep;
            token = strstr(inicio, sep);
        }

        char* parte = strdup(inicio);
        if (n >= capacidad) {
            capacidad *= 2;
            resultados = (char**)realloc(resultados, sizeof(char*) * capacidad);
        }
        resultados[n++] = parte;

        *count = n;
        free(copia);
        return resultados;
    }

    void free_split(char** array, int count) {
        for (int i = 0; i < count; i++)
            free(array[i]);
        free(array);
    }
};

// ================= OPERACIONES DE FECHA =================
class operaciones_compu 
{
public:
    void fechaActual(char* buffer, const char* formato) {
        time_t t = time(NULL);
        struct tm* tm_info = localtime(&t);
        strftime(buffer, 20, formato, tm_info);
    }
};

// ================= CLASE TEX_BAS (BASE DE DATOS) =================
class tex_bas : public operaciones_textos, public operaciones_compu
{
public:
    void crearDirectorioSiNoExiste(const char* rutaArchivo) {
        char ruta[512];
        strcpy(ruta, rutaArchivo);
        char* ultimoBackslash = strrchr(ruta, '\\');
        if (ultimoBackslash) {
            *ultimoBackslash = '\0'; // solo queda la carpeta
            int res = _mkdir(ruta);
            if (res == 0) {
                printf("Directorio creado: %s\n", ruta);
            } else if (res == -1 && errno != EEXIST) {
                printf("ERROR: No se pudo crear directorio %s\n", ruta);
                exit(1);
            }
        }
    }

    void inicializarArchivos() {
        for (int i = 0; i < var_fun_GG::G_cantidad_archivos; i++) {
            crearDirectorioSiNoExiste(var_fun_GG::G_archivos[i][0]);
            FILE* f = fopen(var_fun_GG::G_archivos[i][0], "r");
            if (!f) {
                printf("Archivo no existe. Creando: %s\n", var_fun_GG::G_archivos[i][0]);
                f = fopen(var_fun_GG::G_archivos[i][0], "w");
                if (!f) {
                    printf("ERROR: No se pudo crear el archivo %s\n", var_fun_GG::G_archivos[i][0]);
                    exit(1);
                }
                fprintf(f, "%s\n", var_fun_GG::G_archivos[i][1]); // nombres de columnas
                fclose(f);
            } else {
                fclose(f);
            }
        }
    }

    int leerInventario(char inventario[][COLUMNAS][256], int maxProductos) {
        FILE* f = fopen(var_fun_GG::G_archivos[0][0], "r");
        if (!f) return 0;
        char linea[MAX_LINEA];
        int i = 0;

        // leer primera línea de columnas y descartarla
        fgets(linea, MAX_LINEA, f);

        while (fgets(linea, MAX_LINEA, f) && i < maxProductos) {
            linea[strcspn(linea, "\n")] = 0;
            int n;
            char** partes = split(linea, var_fun_GG::G_caracter_separacion[0], &n);
            for (int j = 0; j < COLUMNAS; j++) {
                if (j < n) strcpy(inventario[i][j], partes[j]);
                else strcpy(inventario[i][j], "0");
            }
            free_split(partes, n);
            i++;
        }
        fclose(f);
        return i;
    }

    void guardarInventario(char inventario[][COLUMNAS][256], int n) {
        FILE* f = fopen(var_fun_GG::G_archivos[0][0], "w");
        if (!f) {
            printf("ERROR: No se pudo guardar el inventario\n");
            return;
        }
        fprintf(f, "%s\n", var_fun_GG::G_archivos[0][1]); // primera línea: nombres columnas

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                fprintf(f, "%s", inventario[i][j]);
                if (j < COLUMNAS - 1) fprintf(f, "%s", var_fun_GG::G_caracter_separacion[0]);
            }
            fprintf(f, "\n");
        }
        fclose(f);
    }

    int editar_celda(char inventario[][COLUMNAS][256], int fila, int columna, const char* valor) {
        strcpy(inventario[fila][columna], valor);
        guardarInventario(inventario, fila + 1);
        return 1;
    }

    int incrementar_celda(char inventario[][COLUMNAS][256], int fila, int columna, int cantidad) {
        int val = atoi(inventario[fila][columna]);
        val += cantidad;
        sprintf(inventario[fila][columna], "%d", val);
        guardarInventario(inventario, fila + 1);
        return val;
    }

    void agregar_fila(char inventario[][COLUMNAS][256], char fila[][256], int& n) {
        for (int j = 0; j < COLUMNAS; j++) strcpy(inventario[n][j], fila[j]);
        n++;
        guardarInventario(inventario, n);
    }
};

// ================= CLASE OPERACIONES_TIENDA (CONTROLADOR) =================
class operaciones_tienda : public tex_bas
{
public:
    int buscarProducto(char inventario[][COLUMNAS][256], int n, const char* codigo) {
        for (int i = 0; i < n; i++) {
            if (strcmp(inventario[i][5], codigo) == 0 || strcmp(inventario[i][12], codigo) == 0)
                return i;
        }
        return -1;
    }

    int venta(char* codigo, int cantidad, char* sucursal) {
        char inventario[MAX_PRODUCTOS][COLUMNAS][256];
        int n = leerInventario(inventario, MAX_PRODUCTOS);
        int idx = buscarProducto(inventario, n, codigo);
        if (idx == -1) return 0;

        int stock = atoi(inventario[idx][6]);
        int cantPaquete = atoi(inventario[idx][10]);
        int esPaquete = atoi(inventario[idx][11]);
        int mov = (esPaquete == 1) ? cantPaquete * cantidad : cantidad;

        if (stock < mov) return 0;

        stock -= mov;
        sprintf(inventario[idx][6], "%d", stock);
        char fecha[20];
        fechaActual(fecha, "%Y-%m-%d");
        strcpy(inventario[idx][18], fecha);

        guardarInventario(inventario, n);

        char registro[512];
        sprintf(registro, "%s%s%d%s%s%s%s", codigo, var_fun_GG::G_caracter_separacion[0], cantidad,
                var_fun_GG::G_caracter_separacion[0], sucursal,
                var_fun_GG::G_caracter_separacion[0], fecha);
        registrarMovimiento("ventas", registro);

        return 1;
    }

    int compra(char* codigo, int cantidad, char* proveedor) {
        char inventario[MAX_PRODUCTOS][COLUMNAS][256];
        int n = leerInventario(inventario, MAX_PRODUCTOS);
        int idx = buscarProducto(inventario, n, codigo);
        if (idx == -1) return 0;

        int stock = atoi(inventario[idx][6]);
        int cantPaquete = atoi(inventario[idx][10]);
        int esPaquete = atoi(inventario[idx][11]);
        int mov = (esPaquete == 1) ? cantPaquete * cantidad : cantidad;

        stock += mov;
        sprintf(inventario[idx][6], "%d", stock);
        char fecha[20];
        fechaActual(fecha, "%Y-%m-%d");
        strcpy(inventario[idx][18], fecha);

        guardarInventario(inventario, n);

        char registro[512];
        sprintf(registro, "%s%s%d%s%s%s%s", codigo, var_fun_GG::G_caracter_separacion[0], cantidad,
                var_fun_GG::G_caracter_separacion[0], proveedor,
                var_fun_GG::G_caracter_separacion[0], fecha);
        registrarMovimiento("compras", registro);

        return 1;
    }

    int editar_precio(char* codigo, char* precio, char* proveedor) {
        char inventario[MAX_PRODUCTOS][COLUMNAS][256];
        int n = leerInventario(inventario, MAX_PRODUCTOS);
        int idx = buscarProducto(inventario, n, codigo);
        if (idx == -1) return 0;

        strcpy(inventario[idx][4], precio);
        strcpy(inventario[idx][8], proveedor);
        char fecha[20];
        fechaActual(fecha, "%Y-%m-%d");
        strcpy(inventario[idx][18], fecha);

        guardarInventario(inventario, n);
        return 1;
    }

    void agregar_producto(char* id, char* producto, char* contenido, char* tipo_medida,
                          char* precio_venta, char* cod_barras, char* cantidad,
                          char* costo_comp, char* proveedor) {
        char inventario[MAX_PRODUCTOS][COLUMNAS][256];
        int n = leerInventario(inventario, MAX_PRODUCTOS);

        char fila[COLUMNAS][256];
        for (int i = 0; i < COLUMNAS; i++) strcpy(fila[i], "0");

        strcpy(fila[0], id); strcpy(fila[1], producto); strcpy(fila[2], contenido);
        strcpy(fila[3], tipo_medida); strcpy(fila[4], precio_venta); strcpy(fila[5], cod_barras);
        strcpy(fila[6], cantidad); strcpy(fila[7], costo_comp); strcpy(fila[8], proveedor);
        char fecha[20];
        fechaActual(fecha, "%Y-%m-%d");
        strcpy(fila[18], fecha);

        agregar_fila(inventario, fila, n);
    }

    void registrarMovimiento(const char* tipo, const char* registro) {
        char dia[20], mes[20], anio[20];
        fechaActual(dia, "%Y%m%d");
        fechaActual(mes, "%Y%m");
        fechaActual(anio, "%Y");

        char archivo[256];
        FILE* f;

        sprintf(archivo, "%s_dia_%s.txt", tipo, dia);
        f = fopen(archivo, "a"); fprintf(f, "%s\n", registro); fclose(f);

        sprintf(archivo, "%s_mes_%s.txt", tipo, mes);
        f = fopen(archivo, "a"); fprintf(f, "%s\n", registro); fclose(f);

        sprintf(archivo, "%s_anio_%s.txt", tipo, anio);
        f = fopen(archivo, "a"); fprintf(f, "%s\n", registro); fclose(f);
    }
};

// ================= MENU PRINCIPAL (VISTA) =================
int main() {
    operaciones_tienda tienda;
    tienda.inicializarArchivos();

    int opcion;
    do {
        printf("\n--- POS TIENDA ---\n");
        printf("1. Venta\n2. Compra\n3. Agregar producto\n4. Editar precio\n0. Salir\nOpcion: ");
        scanf("%d", &opcion);
        getchar();

        if (opcion == 1) {
            char codigo[50], sucursal[50]; int cantidad;
            printf("Codigo de barras: "); scanf("%s", codigo);
            printf("Cantidad: "); scanf("%d", &cantidad);
            printf("Sucursal: "); scanf("%s", sucursal);
            if (tienda.venta(codigo, cantidad, sucursal)) printf("Venta realizada\n");
            else printf("Stock insuficiente o codigo invalido\n");
        }
        else if (opcion == 2) {
            char codigo[50], proveedor[50]; int cantidad;
            printf("Codigo de barras: "); scanf("%s", codigo);
            printf("Cantidad: "); scanf("%d", &cantidad);
            printf("Proveedor: "); scanf("%s", proveedor);
            if (tienda.compra(codigo, cantidad, proveedor)) printf("Compra realizada\n");
            else printf("Codigo invalido\n");
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
            tienda.agregar_producto(id, producto, contenido, tipo_medida, precio_venta, cod_barras, cantidad, costo_comp, proveedor);
            printf("Producto agregado\n");
        }
        else if (opcion == 4) {
            char codigo[50], precio[50], proveedor[50];
            printf("Codigo de barras: "); scanf("%s", codigo);
            printf("Nuevo precio: "); scanf("%s", precio);
            printf("Proveedor: "); scanf("%s", proveedor);
            if (tienda.editar_precio(codigo, precio, proveedor)) printf("Precio editado\n");
            else printf("Codigo invalido\n");
        }
    } while (opcion != 0);

    return 0;
}
