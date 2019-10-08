#include <iostream>
#include <string>
using namespace std;

#define NUMERO_DE_INSTRUCOES 14
#define NUMERO_DE_DIRETIVAS 5
#define NOME_DE_OPERACAO 6
#define TAMANHO_DE_VARIAVEL 50
#define NOME_DO_ARQUIVO 50

int main (int argc, char *argv[]){

string nome_arquivo_original;

if(argc == 2){

    nome_arquivo_original = argv[1];
    cout << "Nome do arquivo original: " << endl;
    cout << nome_arquivo_original << endl;

}
else
{

    cout << "Erro! Quantidade de argumento invalida na linha de comando!\n" << endl;

}

return 0;

}
