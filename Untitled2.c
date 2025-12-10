
#include <stdio.h>

typedef struct {
    char titulo[100];
    char autor[50];
    int ano;
} LIVRO;

int main(void){
    char tituloLivro[100], autorLivro[50];
    int anoDesejado;
    LIVRO l;

    FILE *binFile = fopen ("/Users/marcelow/livros.bin", "wb");

    if(binFile!= NULL){

    printf("Entre com o titulo do livro: \n");
    fgets (l.titulo, 100, stdin);

    printf("Entre com o nome do autor: \n");
    fgets (l.autor, 50, stdin);

    printf("Entre com o ano de publicacao: \n");
    scanf("%d", &anoDesejado);

    while(!feof(binFile)){
        if(fwrite(&l, sizeof(LIVRO), 1, binFile)==1){
            if (l.titulo == tituloLivro[100] && l.autor == autorLivro[50] && l.ano== anoDesejado){
                fprintf(binFile, "%s , %s , %d", tituloLivro[100], autorLivro[50], anoDesejado);
            }
        }
    }
    printf("Novos livros gravados com sucesso!");
    fclose (binFile);
    }
    else
        printf("Erro ao abrir livros.bin");

    return 0;

}
