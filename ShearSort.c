#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include "Algorithms.h"
#include <pthread.h>

// indices para recorrer la matriz
int i,j;
// arreglo dinamico de elementos
int **elements;
// numero de threads a usar
int threadsNumber = 0;
// numero de filas de la matriz
int rowsNumber = 0;
// numero de columnas de la matriz
int columnsNumber = 0;  
// numero de elementos de la matriz
int elementsNumber = 0;
// indices para recorrer la matriz
int row = 0, column = 0;
// numero de iteraciones necesarias para ordenar los N² elementos
int iterations = 0;
// la fase en la que se encontrarà el ordenamiento.
int currentFase = 0;
// barrera para sincronizar los threads
pthread_barrier_t barrier;

int IsPower(double);
int IsEven(int value);
void SortRow(int rowIndex, int sortingMode);
void SortColumn(int columnIndex);
void Print(int **elements);
void ParseArguments(int argumentsCounter, char *arguments[]);
void ReadInitialConfiguration();
void AllocateMemoryFor(int size);
void *Sort(void *index);
void PrintColumn(int columnIndex);
void PrintList(int **elements);

main(int argc, char *argv[])
{
   // Indice para recorrer el arreglo de threads
   int currentThread = 0;
   // Almacena el codigo de retorno de las llamadas a las funciones de pthread
   int returnCode = 0;
   // Leer y analizar los parametros enviados por la linea de comandos.
   ParseArguments(argc, argv);
   // Leer el nùmero de filas y columnas y cada uno de los N^2 elementos.
   ReadInitialConfiguration();
   // Arreglo de Threads
   //printf("%d ", IsEven(1));
	//printf("%d", IsEven(2));
   pthread_t *threads = malloc(sizeof(pthread_t) * (threadsNumber + (elementsNumber % threadsNumber)));
   printf("\tConfiguracion Inicial\n\n");
   Print(elements);
   
   // Calcular el numero de iteraciones necesarias.
   iterations = ceil(1 + 2 * (log10(elementsNumber) / log10(2)));
   
   // Iniciar la barrera de los threads.
   if (pthread_barrier_init(&barrier, NULL, threadsNumber + 1)) {
      printf("\tHa ocurrido un error inicializando la barrera\n");
      exit(EXIT_FAILURE);
   }
    
   // Crear cada uno de los threads necesarios. 
   for(currentThread = 0; currentThread < threadsNumber; currentThread++)
   {
      returnCode = pthread_create(&threads[currentThread], NULL, Sort, (void *)currentThread);
      if (returnCode) {
         printf("\tHa ocurrido un error al crear el thread %d. Codigo de error: %d\n", currentThread, returnCode);
         exit(EXIT_FAILURE);
      }                     
   }
   currentFase = 0;
   // Esperar a que cada barrera haya sido liberada.
   while(currentFase < iterations)
   {
      returnCode = pthread_barrier_wait(&barrier);
      if (returnCode != 0 && returnCode != PTHREAD_BARRIER_SERIAL_THREAD) {
         printf("\tSe ha producido un error esperando en la barrera\n");
         exit(EXIT_FAILURE);
      }
      //printf("\n\tFase %d finalizada\n",currentFase + 1);
      Print(elements);
      currentFase++;
   }
   // Imprimir la configuraciòn final de la matriz.
   printf("\n\n\tConfiguracion final(como matriz):\n");
   Print(elements);   
   printf("\n\n\tConfiguracion final(como lista):\n");
   PrintList(elements);
   pthread_exit(NULL);   
}

void *Sort(void *index)
{
   int auxIndex = (int)index;
   int current;
   int returnCode;
   
   for(current = 0; current < iterations; current++)
   {
      if(IsEven(current + 1) != 0)
      {
         SortRow(auxIndex, 0);
      }
      else
      {
         SortColumn(auxIndex);
      }                
      returnCode = pthread_barrier_wait(&barrier);
      if (returnCode != 0 && returnCode != PTHREAD_BARRIER_SERIAL_THREAD) {
         printf("\tSe ha producido un error esperando en la barrera\n");
         exit(EXIT_FAILURE);
      } 
   }     
   pthread_exit(NULL);
}

// permite ordenar una columna de la matriz de valores de forma ascendente.
void SortColumn(int columnIndex)
{
     // Los indices usaoos para recorrer la columna.
     int i,j;
     // Reservar suficiente espacio para un arreglo de enteros.
     int *auxColumn = malloc(sizeof(int) * elementsNumber);
     // Copiar cada elemento de la columna al nuevo arreglo
     for(i = 0; i < elementsNumber; i++)
        auxColumn[i] = elements[i][columnIndex];
     // Ordenar la columna usando InsertSort
     insertionSort(auxColumn, elementsNumber);
     // Insertar nuevamente los valores de la columna ordenada en sus posiciones originales.
     for(i = 0; i < elementsNumber; i++)
        elements[i][columnIndex] = auxColumn[i];
}

// Permite ordenar una fila de la matriz de elementos.
void SortRow(int rowIndex, int sortingMode)
{
   // Los indices usados para recorrer la fila.
   int i,j;
   // Reservar suficiente espacio para un nuevo arreglo de enteros.
   int *auxRow = malloc(sizeof(int) * elementsNumber);
   for(i = 0;i < elementsNumber; i++)
       auxRow[i] = elements[rowIndex][i];
 
   // Ordenar la fila usando InsertSort
   insertionSort(auxRow, elementsNumber);
   // Verificar si la fila es fila par o impar
   if(IsEven(rowIndex+1) == 0)
   { 
for(i = 0, j = elementsNumber - 1;i < elementsNumber && j >= 0; i++,j--)
         elements[rowIndex][i] = auxRow[j]; 
              
   }
   else{
     for(i = 0;i < elementsNumber; i++)
         elements[rowIndex][i] = auxRow[i];       
    }
}

// Permite imprimir la matriz de elementos.
void Print(int **elements)
{
   // Los indices usados para recorrer la matriz.
   int i,j;
   // Recorremos las filas
   for(i = 0; i < elementsNumber; i++)
   {
      printf("\t");	
      // Recorremos las columnas   
      for(j = 0; j < elementsNumber; j++)
         printf("%d ", elements[i][j]);
      printf("\n");   
   } 
//printf("dentro de la impresion");
}

// Permite leer la configuracion inicial: el tamaño de la matriz y los N^2 valores.
void ReadInitialConfiguration()
{
   int i, j;
   // Leer el nùmero de elementos.
   scanf("%d", &elementsNumber);
   if(!IsPower((double)elementsNumber))
   {
      printf("\tEl valor ingresado no corresponde a una potencia de 2.");
      exit(EXIT_FAILURE);                  
   }
   // Reservar espacio suficiente para el cuadrado de elementos.
   AllocateMemoryFor(elementsNumber);   
   // Leer los numeros de casa posicion de la matriz.
   for(i = 0; i < elementsNumber; i++)
   {
      for(j = 0; j < elementsNumber; j++)
	{
         scanf("%d", &elements[i][j]);
	}
   }
}

// permite reservar memoria para una matriz cuadrada de tantos elementos como lo indica el parametro.
// size: Indica el tamaño del espacio a reservar.
void AllocateMemoryFor(int size)
{
   int i, j;
   // Reservar espacio para las filas
   elements = malloc(sizeof(int *) * size);   
   for(i = 0; i < size; i++)
   {
      // reservar espacio para las columnas.
      elements[i] = malloc(sizeof(int) * size);
   }
//printf("reservando memoria"); 
}

// Permite leer los parametros de la linea de comandos pasados como argumentos.
void ParseArguments(int argumentsCounter, char *arguments[])
{
   
   if(argumentsCounter != 2)
   {
      printf("\tUso: ShearSort NivelDeConcurrencia\n");
      exit(EXIT_FAILURE);
   }
   threadsNumber = atoi(arguments[1]); 
   if(threadsNumber <= 0)
   {
      printf("\tEl nivel de concurrencia no puede ser un valor negativo.\n");
      exit(EXIT_FAILURE);
   }
   
}

// Permite verificar si el valor ingresado es par o impar.
int IsEven(int value)
{
   return (value % 2);    
}

// Permite verificar que el numero de elementos ingresados sea efectivamente una potencia de 2.
int IsPower(double value)
{
    double exponent = log10(value) / log10(2);
    return pow(2,exponent) == value; 
}
// Permite imprimir una lista de los valores ordenados. 
void PrintList(int **elements)
{
   int row, column;
   int initialValue = elementsNumber;
   printf("\t");
   for(row = 0; row < elementsNumber; row++)
   {
      if(IsEven(row + 1) == 0)
      {
         for(column = elementsNumber - 1; column >= 0; column--)
         {
            printf("%d ", elements[row][column]);
         }
      }
      else
      { 
         for(column = 0; column < elementsNumber; column++)
         {
            printf("%d ", elements[row][column]);
         }
      }
   }
   printf("\n");
}
