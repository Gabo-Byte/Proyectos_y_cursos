#include<stdio.h>

typedef struct{
    int id;    
    char nombre[50];
    float precio;
    int cantidad;

}Producto;

void agregrarProducto(Producto inventario[], int *total){
    int pos = *total; //posicion actual señalada por puntero
    printf("Ingresa informacion del producto\n\n");
   
       
    printf("Id: \n");
    scanf("%d", &inventario[pos].id);

    printf("Nombre: \n");
    scanf("%s", inventario[pos].nombre);
    
    printf("Precio: \n");
    scanf("%f", &inventario[pos].precio);
    
    printf("Cantidad: \n");
    scanf("%d", &inventario[pos].cantidad);

    (*total)++; // Incrementa el contador original fuera de la función
}

void mostrarProductos(const Producto inventario[], int total){
    if(total <= 0){
        printf("Inventario vacio\n");
        return;
    }

    for(int i = 0; i < total; i++){
        printf("%d %s $%.2f %d\n", inventario[i].cantidad, inventario[i].nombre, inventario[i].precio, inventario[i].cantidad);
    }
}

void ActualizarStock(Producto *prod, int nuevaCantidad){
    if(prod != NULL){
        prod->cantidad = nuevaCantidad;
    }
}

void aplicarDescuentoGeneral(Producto inventario[], int total, float porcentaje){

    if(porcentaje < 0.0f || porcentaje > 100.0f){
        printf("Porcentaje invalido\n");
        return;
    }

    for(int i = 0; i < total; i++){
        inventario[i].precio -= inventario[i].precio * (porcentaje/100.0f);
    }
}

int main(){

Producto inventario[50];
int total = 0;
int opc;

do{
    printf("Menu de la tienda...\n");
    printf("1-Agregar producto\n");
    printf("2-Mostrar inventario\n");
    printf("3-Modificar stock de un producto por ID\n");
    printf("4-Aplicar descuento general\n");
    printf("5-Salir\n");
    scanf("%d", &opc);

    switch(opc){

        case 1:
        agregrarProducto(inventario, &total); //en los struct, se pasan tal cual es nombre
        break;

        case 2:
        mostrarProductos(inventario, total);
        break;

        case 3:{
            int idBuscado, nuevaCantidad, encontrado = 0;
            printf("Ingrese nuevo producto: \n");
            scanf("%d", &idBuscado);

            for(int i = 0; i < total; i++){
                if(inventario[i].id == idBuscado){
                    printf("Ingrese la nueva cantidad para '%s': ", inventario[i].nombre);
                    scanf("%d", &nuevaCantidad);

                    ActualizarStock(&inventario[i], nuevaCantidad);
                    encontrado = 1;
                    break;
                }
            }
            if(!encontrado){
                printf("[!] No se encontró ningún producto con ID %d.\n", idBuscado);
            }
        }
        break;

        case 4:{
            float porcentaje;
            printf("Ingrese descuento por aplicar '/. \n");
            scanf("%f", &porcentaje);
            aplicarDescuentoGeneral(inventario, total, porcentaje);
        }
        break;

        case 5:
        printf("Saliendo...\n");
        break;

        default:
        printf("Error: no existe esa opcion\n");
        break;

    }

}while(opc != 5);


        return 0;
}



    
