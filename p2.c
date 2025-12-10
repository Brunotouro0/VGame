#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define dimensao 3


int le_matriz(int dimensao){
    int fdimensao, matriz[dimensao][dimensao], linha =0, coluna =0;

    fdimensao = dimensao;

    printf("Entre com a matriz:\n");

    for (linha =0; linha <dimensao; linha++){
        for (coluna =0; coluna <dimensao; coluna++){
            scanf("%d", &matriz[linha][coluna]);
        }
    }
    return matriz;
}

int fsoma_linhas (int dimensao, int matriz[dimensao][dimensao]){
    int linha =0, coluna =0, soma[dimensao]={0}, i=0, resultado;

    for (linha=0; linha<dimensao; linha++){
        for (coluna =0; coluna<dimensao; coluna++){
            soma [linha]+=matriz[linha][coluna]
        }
    }

    for (i=0; i<dimensao; i++){
        if (soma[0]!=soma[i])
            resultado = FALSE;
        else
            resultado = TRUE;

    }
    return resultado;
}

int fsoma_colunas (int dimensao, int matriz[dimensao][dimensao]){
    int linha =0, coluna =0, soma[dimensao]={0}, i=0, resultado;

    for (coluna=0; coluna<dimensao; coluna++){
        for (linha =0; linha<dimensao; linha++){
            soma [coluna]+=matriz[coluna][linha];
        }
    }

    for (i=0; i<dimensao; i++){
        if (soma[0]!=soma[i])
            resultado = FALSE;
        else
            resultado = TRUE;

    }
    return resultado;
}

int main (void){
    int dimensao, matriz[dimensao][dimensao], soma_linhas[dimensao][dimensao], soma_colunas[dimensao][dimensao];

    printf("Dimensao da matriz: ");
    scanf("%d", &dimensao);

    matriz = le_matriz(dimensao);

    soma_linhas = fsoma_linhas(matriz, dimensao);
    soma_colunas = fsoma_colunas (matriz, dimensao);

    if (soma_linhas == soma_colunas)
        printf("A matriz satisfaz a propriedade requisitada.");
    else
        printf("A matriz NAO satisfaz a propriedade requisitada.");



return 0;
}
