# MiniTP2 Threads y Semanaforos
## _Sistemas operativos y redes_

El minitp consta de leer la receta para preparar un sandwich desde un archivo: receta.txt y ver quien termine primero.

#### Archivos necesarios para la ejecucion: 
- subwayArgento.c
- receta.txt

#### La receta esta compuesta por:
> cortar|ajo 1|ajo 2|puñado de rejeil
> mezclar|ajo|perejil|huevo|carne
> agregarSal|Sal
> cocinar|Milanesa en sarten
> hornearPan|pan
> prepararEnsalada|tomate|lechuga
>armarSandwich|milanesa|pan|ensalada
###### Aclaracion: Considerando que la primera palabra es la accion y el resto del renglon son los ingredientes para dicha accion

***
#### Comentarios
Los 4 equipos comenzaran en el mismo instante y sus acciones son:
cortar -> mezclar -> agregarSal -> cocinar          -> (armarSandwich)
hornear -> prepararEnsalada                         -> (armarSandwich)

Es decir, comenzaran por un lado con la accion cortar luego mezclar, luego agregar sal y cocinar. En el mientras tanto, podran hornear el pan posterior a eso preparar la ensalada y cuando tenga finalizada la accion cocinar y preparar ensalada, recien podran armar el sandwich.


Consideraciones: utensillos unicos para todos los grupos
- El salalero.
- La sarten que se utiliza en cocinar.
- El horno.

## Ejecutar
```sh
gcc subwayArgento.c -o ejecutable -lpthread
./ejecutable
```
