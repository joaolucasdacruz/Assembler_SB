#include <iostream>
#include <string>
#include <fstream>
using namespace std;

#define NUMERO_DE_INSTRUCOES 14
#define NUMERO_DE_DIRETIVAS 5
#define NOME_DE_OPERACAO 7 // A operação com o maior nome tem 6 caracteres, porem coloquei + 1 pra encerrar a string caso necessário;
#define TAMANHO_DE_VARIAVEL 50
#define NUMERO_DE_SIMBOLOS 500

//////////////////////////////////////////////////////////
//Prototipo das Funcoes

void preencheOperacoes (struct operacoes_info operacoes[]);
void preencheDiretivas (struct diretivas_info diretivas[]);
void formataArquivo(ifstream& arquivo_original, fstream& arquivo_formatado);
void linha(fstream& arquivo_formatado, fstream& arquivo_objeto, fstream& arquivo_pre_processado, struct operacoes_info operacoes[], struct diretivas_info diretivas[], struct simbolo_tab Tsimbolos[]);
int procuraInstrucao(struct operacoes_info operacoes[], string mnemonico);
int procuraDiretiva(struct diretivas_info diretivas[], string mnemonico);
int procuraSimbolo(string simbolo, struct simbolo_tab Tsimbolos[]);
int insereSimbolo(struct simbolo_tab Tsimbolos[], string simbolo, string mnemonico, string operando1, string operando2, string vec_op_1, string vec_op_2);
int preProcessamento(fstream& arquivo_pre_processado, string linha);

//////////////////////////////////////////////////////////
// Estruturas

struct operacoes_info
{
    string nome;
    int numero_de_operandos, opcode, tamanho;
};

struct diretivas_info
{
    string nome;
    int numero_de_operandos, tamanho;
};

struct simbolo_tab
{
    string simbolo;
    int valor; // Se a flag-def == 0, o valor é -1; Se a flag-def == 1, o valor é a posição
    int lista[50][2]; //Matrix 50 por 2, onde a primeira coluna contém a posição da label não definida e na segunda o deslocamento de vetor. "N2+1"
    int flag_def = 0; // flag_def estabelece se o simbolo já foi definido; 1 - definido & 0 - não definido;
    int space = 0; //verifica se a diretiva atual é um SPACE
    int space_vec = 0;
    int pos_fim_lista = -1; // fim da lista
};
/////////////////////////////////////////////////////////
// Variáveis Globais
unsigned int text_flag = 0, data_flag = 0;
int contador_de_simbolos = -1; // Quantidade de simbolos dentro da tabela de simbolos;
int contador_de_posicao = -1; //Posição absoluta dentro do arquivo de código (texto);
int contador_de_linha_original = -1; //Posição do contador de linha (contador de programa). Serve para imprimir os erros.
int contador_de_linha_pre = -1; //Posição do contador de linha do programa do arquivo pre processado. Serve para marcar a posição das labels.

/////////////////////////////////////////////////////////
// Codigo Principal

int main(int argc, char *argv[]){

    string nome_arquivo_original, nome_geral_arquivos; //nome_arquivo_original contem a extensão '.asm', o nome_geral_arquivos nao possui a extensao;
    string extensao_original = ".asm", extensao_processado = ".pre", extensao_objeto = ".obj";
    struct operacoes_info operacoes[NUMERO_DE_INSTRUCOES];
    struct diretivas_info diretivas[NUMERO_DE_DIRETIVAS];
    struct simbolo_tab Tsimbolos[NUMERO_DE_SIMBOLOS];
    ifstream arquivo_original;
    fstream arquivo_pre_processado;
    fstream arquivo_objeto;
    fstream arquivo_formatado;

    ////// Iniciando Estruturas.

    for(int i = 0; i < NUMERO_DE_SIMBOLOS;  i++){
        for(int j = 0; j < 50; j++){
            Tsimbolos[i].lista[j][1] = -1;
            Tsimbolos[i].lista[j][2] = -1;
        }
    }
    //////

    if(argc == 2){
        nome_arquivo_original = argv[1];
        cout << "Nome do arquivo original: ";
        cout << nome_arquivo_original << endl;

        size_t found = nome_arquivo_original.find(extensao_original);
        if(found!=string::npos){
            nome_geral_arquivos = nome_arquivo_original.substr(0,found);
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

// Inicializa as Tabelas de Operções e Diretivas;
    preencheOperacoes(operacoes);
    preencheDiretivas(diretivas);

// Abertura do arquivo fonte;
    arquivo_original.open(nome_arquivo_original, ios_base::in);
    arquivo_formatado.open(nome_geral_arquivos + ".fmt", ios_base::out);
    if(!arquivo_original.is_open()||!arquivo_formatado.is_open())
    {
        cout << "Erro! Nao foi possível abrir o arquivo." << endl;
        return 0;
    }
// formata arquivo original;

    formataArquivo(arquivo_original, arquivo_formatado);

//fechando todos os arquivos abertos;

    arquivo_original.close();
    arquivo_formatado.close();

// Reabrindo arquivos;

    arquivo_formatado.open(nome_geral_arquivos + ".fmt", ios_base::in);
    arquivo_pre_processado.open(nome_geral_arquivos + extensao_processado, ios_base::out);
    arquivo_objeto.open(nome_geral_arquivos + extensao_objeto, ios_base::out);

    if(!arquivo_objeto.is_open()||!arquivo_formatado.is_open()||!arquivo_pre_processado.is_open())
    {
        cout << "Erro! Nao foi possível abrir o arquivo." << endl;
        return 0;
    }

// Montador de Passo Unico

    linha(arquivo_formatado, arquivo_objeto, arquivo_pre_processado, operacoes, diretivas, Tsimbolos);

//// Testando Tabela de simbolos
//    cout << "Definido? - " << Tsimbolos[2].flag_def << endl;
//    cout << "Definido? - " << Tsimbolos[2].valor << endl;
//for(int i = 0; i <= Tsimbolos[2].flag_fim; i++){
//    cout << "Lista - Simbolo 'H'- " << i << " - " << Tsimbolos[2].lista[i] << endl;
//}
//
for(int i = 0; i <= contador_de_simbolos; i++){
    cout << "Simb: " << Tsimbolos[i].simbolo << endl;
    for(int j = 0; j <= Tsimbolos[i].pos_fim_lista; j++){
        cout << "end:: " << Tsimbolos[i].lista[j][0] << endl;
        cout << "vec:: " << Tsimbolos[i].lista[j][1] << endl;
    }
    cout << "\n" << endl;
}


// Encerrando programa
    arquivo_formatado.close();
    arquivo_pre_processado.close();
    arquivo_objeto.close();

    return 0;
};

////////////////////////////////////////////////////////////////
//Functions

void preencheOperacoes (struct operacoes_info operacoes[])
{
    //string ADD ("ADD");
    operacoes[0].nome = "ADD";
    operacoes[1].nome = "SUB";
    operacoes[2].nome = "MULT";
    operacoes[3].nome = "DIV";
    operacoes[4].nome = "JMP";
    operacoes[5].nome = "JMPN";
    operacoes[6].nome = "JMPP";
    operacoes[7].nome = "JMPZ";
    operacoes[8].nome = "COPY";
    operacoes[9].nome = "LOAD";
    operacoes[10].nome = "STORE";
    operacoes[11].nome = "INPUT";
    operacoes[12].nome = "OUTPUT";
    operacoes[13].nome = "STOP";

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

};

void preencheDiretivas (struct diretivas_info diretivas[])
{
    diretivas[0].nome = "SECTION";
    diretivas[1].nome = "SPACE";
    diretivas[2].nome = "CONST";
    diretivas[3].nome = "EQU";
    diretivas[4].nome = "IF";

    diretivas[0].numero_de_operandos = 1;
    // A quantidade de operandos em SPACE é variado. Vou criar uma Flag ou uma função de verificação;
    diretivas[1].numero_de_operandos = -1;
    //
    diretivas[2].numero_de_operandos = 1;
    diretivas[3].numero_de_operandos = 1;
    diretivas[4].numero_de_operandos = 1;

};

void formataArquivo (ifstream& arquivo_original, fstream& arquivo_formatado)
{
    string linha;
    int i, tamanho;
    size_t posicao, proxima;

    while(!arquivo_original.eof())
    {
        getline(arquivo_original, linha);
        tamanho = linha.length();

        // Coloca tudo em Maiusculo
        for(i = 0; i < tamanho; i++)
        {
            linha[i] = toupper(linha[i]);
        }

        //Elimina comentarios
        posicao = linha.find(';');

        if(posicao!=string::npos)
        {
            linha.erase(posicao, tamanho-posicao);
        }

        //Elimina Tabulacao
        posicao = linha.find('\t');
        while (posicao!=string::npos){
            linha.replace(posicao, 1, " ");
            posicao = linha.find('\t');
        }

        //Elimina espaco duplo
        posicao = linha.find(' ');
        proxima = linha.find(' ', posicao + 1);

        while(proxima!=string::npos)
        {
            if(proxima == posicao + 1)
            {
                linha.erase(proxima, 1);
                proxima = linha.find(' ', posicao + 1);
            }else{
                posicao = proxima;
                proxima = linha.find(' ', posicao + 1);
            }
        }

        //Elimina espacos no comeco
        while(linha[0] == ' ')
        {
            linha.erase(0, 1);
        }

        //Elimina espacos no verificaTokensValidos
        posicao = linha.length();

        while(linha[posicao - 1] == ' ')
        {
            linha.erase(posicao - 1, 1);
        }

        //cout << linha << endl;
        arquivo_formatado << linha << endl;
    }

}

void linha (fstream& arquivo_formatado, fstream& arquivo_objeto, fstream& arquivo_pre_processado, struct operacoes_info operacoes[], struct diretivas_info diretivas[], struct simbolo_tab Tsimbolos[])
{
    string linha;
    string simbolo, mnemonico, operando1, operando2;
    string vec_op_1, vec_op_2;
    string vazia = "\0";
    size_t posicaoINI, posicaoFIN, vec_pos;
    int aux = 0;

    while(!arquivo_formatado.eof())
    {
        getline(arquivo_formatado, linha);

        cout << linha << "\n" << endl;

        // Verifica se ha um simbolo na linha
        // Criar funcao para verificar se comeca
        //com Letra ou underscore e se
        // não há espaços e tab no meio da palavra

        // Verifica o mnemonico
        // Criar funcao para verificar se o mnemonico
        // é instrucao ou diretiva
        // e verificar
        // É NECESSARIO TER CUIDADO PARA O CASO
        // DE TER APENAR A LABEL NESSA LINHA;

        posicaoINI = linha.find(':');
        if(posicaoINI != string::npos)
        {
            simbolo = linha.substr(0, posicaoINI);
            posicaoINI = linha.find(' ');
            posicaoFIN = linha.find(' ', posicaoINI + 1);

            if((posicaoINI != string::npos) && (posicaoFIN != string::npos))
            {
                mnemonico = linha.substr(posicaoINI + 1, posicaoFIN - posicaoINI - 1);
                posicaoINI = posicaoFIN;
                posicaoFIN = linha.find(',', posicaoINI + 1); // Condicao de cumprimento do copy

                if((posicaoINI != string::npos) && (posicaoFIN != string::npos))
                {
                    operando1 = linha.substr(posicaoINI + 1, posicaoFIN - posicaoINI - 1);
                        vec_pos = operando1.find('+');
                        if(vec_pos != string::npos){
                            vec_op_1 = operando1.substr(vec_pos + 1, operando1.length());
                            operando1 = operando1.substr(0,vec_pos);
                        }else{
                            vec_op_1 = "\0";
                        }

                    operando2 = linha.substr(posicaoFIN + 1, linha.length());
                    vec_pos = operando2.find('+');
                    if(vec_pos != string::npos){
                        vec_op_2 = operando2.substr(vec_pos + 1, operando2.length());
                        operando2 = operando2.substr(0,vec_pos);
                    }else{
                        vec_op_2 = "\0";
                    }
                }else{
                    operando1 = linha.substr(posicaoINI + 1, linha.length());
                    vec_pos = operando1.find('+');
                    if(vec_pos != string::npos){
                        vec_op_1 = operando1.substr(vec_pos + 1, operando1.length());
                        operando1 = operando1.substr(0,vec_pos);
                    }else{
                        vec_op_1 = "\0";
                    }

                    operando2 = "\0";
                    vec_op_2 = "\0";
                }
            }
            else if((posicaoINI != string::npos) && (posicaoFIN == string::npos))
            {
                mnemonico = linha.substr(posicaoINI + 1, linha.length());
                operando1 = "\0";
                vec_op_1 = "\0";
                operando2 = "\0";
                vec_op_2 = "\0";
            }else{
                mnemonico = "\0";
                operando1 = "\0";
                vec_op_1 = "\0";
                operando2 = "\0";
                vec_op_2 = "\0";

                cout << "Verificar a label sozinha e a posicao da proxima linha" << "???" << endl;
            }
        }else{
            simbolo = "\0";
            posicaoINI = linha.find(' ');
            posicaoFIN = linha.find(',', posicaoINI + 1); // condicao de cumprimento de copy;

            if((posicaoINI != string::npos) && (posicaoFIN != string::npos))
            {
                mnemonico = linha.substr(0, posicaoINI);

                operando1 = linha.substr(posicaoINI + 1, posicaoFIN - posicaoINI - 1);
                vec_pos = operando1.find('+');
                if(vec_pos != string::npos){
                    vec_op_1 = operando1.substr(vec_pos + 1, operando1.length());
                    operando1 = operando1.substr(0,vec_pos);
                }else{
                    vec_op_1 = "\0";
                }

                operando2 = linha.substr(posicaoFIN + 1, linha.length());
                vec_pos = operando2.find('+');
                if(vec_pos != string::npos){
                    vec_op_2 = operando2.substr(vec_pos + 1, operando2.length());
                    operando2 = operando2.substr(0,vec_pos);
                }else{
                    vec_op_2 = "\0";
                }
            }
            else if((posicaoINI != string::npos) && (posicaoFIN == string::npos))
            {
                mnemonico = linha.substr(0, posicaoINI);

                operando1 = linha.substr(posicaoINI + 1, linha.length());
                vec_pos = operando1.find('+');

                if(vec_pos != string::npos){
                    vec_op_1 = operando1.substr(vec_pos + 1, operando1.length());
                    operando1 = operando1.substr(0,vec_pos);
                }else{
                    vec_op_1 = "\0";
                }

                operando2 = "\0";
                vec_op_2 = "\0";
            }else{
                mnemonico = linha;
                operando1 = "\0";
                operando2 = "\0";
            }
        }



        cout << "simbolo: |" << simbolo << "|" << endl;
        cout << "mnemonico: |" << mnemonico << "|" << endl;
        cout << "Operando 1: |" << operando1 << "|" << endl;
        cout << "Operando 2: |" << operando2 << "|" << endl;
        cout << "vec_op_1: |" << vec_op_1 << "|" << endl;
        cout << "vec_op_2: |" << vec_op_2 << "|" << endl;
        cout << "\n\n";

        int  index;
        index = procuraDiretiva(diretivas,mnemonico); /// Verificar se eh diretiva antes de add a tabela de simbolo;
        if(index != -1){
            operando1 = "\0";
            operando2 = "\0";
        }

        aux = insereSimbolo(Tsimbolos, simbolo, mnemonico, operando1, operando2, vec_op_1, vec_op_2);
        //cout << "Simbolo: " << Tsimbolos[contador_de_simbolos].simbolo << "++" << endl;
        aux = procuraInstrucao(operacoes, mnemonico);
        //cout << "Tamanho Operacao: " << operacoes[aux].tamanho << " - posicao : " << contador_de_posicao + 1 << endl;
        if(aux!=-1){
            contador_de_posicao = contador_de_posicao + operacoes[aux].tamanho;
        }
        //system("Pause");

    }
}

//Retorna o Index da instrucao
int procuraInstrucao (struct operacoes_info operacoes[], string mnemonico)
{
    int i = 0;
    int indicador = -1;

    for(i = 0; i < NUMERO_DE_INSTRUCOES; i++){
        if(mnemonico.compare(operacoes[i].nome)==0){
            //opcode = operacoes[i].opcode;
            indicador = i;
        }
    }
        return indicador;
}

//Retorna o Index da diretiva
int procuraDiretiva (struct diretivas_info diretivas[], string mnemonico)
{
    int i = 0;
    int diretiva = -1;

    for(i = 0; i < NUMERO_DE_DIRETIVAS; i++){
        if(mnemonico.compare(diretivas[i].nome) == 0){
            //diretiva = diretivas[i].opcode;
            diretiva = i;
        }
    }
        return diretiva;
}

int preProcessamento(fstream& arquivo_pre_processado, string linha)
{
    int a = 0;
    return a;
}

/////////////////////////////////////////////////////////////////

int insereSimbolo(struct simbolo_tab Tsimbolos[], string simbolo, string mnemonico, string operando1, string operando2, string vec_op_1, string vec_op_2){

    int index, i = 0;

    //Confere se há simbolo/label na string simbolo ou nas strings operandos
    if(simbolo.compare("\0")!=0) // Caso exista simbolo definido - label
    {
        index = procuraSimbolo(simbolo, Tsimbolos);
        if(index != -1) // Simbolo já existente em Tsimbolos
        {
            Tsimbolos[index].flag_def = 1; //flag de endereço da label encontrado;
            Tsimbolos[index].valor = contador_de_posicao + 1; //Endereço da label;
            if(mnemonico.compare("SPACE")==0)
            {
                Tsimbolos[index].space == 1;
                if(operando1.compare("\0")!=0){
                    Tsimbolos[index].space_vec = stoi(operando1, nullptr, 0); //Verificar aqui se o space funciona;
                }
            }
        // Verificar erro de label já existente aqui.
        }else{  // Simbolo não existe na Tsimbolos
            contador_de_simbolos++;
            Tsimbolos[contador_de_simbolos].simbolo = simbolo;
            Tsimbolos[contador_de_simbolos].flag_def = 1; //flag de endereço da label encontrado;
            Tsimbolos[contador_de_simbolos].valor = contador_de_posicao + 1; //Endereço da label;

            if(mnemonico.compare("SPACE")==0)
            {
                Tsimbolos[contador_de_simbolos].space == 1;
                if(operando1.compare("\0")!=0){
                    Tsimbolos[contador_de_simbolos].space_vec = stoi(operando1, nullptr, 0); //Verificar aqui se o space funciona;
                }
            }
        // Verificar erro de label já existente aqui.
        }
    }
    if(operando1.compare("\0")!=0) // Caso exista simbolo não definido ou definido como argumento
    {
        index = procuraSimbolo(operando1, Tsimbolos);

        //cout << index << endl;
        if(index != -1) // Simbolo já existente em Tsimbolos
        {
            Tsimbolos[index].pos_fim_lista++;
            Tsimbolos[index].lista[Tsimbolos[index].pos_fim_lista][0] = contador_de_posicao + 2; // posição onde a label é utilizada;
            if(vec_op_1.compare("\0")!=0)
            {
                Tsimbolos[index].lista[Tsimbolos[index].pos_fim_lista][1] = stoi(vec_op_1, nullptr, 0);
                //Verificar erro de salto maior que o space;
            }
        }else{  // Simbolo não existe na Tsimbolos
            contador_de_simbolos++;
            Tsimbolos[contador_de_simbolos].simbolo = operando1;
            Tsimbolos[contador_de_simbolos].pos_fim_lista++;
            Tsimbolos[contador_de_simbolos].lista[Tsimbolos[contador_de_simbolos].pos_fim_lista][0] = contador_de_posicao + 2; // posição onde a label é utilizada;

            if(vec_op_1.compare("\0")!=0)
            {
                Tsimbolos[contador_de_simbolos].lista[Tsimbolos[contador_de_simbolos].pos_fim_lista][1] = stoi(vec_op_1, nullptr, 0);
                //Verificar erro de salto maior que o space;
            }
        }
    }
    if(operando2.compare("\0")!=0) // Caso exista simbolo não definido ou definido como argumento
    {
        index = procuraSimbolo(operando2, Tsimbolos);
        if(index != -1) // Simbolo já existente em Tsimbolos
        {
            while(Tsimbolos[index].lista[i][1]!=-1)
            {
                i++;
            }
            Tsimbolos[index].pos_fim_lista++;
            Tsimbolos[index].lista[Tsimbolos[index].pos_fim_lista][0] = contador_de_posicao + 3; // posição onde a label é utilizada;
            if(vec_op_2.compare("\0")!=0)
            {
                Tsimbolos[index].lista[Tsimbolos[index].pos_fim_lista][1] = stoi(vec_op_2, nullptr, 0);
                //Verificar erro de salto maior que o space;
            }
        }else{  // Simbolo não existe na Tsimbolos
            contador_de_simbolos++;
            Tsimbolos[contador_de_simbolos].simbolo = operando2;
            Tsimbolos[contador_de_simbolos].pos_fim_lista++;
            Tsimbolos[contador_de_simbolos].lista[Tsimbolos[contador_de_simbolos].pos_fim_lista][0] = contador_de_posicao + 3; // posição onde a label é utilizada;
            if(vec_op_2.compare("\0")!=0)
            {
                Tsimbolos[contador_de_simbolos].lista[Tsimbolos[contador_de_simbolos].pos_fim_lista][1] = stoi(vec_op_2, nullptr, 0);
                //Verificar erro de salto maior que o space;
            }
        }
    }
    if(simbolo.compare("\0")==0 && operando1.compare("\0")==0 && operando2.compare("\0")==0){
        return -1;
    }
    return 1;
}


//////////////////////////////////////////////////////////////////

int procuraSimbolo(string simbolo, struct simbolo_tab Tsimbolos[])
{
    // retorna -1 se o simbolo não existe.
    // retorna o index do simbolo se existe.

    int i = 0;
    int index = -1;

    for(i = 0; i <= contador_de_simbolos; i++){
        if(simbolo.compare(Tsimbolos[i].simbolo) == 0){
            //diretiva = diretivas[i].opcode;
            index = i;
            break;
        }
        //cout << index << "++" << endl;
    }
    return index;
}
