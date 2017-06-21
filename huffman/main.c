
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

struct nodo
{
    struct nodo *der,*izq,*arr;
    int cuenta;
    char bit;
    unsigned char karacter;
    char *codigo;
    char nbits;
}hojas[256],*arbol[256],*MENOR,*SEGUNDO;

int NSIMB=0,nsimb;
FILE *f,*g;
int NBYTES=0;

int preparar_hojas(char *archivo)
{
    int j;
    for(j=0;j<256;++j){
        hojas[j].der=hojas[j].izq=hojas[j].arr=NULL;
        hojas[j].cuenta=0;
        hojas[j].karacter=j;
        hojas[j].codigo=NULL;
    }
    if ((f=fopen(archivo,"rb"))!=NULL){
        while ((j=fgetc(f))!=EOF){
            ++hojas[j].cuenta;
            ++NBYTES;
        }
        fclose(f);
    }
    else
    {
        return(1);
    }
    for(j=0;j<256;++j){
        if (hojas[j].cuenta!=0)
            ++NSIMB;
    }
    nsimb=NSIMB;
    return(0);
}

void preparar_arbol()
{
    int j;
    for(j=0;j<256;++j){
        arbol[j]=&(hojas[j]);
    }
    return;
}


void costruirArbol()
{
    int menor=-1;
    int segundo=-1;
    int temporal;
    int j;
    struct nodo *P;

    if (nsimb==1) return;

    /* buscar menor valor */
    for(j=0;j<256;++j){
        if (arbol[j]==NULL) continue;
        if (arbol[j]->cuenta==0) continue;
        if (menor==-1){
            menor=j;
            temporal=arbol[j]->cuenta;
        } else
        {
            if (arbol[j]->cuenta<temporal){
                menor=j;
                temporal=arbol[j]->cuenta;
            }
        }
    }

    /* buscar segundo menor */
    for(j=0;j<256;++j){
        if (arbol[j]==NULL) continue;
        if (arbol[j]->cuenta==0) continue;
        if (j==menor) continue;
        if (segundo==-1){
            segundo=j;
            temporal=arbol[j]->cuenta;
        } else
        {
            if (arbol[j]->cuenta<temporal){
                segundo=j;
                temporal=arbol[j]->cuenta;
            }
        }
    }

    /* costruirArbol un nuevo nodo */
    P=(struct nodo *)malloc(sizeof(struct nodo));
    arbol[menor]->arr=P;
    arbol[segundo]->arr=P;
    P->izq=arbol[menor];
    P->der=arbol[segundo];
    P->arr=NULL;
    arbol[menor]->bit=0;
    arbol[segundo]->bit=1;
    P->cuenta=arbol[menor]->cuenta+arbol[segundo]->cuenta;
    arbol[menor]=NULL;
    arbol[segundo]=P;
    --nsimb;

    /* sigue tejiendo hasta que sĂłlo quede un nodo */
    costruirArbol();
}

void codificar()
{
    char pila[64];
    char tope;
    int j;
    char *w;
    struct nodo *P;
    for(j=0;j<256;++j){
        if (hojas[j].cuenta==0) continue;
        P=(struct nodo *)(&(hojas[j]));
        tope=0;
        while (P->arr!=NULL){
            pila[tope]=P->bit;
            ++tope;
            P=P->arr;
        }
        hojas[j].nbits=tope;
        hojas[j].codigo=(char *)malloc((tope+1)*sizeof(char));
        w=hojas[j].codigo;
        --tope;
        while (tope>-1){
            *w=pila[tope];
            --tope;
            ++w;
        }
        *w=2;
    }
    return;
}


int escribe_cabecera(char *destino)
{
    int j,k;
    FILE *g;

    char *p=(char *)(&NBYTES);
    if ((g=fopen(destino,"wb"))==NULL) return(1);
    for(j=0;j<4;++j){
        fputc(*p,g);
        ++p;
    }

    p=(char *)(&NSIMB);
    fputc(*p,g);

    for(j=0;j<256;++j){
        if (hojas[j].cuenta==0) continue;
        fputc(j,g);
        p=(char *)(&(hojas[j].cuenta));
        for(k=0;k<4;++k){
            fputc(*p,g);
            ++p;
        }
    }
    fclose(g);
    return(0);
}


int comprimir(char *origen, char *destino)
{
    system("cls");
    system ("color f2");
    printf("--------Comprimiendo-------\n");
    unsigned char d=0;
    int x;
    char nbit=0;
    char *p;

    if ((f=fopen(origen,"rb"))==NULL) return(1);
    if ((g=fopen(destino,"ab"))==NULL) return(2); /* ya esta la cabecera */
    int cont = 0;

    while ((x=fgetc(f))!=EOF){
        if(cont % 150000 == 0)
        {
            printf("--------Comprimiendo-------\n");
        }
        cont++;
        p=hojas[x].codigo;
        while (*p!=2){
            if (nbit==8){
                nbit=0;
                fputc(d,g);
                d=0;
            } else
            {
                if (*p==1){
                    d|=(1<<nbit);
                }
                ++nbit;
                ++p;
            }
        }
    }
    fputc(d,g);
    fclose(f);
    fclose(g);
    return(0);
}

int descomprimir(char *origen, char *destino)
{
    char *p;
    int j,k,n,m;
    unsigned char x,nbit;
    struct nodo *P,*Q;

    if ((g=fopen(origen,"rb"))==NULL) return(1);
    if ((f=fopen(destino,"wb"))==NULL) return(2);

    /* leer NBYTES */
    p=(char *)(&n);
    for(j=0;j<4;++j){
        *p=(unsigned char)fgetc(g);
        ++p;
    }
    NBYTES=n;

    /* leer NSIMB */
    NSIMB=nsimb=fgetc(g);

    /* preparar las hojas */
    for(j=0;j<256;++j){
        hojas[j].cuenta=0;
        hojas[j].izq=hojas[j].der=hojas[j].arr=NULL;
        hojas[j].karacter=j;
    }
    for(j=0;j<NSIMB;++j){
        n=fgetc(g);
        p=(char *)(&m);
        for(k=0;k<4;++k){
            *p=(unsigned char)fgetc(g);
            ++p;
        }
        hojas[n].cuenta=m;
    }

    /* construyo el ĂĄrbol */
    preparar_arbol();
    costruirArbol();

    /* apunto a la raĂ­z del ĂĄrbol */
    j=0;
    while (hojas[j].cuenta==0) ++j;
    P=(struct nodo *)(&(hojas[j]));
    while (P->arr!=NULL) P=P->arr;

    /* ahora ya se puede descomprimir */
    j=0;
    x=fgetc(g);
    nbit=0;
    Q=P;
    while(j<NBYTES){
        if (Q->izq==NULL){
            fputc(Q->karacter,f);
            Q=P;
            ++j;
        } else
        if (nbit==8){
            x=fgetc(g);
            nbit=0;
        } else
        {
            if (x&(1<<nbit)){
                Q=Q->der;
            }
            else
            {
                Q=Q->izq;
            }
            ++nbit;
        }
    }
    fclose(f);
    fclose(g);
    return(0);
}

main(int argc, char *argv[])
{
    int opc = 0;
    do{
        system("cls");
        system ("color f5");
        printf("Ingrese 1 si quiere comprimir un archivo\n");
        printf("Ingrese 2 si quiere descomprimir un archivo\n");
        printf("Ingrese 3 para Salir\n");
        printf("Ingrese su opcion: ");
        scanf("%i" , &opc);
        switch(opc)
        {
        case 1:
            //codigo
            system("cls");
            system ("color f2");
            printf("Ingrese el nombre del archivo: ");
            char nombre[25];
            char nombreaux[25] = "";
            fflush(stdin);
            scanf("%s" , nombre);
            printf("------------------------------------------------------------------\n");
            fflush(stdin);
            FILE *fp;
            strcat(nombreaux , nombre);
            strcat(nombreaux , ".huffman");
            strcat(nombre , ".txt");

            fp = fopen(nombre , "r");
            if(fp == NULL)
            {
                printf("El archivo no existe");
                fclose(fp);
                system("Pause");
            }
            else
            {
                printf("\n--------Comprimiendo-------\n");
                fclose(fp);
                clock_t start = clock();
                preparar_hojas(nombre);
                preparar_arbol();
                costruirArbol();
                codificar();
                escribe_cabecera(nombreaux);
                comprimir(nombre , nombreaux);
                system("cls");
                system ("color f2");

                printf("Archivo Comprimido\n");
                printf("Tiempo transcurrido: %f", ((double)clock() - start) / CLOCKS_PER_SEC);
                printf("\n");
                system("Pause");
            }
            break;
        case 2:
            system("cls");
            system ("color f2");
            printf("Ingrese el nombre del archivo: ");
            char nombre2[25];
            char nombreaux2[25] = "";
            FILE *fp2;
            fflush(stdin);
            scanf("%s" , nombre2);
            fflush(stdin);
            strcat(nombreaux2 , nombre2);
            strcat(nombreaux2 , ".huffman");
            strcat(nombre2 , ".txt");

            fp2 = fopen(nombreaux2 , "r");
            if(fp2 == NULL)
            {
                printf("El archivo no existe\n");
                fclose(fp2);

                system("Pause");
            }
            else
            {
                fclose(fp2);
                clock_t start2 = clock();
                descomprimir(nombreaux2 , nombre2);
                printf("Tiempo transcurrido: %f", ((double)clock() - start2) / CLOCKS_PER_SEC);
                printf("\n");

                printf("Archivo descomprimido\n");
                system("Pause");
            }
            break;
        case 3:
            printf("----------Saliendo---------\n");
            break;
        default:
            printf("Ingreso una opcion incorrecta\n");
            system("Pause");
            break;
        }
    }while(opc != 3);





}
