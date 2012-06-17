#include<stdio.h>
#include<stdlib.h>
#include<math.h>

void Swap(int *vector, int a, int b)
{
	if (a != b)
	{
		int tmp = vector[a];
		vector[a] = vector[b];
		vector[b] = tmp;
	}
}

void QuickSort(int *vector, int first, int last)
{
	if (first < last)
	{
		/* Selecciona un elemento del vector y coloca los menores
		 que él a su izquierda y los mayores a su derecha */
		int p = Partition(vector, first, last, last);
 
		/* Repite el proceso para cada una de las 
		 particiones generadas en el paso anterior */
		QuickSort(vector, first, p - 1);
		QuickSort(vector, p + 1, last);
	}
}
 
/* Implementación no clásica de la función Pivote. En lugar de
recorrer el vector simultáneamente desde ambos extremos hasta el
cruce de índices, se recorre desde el comienzo hasta el final */
int Partition(int *vector, int first, int last, int pivot)
{
	int p = vector[pivot];
	int j = first;
    int i; 
	// Mueve el pivote a la última posición del vector
	Swap(vector, pivot, last);
 
	/* Recorre el vector moviendo los elementos menores
	 o iguales que el pivote al comienzo del mismo */
	for (i = first; i < last; i++)
	{
		if (vector[i] <= p)
		{
			Swap(vector, i, j);
			j++;
		}
	}
 
	// Mueve el pivote a la posición que le corresponde
	Swap(vector, j, last);
 
	return j;
}
 
void insertionSort(int *number, int size) {
    int in, out;
    //     out is dividing line
    for (out = 1; out < size; out++) {
      int temp = number[out]; // remove marked item
      in = out; // start shifts at out
      while (in > 0 && number[in - 1] >= temp) // until one is smaller,
      {
        number[in] = number[in - 1]; // shift item to right
        --in; // go left one position
      }
      number[in] = temp; // insert marked item
    }
  }

