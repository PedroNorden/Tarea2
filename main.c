#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Map.h"
#include "stack.h"
#include "list.h"

typedef struct
{
    char nombre[20];
    int puntosHabilidad;
    Map *items;
    int cantItems;
    Stack *funcionesAnteriores;
}Datos;

const char *get_csv_field (char * tmp, int k) {
    int open_mark = 0;
    char* ret=(char*) malloc (100*sizeof(char));
    int ini_i=0, i=0;
    int j=0;
    while(tmp[i+1]!='\0'){
        if(tmp[i]== '\"'){
            open_mark = 1-open_mark;
            if(open_mark) ini_i = i+1;
            i++;
            continue;
        }
        if(open_mark || tmp[i]!= ','){
            if(k==j) ret[i-ini_i] = tmp[i];
            i++;
            continue;
        }
        if(tmp[i]== ','){
            if(k==j) {
               ret[i-ini_i] = 0;
               return ret;
            }
            j++; ini_i = i+1;
        }

        i++;
    }
    if(k==j) {
       ret[i-ini_i] = 0;
       return ret;
    }
    return NULL;
}

int is_equal_string(void *key1, void *key2)
{
    if (strcmp((char *)key1, (char *)key2) == 0)
    {
        return 1;
    }
    return 0;
}

Datos *crearCopia(Datos *Original)
{
    Datos *datos = (Datos *)malloc(sizeof(Datos));

    strcpy(datos->nombre, Original->nombre);
    datos->puntosHabilidad = Original->puntosHabilidad;
    datos->items = createMap(is_equal_string);
    datos->cantItems = Original->cantItems;
    datos->items = Original->items;
    return datos;
}

Datos *crearPerfilJugador()
{
    char nombre[20];
    Datos *datos = (Datos *)malloc(sizeof(Datos));
    printf("Ingrese el nombre del jugador: ");
    scanf("%s", nombre);
    strcpy(datos->nombre, nombre);
    datos->puntosHabilidad = 0;
    datos->items = createMap(is_equal_string);
    datos->cantItems = 0;
    datos->funcionesAnteriores = stack_create();
    Datos *copia = crearCopia(datos);
    stack_push(datos->funcionesAnteriores, copia);
    printf("Perfil creado con exito\n");
    return datos;
}

void mostrarPerfilJugador(Map *jugadores, char *nombre)
{
    Datos *datos = searchMap(jugadores, nombre);
    if(datos == NULL)
    {
        printf("El jugador no existe\n");
        return;
    }
    
    printf("Nombre: %s\n", datos->nombre);
    printf("Puntos de habilidad: %d\n", datos->puntosHabilidad);
    printf("Cantidad de items: %d\n", datos->cantItems);
    printf("Items: ");
    char *item = firstMap(datos->items);
    if(item != NULL)
    {
        printf("%s, ", item);
        while((item = nextMap(datos->items)) != NULL)
        {
            printf("%s ", item);
        }
    }
    else
    {
        printf("El jugador no tiene items\n");
    }
    printf("\n");
}

void agregarItem(Map *jugadores, char *nombre)
{
    Datos *datos = searchMap(jugadores, nombre);
    if(datos == NULL)
    {
        printf("El jugador no existe\n");
        return;
    }
    printf("Ingrese el nombre del item: ");
    char *item = malloc(sizeof(char)*20);
    scanf("%s", item);
    if(searchMap(datos->items, item) != NULL)
    {
        printf("El item ya existe\n");
        return;
    }
    Datos *copia = crearCopia(datos);
    stack_push(datos->funcionesAnteriores, copia);
    insertMap(datos->items, item, item);
    datos->cantItems++;
}

void eliminarItem(Map *jugadores, char *nombre)
{
    Datos *datos = searchMap(jugadores, nombre);
    if (datos == NULL)
    {
        printf("El jugador no existe\n");
        return;
    }
    
    if (firstMap(datos->items) != NULL)
    {
        char item[20];
        printf("Ingrese el nombre del item a eliminar: ");
        scanf("%s", item);

        if (searchMap(datos->items, item) != NULL)
        {
            Datos *copia = crearCopia(datos);
            stack_push(datos->funcionesAnteriores, copia);
            eraseMap(datos->items, item);
            datos->cantItems--;
            printf("Item eliminado con exito\n");
        }
        else
        {
            printf("El item no existe\n");
        }


        printf("El item no existe\n");
    }
    else
    {
        printf("El jugador no tiene items\n");
    }
}

void sumarPuntosHabilidad(Map *jugadores, char *nombre)
{
    Datos *datos = searchMap(jugadores, nombre);
    if(datos == NULL)
    {
        printf("El jugador buscado no existe\n");
        return;
    }
    Datos *copia = crearCopia(datos);
    stack_push(datos->funcionesAnteriores, copia);
    int puntos;
    printf("Ingrese la cantidad de puntos de habilidad que desea agregar: ");
    scanf("%d", &puntos);
    datos->puntosHabilidad += puntos;
    printf("Puntos de habilidad agregados con exito\n");
    
}

void mostrarJugadoresConItemsEspecifico(Map *jugadores, char *item)
{
    if(firstMap(jugadores) != NULL)
    {
        char *nombre = firstMap(jugadores);
        while(nombre != NULL)
        {
            Datos *datos = searchMap(jugadores, nombre);
            if(searchMap(datos->items, item) != NULL)
            {
                printf("%s, ", nombre);
            }
            nombre = nextMap(jugadores);
        }
        printf("\n");
    }
    else
    {
        printf("No hay jugadores\n");
    }
}
void deshacerAccion(Map *jugadores, char *nombre)
{
    Datos *datos = searchMap(jugadores, nombre);
    if(datos == NULL)
    {
        printf("El jugador no existe\n");
        return;
    }
    Datos *datosAnteriores = stack_pop(datos->funcionesAnteriores);
    
    if(datosAnteriores == NULL)
    {
        printf("No hay acciones para deshacer\n");
        return;
    }
    datos->puntosHabilidad = datosAnteriores->puntosHabilidad;
    datos->cantItems = datosAnteriores->cantItems;
    datos->items = datosAnteriores->items;
    printf("Accion deshecha con exito\n");
}

void importarJugadores(Map *Jugadores)
{
    char archivo[30];
    printf("Ingrese el nombre del archivo: ");
    scanf("%s", archivo);
    strcat(archivo, ".csv");
    printf("\n");
    FILE *fp = fopen(archivo, "r");
    char linea[1024];
    while (fgets(linea, 1023, fp) != NULL)
    {
        Datos *nuevoJugador = (Datos*) malloc(sizeof(Datos));
        const char *aux = get_csv_field(linea, 0);
        strcpy(nuevoJugador->nombre, aux);
        aux = get_csv_field(linea, 1);
        nuevoJugador->puntosHabilidad = atoi(aux);
        aux = get_csv_field(linea, 2);
        nuevoJugador->cantItems = atoi(aux);
        nuevoJugador->items = createMap(is_equal_string);
        for(int i = 0; i < nuevoJugador->cantItems; i++)
        {
            aux = get_csv_field(linea, i+3);
            insertMap(nuevoJugador->items, (void *)aux, (void *)aux);
        }
        insertMap(Jugadores, nuevoJugador->nombre, nuevoJugador);
    }
    fclose(fp);
}
void exportarDatosJugadores(Map *jugadores )
{
    Datos *jugador = firstMap(jugadores);
    char archivo[30];
    printf("Ingrese el nombre del archivo .csv: ");
    scanf("%s", archivo);
    strcat(archivo, ".csv");
    FILE *fp = fopen(archivo,"a");
    while(jugador != NULL){
        fputs(jugador->nombre, fp);
        fputs("," , fp);
        char Puntos[10];
        sprintf(Puntos, "%d", jugador->puntosHabilidad);
        fputs(Puntos, fp);
        fputs("," , fp);
        char stringCantItems[10];
        sprintf(stringCantItems, "%d", jugador->cantItems);
        fputs(stringCantItems, fp);
        char *aux = firstMap(jugador->items);
        while(aux != NULL)
        {
            fputs("," , fp);
            fputs(aux, fp);
            aux = nextMap(jugador->items);
        }
        jugador = nextMap(jugadores);
        fputs("\n",fp);
    }
    printf("Datos exportados con exito\n");
    fclose(fp);
}

int main()
{
    Map *jugadores = createMap(is_equal_string);
    Datos *jugador;
    char nombre[20];
    int opcion = 1;
    while(opcion != 0)
    {
        printf("Menu de acciones:\n");
        printf("1. Crear perfil de jugador\n");
        printf("2. Mostrar perfil de jugador\n");
        printf("3. Agregar item a jugador\n");
        printf("4. Eliminar item de jugador\n");
        printf("5. Agregar puntos de habilidad a jugador\n");
        printf("6. Mostrar jugadores con item especifico\n");
        printf("7. Deshacer ultima accion\n");
        printf("8. Exportar datos de jugadores a archivo de texto\n");
        printf("9. Importar datos de jugadores desde archivo de texto\n");
        printf("0. Salir\n");
        printf("Ingrese la opcion deseada: ");
        scanf("%d", &opcion);
        switch (opcion)
        {
            case 1:
                jugador = crearPerfilJugador();
                insertMap(jugadores, jugador->nombre, jugador);
                break;
            case 2:
                printf("Ingrese el nombre del jugador: ");
                scanf("%s", nombre);
                mostrarPerfilJugador(jugadores, nombre);
                break;
            case 3:
                printf("Ingrese el nombre del jugador: ");
                scanf("%s", nombre);
                agregarItem(jugadores, nombre);
                break;
            case 4:
                printf("Ingrese el nombre del jugador: ");
                scanf("%s", nombre);
                eliminarItem(jugadores, nombre);
                break;
            case 5:
                printf("Ingrese el nombre del jugador: ");
                scanf("%s", nombre);
                sumarPuntosHabilidad(jugadores, nombre);
                break;
            case 6: 
                printf("Ingrese el nombre del item: ");
                scanf("%s", nombre);
                mostrarJugadoresConItemsEspecifico(jugadores, nombre);
                break;
            case 7:
                printf("Ingrese el nombre del jugador: ");
                scanf("%s", nombre);
                deshacerAccion(jugadores, nombre);
                break;
            case 8:
                exportarDatosJugadores(jugadores);
                break;
            case 9:
                importarJugadores(jugadores);
                break;
            case 0:
                printf("Saliendo del programa...\n");
                break;
            default:
                printf("Opcion no valida\n");
                break;
        }

    }
    return 0;
}