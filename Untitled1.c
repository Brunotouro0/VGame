/*Suponha um arquivo binário filmes.bin já existente que armazena informações sobre filmes. Você NÃO SABE
quantos filmes estão armazenados no arquivo. Os dados de cada filme são armazenados na seguinte estrutura:
typedef s{
 char diretor[50];
 char titulo[100];
 int ano;
 int duracao; // duracao em minutos
 char genero; // generos possiveis (a-aventura; p-policial; t-terror)
}FILME;
Faça um programa em C que leia este arquivo e solicite ao usuário um estilo de filme (um dos três estilos
possíveis, a, p ou t, faça validação) e um intervalo de anos (p. ex. 1989-2001). Escrever num arquivo filmes.txt
todos os títulos e anos dos filmes que satisfazem as condições lidas (estilo e intervalo de anos), um filme por
linha. Escrever também na tela o título e duração do filme mais longo.*/
