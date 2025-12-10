#include <stdio.h>

#define MINIMO 3
#define MAXIMO 9
#define TRUE 1
#define FALSE 0


int linhasSomaIgual (int matriz[MAXIMO][MAXIMO], int dimensao){
    int linha =0, coluna =0, soma[dimensao];

    for(linha=0; linha <dimensao; linha++){
        soma [linha]= 0;
        for (coluna =0; coluna <dimensao; coluna++){
            soma[linha]+=matriz[linha][coluna];
        }
    }

    for (linha=0; linha<dimensao; linha++){
        if (soma[0]!=soma[linha])
            return FALSE;
        else
            return TRUE;
    }
}

int colunasSomaIgual (int matriz[MAXIMO][MAXIMO], int dimensao){
    int linha =0, coluna =0, soma[dimensao];

    for(coluna =0; coluna<dimensao; coluna++){
        soma[coluna]=0;
        for(linha=0; linha<dimensao; linha++){
            soma[coluna]+= matriz[linha][coluna];
        }
    }

    for (coluna =0; coluna <dimensao; coluna++){
        if (soma[0]!=soma[coluna])
            return FALSE;
        else
            return TRUE;
    }
}
void le_matriz (int matriz[MAXIMO][MAXIMO], int dimensao){
    int linha =0, coluna =0, resultados[2];
    printf("Entre com a matriz: \n");
    for (linha =0; linha<dimensao; linha++){
        for(coluna =0; coluna<dimensao; coluna++){
            scanf("%d", &matriz[linha][coluna]);
        }
    }
    resultados[0] = linhasSomaIgual(matriz, dimensao);
    resultados[1] = colunasSomaIgual(matriz, dimensao);

    return resultados;
}




int main(void){
    int dimensao, matriz[MAXIMO][MAXIMO], resultados[2];



    do{
    printf("Entre com a dimensao da matriz: \n");
    scanf("%d", &dimensao);
    }while (dimensao<MINIMO || dimensao>MAXIMO);

    resultados = le_matriz (matriz, dimensao);






}
