#include <iostream>
#include <string>
using namespace std;

#define NUMERO_DE_INSTRUCOES 14
#define NUMERO_DE_DIRETIVAS 5
#define NOME_DE_OPERACAO 7 // A operação com o maior nome tem 6 caracteres, porem coloquei + 1 pra encerrar a string caso necessário;
#define TAMANHO_DE_VARIAVEL 50
//#define NOME_DO_ARQUIVO 50

//////////////////////////////////////////////////////////
//Prototipo das Funcoes

void preencheOperacoes (struct operacoes_info operacoes[])
void preencheDiretivas (struct diretivas_info diretivas[])

//////////////////////////////////////////////////////////
// Estruturas

struct operacoes_info
{
    string nome;
    int numero_de_operandos, opcode, tamanho;
}

struct diretivas_info
{
    string nome;
    int numero_de_operandos, tamanho;
}



/////////////////////////////////////////////////////////
// Codigo Principal

int main(int argc, char *argv[]){

    string nome_arquivo_original, nome_geral_arquivos; //nome_arquivo_original contem a extensão '.asm', o nome_geral_arquivos nao possui a extensao;
    string extensao_original = ".asm", extensao_processado = ".pre", extensao_objeto = ".obj";
    struct operacoes_info operacoes[NUMERO_DE_INSTRUCOES];
    struct diretivas_info diretivas[NUMERO_DE_DIRETIVAS];

    if(argc == 2){
        nome_arquivo_original = argv[1];
        cout << "Nome do arquivo original: ";
        cout << nome_arquivo_original << endl;

        size_t found = nome_arquivo_original.find(extensao_original);
        if(found!=string::npos){
            //cout << "Extensao Do Arquivo Original: " << found << endl;
            //cout << nome_arquivo_original[found] << endl;
            nome_geral_arquivos = nome_arquivo_original.substr(0,found);
            //cout << nome_geral_arquivos << endl;
        }else{
            cout << "Erro! Arquivo com extensao invalida!\n" << endl;
            return 0;
        }
    }
    else
    {
        cout << "Erro! Quantidade de argumento invalida na linha de comando!\n" << endl;
        return 0;
    }

    preencheOperacoes(operacoes_info operacoes[]);

    return 0;
}

////////////////////////////////////////////////////////////////
//Functions


void preencheOperacoes (struct operacoes_info operacoes[])
{
    string ADD ("ADD");
    operacoes[0].nome = ADD;
    string SUB ("SUB");
    operacoes[1].nome = SUB;
    string MULT = ("MULT");
    operacoes[2].nome = MULTI;
    string Divisao = ("DIV");
    operacoes[3].nome = DIV;
    string JMP ("JMP");
    operacoes[4].nome = JMP;
    string JMPN ("JMPN");
    operacoes[5].nome = JMPN;
    string JMPP ("JMPP");
    operacoes[6].nome = JMPP;
    string JMPZ ("JMPZ");
    operacoes[7].nome = JMPZ;
    string COPY ("COPY");
    operacoes[8].nome = COPY;
    string LOAD ("LOAD");
    operacoes[9].nome = LOAD;
    string STORE ("STORE");
    operacoes[10].nome = STORE;
    string INPUT ("INPUT");
    operacoes[11].nome = INPUT;
    string OUTPUT ("OUTPUT");
    operacoes[12].nome = OUTPUT;
    string STOP ("STOP");
    operacoes[13].nome = STOP;

    operacoes[0].numero_de_operandos = 1;
    operacoes[1].numero_de_operandos = 1;
    operacoes[2].numero_de_operandos = 1;
    operacoes[3].numero_de_operandos = 1;
    operacoes[4].numero_de_operandos = 1;
    operacoes[5].numero_de_operandos = 1;
    operacoes[6].numero_de_operandos = 1;
    operacoes[7].numero_de_operandos = 1;
    operacoes[8].numero_de_operandos = 2;
    operacoes[9].numero_de_operandos = 1;
    operacoes[10].numero_de_operandos = 1;
    operacoes[11].numero_de_operandos = 1;
    operacoes[12].numero_de_operandos = 1;
    operacoes[13].numero_de_operandos = 0;

    operacoes[0].opcode = 1;
    operacoes[1].opcode = 2;
    operacoes[2].opcode = 3;
    operacoes[3].opcode = 4;
    operacoes[4].opcode = 5;
    operacoes[5].opcode = 6;
    operacoes[6].opcode = 7;
    operacoes[7].opcode = 8;
    operacoes[8].opcode = 9;
    operacoes[9].opcode = 10;
    operacoes[10].opcode = 11;
    operacoes[11].opcode = 12;
    operacoes[12].opcode = 13;
    operacoes[13].opcode = 14;

    operacoes[0].tamanho = 2;
    operacoes[1].tamanho = 2;
    operacoes[2].tamanho = 2;
    operacoes[3].tamanho = 2;
    operacoes[4].tamanho = 2;
    operacoes[5].tamanho = 2;
    operacoes[6].tamanho = 2;
    operacoes[7].tamanho = 2;
    operacoes[8].tamanho = 3;
    operacoes[9].tamanho = 2;
    operacoes[10].tamanho = 2;
    operacoes[11].tamanho = 2;
    operacoes[12].tamanho = 2;
    operacoes[13].tamanho = 1;

}

void preencheDiretivas (struct diretivas_info diretivas[])
{
    string SECTION ("SECTION");
    diretivas[0].nome = SECTION;
    string SPACE ("SPACE");
    diretivas[1].nome = SPACE;
    string CONST ("CONST");
    diretivas[2].nome = CONST;
    string EQU ("EQU");
    diretivas[3].nome = EQU;
    string IF ("IF");
    diretivas[4].nome = IF;

    diretivas[0].numero_de_operandos = 1;
    // A quantidade de operandos em SPACE é variado. Vou criar uma Flag ou uma função de verificação;
    diretiva[1].numero_de_operandos = -1;
    //
    diretivas[2].numero_de_operandos = 1;
    diretivas[3].numero_de_operandos = 1;
    diretivas[4].numero_de_operandos = 1;

}
