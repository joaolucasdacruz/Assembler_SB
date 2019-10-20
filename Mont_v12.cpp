#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
using namespace std;

#define NUMERO_DE_INSTRUCOES 14
#define NUMERO_DE_DIRETIVAS 5
#define NOME_DE_OPERACAO 7 // A operação com o maior nome tem 6 caracteres, porem coloquei + 1 pra encerrar a string caso necessário;
#define TAMANHO_DE_VARIAVEL 50
#define NUMERO_DE_SIMBOLOS 500
#define LINHAS_DE_CODIGO 500
#define NUMERO_DE_DADOS 200

//////////////////////////////////////////////////////////
//Prototipo das Funcoes

void preencheOperacoes (struct operacoes_info operacoes[]);
void preencheDiretivas (struct diretivas_info diretivas[]);
void formataArquivo(ifstream& arquivo_original, fstream& arquivo_formatado);
void linha(fstream& arquivo_formatado, fstream& arquivo_objeto, fstream& arquivo_pre_processado, struct operacoes_info operacoes[], struct diretivas_info diretivas[], struct simbolo_tab Tsimbolos[], struct diretiva_tab Tdiretivas[], struct objeto_code code[], struct data_tab Tdata[]);
int procuraInstrucao(struct operacoes_info operacoes[], string mnemonico);
int procuraDiretiva(struct diretivas_info diretivas[], string mnemonico);
int procuraSimbolo(string simbolo, struct simbolo_tab Tsimbolos[]);
int insereSimbolo(struct simbolo_tab Tsimbolos[], string simbolo, string mnemonico, string operando1, string operando2, string vec_op_1, string vec_op_2);
void preProcessamento(fstream& arquivo_pre_processado, string simbolo, string mnemonico, string operando1, string operando2, string vec_op_1, string vec_op_2);
int procuraEQU(string simbolo, struct diretiva_tab Tdiretivas[]);
void insereEQU(string simbolo, string operando1, struct diretiva_tab Tdiretivas[]);
void montagem(fstream& arquivo_objeto, string simbolo, string mnemonico, string operando1, string operando2, string vec_op_1, string vec_op_2, struct objeto_code code[], struct operacoes_info operacoes[], struct simbolo_tab Tsimbolos[], struct diretivas_info diretivas[]);
void corrige_end(struct simbolo_tab Tsimbolos[], struct  objeto_code code[], fstream& arquivo_objeto); //corrige endereço tbm escreve no arquivo objeto;
int varSyntax(string var);

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
    int cte = 0; // flag - constante da diretiva const;
    int cte_val = 0; // valor da constante da diretiva const;
    int pos_fim_lista = -1; // fim da lista
};

struct diretiva_tab
{
    string diretiva;
    int valor;
};

struct data_tab{
    string simbolo;
    string mnemonico;
    string operando1;
    string operando2;
    string vec_op_1;
    string vec_op_2;
};

struct objeto_code
{
    string mnemonico = "\0";
    int opcode = -1;
    string operador1 = "\0";
    int end_op_1 = -1;
    string operador2 = "\0";
    int end_op_2 = -1;

    // posicao absoluta
    int p_abs = 0;
    // tamanho da operacao
    int tam = 0;
    // space
    string space = "\0";
    //const
    int cte = -1;
    string conf_cte = "\0";

};

/////////////////////////////////////////////////////////
// Variáveis Globais
unsigned int text_flag = 0, data_flag = 0, ordem_section = 0;
int contador_de_simbolos = -1; // Quantidade de simbolos dentro da tabela de simbolos;
int contador_de_posicao = -1; //Posição absoluta dentro do arquivo de código (texto);
int contador_de_dados = -1;
int contador_de_linha_original = -1; //Posição do contador de linha (contador de programa). Serve para imprimir os erros.
int contador_de_linha_obj = -1; //Posição do contador de linha do programa do arquivo pre processado. Serve para marcar a posição das labels. // tbm serve para contar o codigo objeto;
int contador_de_diretivas = -1;
/////////////////////////////////////////////////////////
// Codigo Principal

int main(int argc, char *argv[]){

    string nome_arquivo_original, nome_geral_arquivos; //nome_arquivo_original contem a extensão '.asm', o nome_geral_arquivos nao possui a extensao;
    string extensao_original = ".asm", extensao_processado = ".pre", extensao_objeto = ".obj";
    struct operacoes_info operacoes[NUMERO_DE_INSTRUCOES];
    struct diretivas_info diretivas[NUMERO_DE_DIRETIVAS];
    struct simbolo_tab Tsimbolos[NUMERO_DE_SIMBOLOS];
    struct diretiva_tab Tdiretivas[NUMERO_DE_SIMBOLOS];
    struct objeto_code code[LINHAS_DE_CODIGO];
    struct data_tab Tdata[NUMERO_DE_DADOS];

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
    arquivo_objeto.open(nome_geral_arquivos + ".txt", ios_base::out);

    if(!arquivo_objeto.is_open()||!arquivo_formatado.is_open()||!arquivo_pre_processado.is_open())
    {
        cout << "Erro! Nao foi possível abrir o arquivo." << endl;
        return 0;
    }

// Montador de Passo Unico

    linha(arquivo_formatado, arquivo_objeto, arquivo_pre_processado, operacoes, diretivas, Tsimbolos, Tdiretivas, code, Tdata);

/*for(int i = 0; i <= contador_de_simbolos; i++){
    cout << "Simb: " << Tsimbolos[i].simbolo << endl;
    cout << "valor (end. def): " << Tsimbolos[i].valor << endl;
    cout << "Space: " << Tsimbolos[i].space << endl;
    cout << "Const: " << Tsimbolos[i].cte << endl;
    cout << "valor const: " << Tsimbolos[i].cte_val << endl;
    cout << "Flag_def: " << Tsimbolos[i].flag_def << endl;
    cout << "-------" << endl;
    for(int j = 0; j <= Tsimbolos[i].pos_fim_lista; j++){
        cout << "end. sub:: " << Tsimbolos[i].lista[j][0] << endl;
        cout << "vetor:: " << Tsimbolos[i].lista[j][1] << endl;
    }
    cout << "\n" << endl;
}*/

/*for(int i = 0; i <= contador_de_diretivas; i++){
    cout << "Simb: " << Tdiretivas[i].diretiva << endl;
    cout << "val: " << Tdiretivas[i].valor << endl;
    cout << "\n" << endl;
}*/

/*for(int i = 0; i <= contador_de_linha_obj; i++){
    cout << "inst: " << code[i].mnemonico << endl;
    cout << "posicao_abs: " << code[i].p_abs << endl;
    cout << "inst_tam: " << code[i].tam << endl;
    cout << "\n" << endl;
}*/

/*for(int i = 0; i <= contador_de_linha_obj; i++){
    cout << "Simb: " << code[i].mnemonico << endl;
    cout << "Opcode: " << code[i].opcode << endl;
    cout << "Op1: " << code[i].operador1 << endl;
    cout << "end_op1: " << code[i].end_op_1 << endl;
    cout << "Op2: " << code[i].operador2 << endl;
    cout << "end_op2: " << code[i].end_op_2 << endl;
    cout << "Space: " << code[i].space << endl;
    cout << "Const: " << code[i].cte << endl;
    cout << "END::ABS:: " << code[i].p_abs << endl;
    cout << "TAM:: " << code[i].tam << endl;
    cout << "\n" << endl;
}*/

/*    cout << "contador_de_dados:: " << contador_de_dados << endl;
for (int i=0;i<=contador_de_dados;i++){
    cout << "::DATA:: " << Tdata[i].str_data << endl;
}*/

// Encerrando programa
    arquivo_formatado.close();
    //remove((char const*)nome_geral_arquivos.append(".fmt"));
    string fmt1 = nome_geral_arquivos + ".fmt";
    char * fmt2 = new char[fmt1.size() + 1];
    copy(fmt1.begin(), fmt1.end(), fmt2);
    fmt2[fmt1.size()] = '\0';
    remove(fmt2);
    delete[] fmt2;

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

void linha (fstream& arquivo_formatado, fstream& arquivo_objeto, fstream& arquivo_pre_processado, struct operacoes_info operacoes[], struct diretivas_info diretivas[], struct simbolo_tab Tsimbolos[], struct diretiva_tab Tdiretivas[], struct objeto_code code[], struct data_tab Tdata[])
{
    string linha;
    string simbolo, mnemonico, operando1, operando2;
    string vec_op_1, vec_op_2;
    string vazia = "\0";
    size_t posicaoINI, posicaoFIN, vec_pos, test_var;
    int aux = 0, if_flag = 0, if_flag_count = 0;

    while(!arquivo_formatado.eof())
    {
        getline(arquivo_formatado, linha);

        if(linha.compare("\n")==0){
            getline(arquivo_formatado, linha);
        }

        //Contador de Linha para erro
        /*if(if_flag == 0){
                contador_de_linha_original++;
        }*/

        //cout << linha << " ::linha:: " << contador_de_linha_original << endl;

        // Separa a linha em simbolos, instrucao/diretivas e operandos

        posicaoINI = linha.find(':');
        test_var  = linha.find(':', posicaoINI + 1);
        if(test_var != string::npos){
            cout << "Erro! (Sintático) Dois rótulos na mesma linha." << " ::linha:: " << contador_de_linha_original << endl;
        }
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

/*        cout << "simbolo: |" << simbolo << "|" << endl;
        cout << "mnemonico: |" << mnemonico << "|" << endl;
        cout << "Operando 1: |" << operando1 << "|" << endl;
        cout << "Operando 2: |" << operando2 << "|" << endl;
        cout << "vec_op_1: |" << vec_op_1 << "|" << endl;
        cout << "vec_op_2: |" << vec_op_2 << "|" << endl;
        cout << "\n\n";
*/
        // Erro Léxico

        aux =  varSyntax(simbolo);
        if(aux == -1){
            cout << "Erro! (Lexico) Tokens Invalido." << " ::linha:: " << contador_de_linha_original << endl;
        }
        aux =  varSyntax(mnemonico);
        if(aux == -1){
            cout << "Erro! (Lexico) Tokens Invalido." << " ::linha:: " << contador_de_linha_original << endl;
        }

        aux = procuraDiretiva(diretivas,mnemonico);
        if(aux == -1){
            aux =  varSyntax(operando1);
            if(aux == -1){
                cout << "Erro! (Lexico) Tokens Invalido." << " ::linha:: " << contador_de_linha_original << endl;
            }
            aux =  varSyntax(operando2);
            if(aux == -1){
                cout << "Erro! (Lexico) Tokens Invalido." << " ::linha:: " << contador_de_linha_original << endl;
            }
        }

        ///Verificando ordem das secoes
        if(data_flag == 0 && text_flag==1){
            ordem_section = 1; // Secao text vem antes de data
        }else if(data_flag == 1 && text_flag==0){
            ordem_section = 2; // secao data vem antes de texto
        }

        /// Configura flag de secao
        aux = procuraDiretiva(diretivas,mnemonico);
        if(aux == 0){
            if(operando1.compare("TEXT")==0){
                text_flag = 1;
            }else if(operando1.compare("DATA")==0){
                data_flag = 1;
            }else{
                cout << "Erro!(Sintatico) Seção Invalida." << " ::linha:: " << contador_de_linha_original << endl;
            }
        }

        //////
        //Reorganizando seção data e text parte 1
        aux = procuraDiretiva(diretivas,mnemonico);
        if(aux!=0){
            if((data_flag==1) && (text_flag==0)){
                contador_de_dados = contador_de_dados + 1;
                Tdata[contador_de_dados].simbolo = simbolo;
                Tdata[contador_de_dados].mnemonico = mnemonico;
                Tdata[contador_de_dados].operando1 = operando1;
                Tdata[contador_de_dados].operando2 = operando2;
                Tdata[contador_de_dados].vec_op_1 = vec_op_1;
                Tdata[contador_de_dados].vec_op_2 = vec_op_2;
            }
        }

        /// Tratamento para EQU
        aux = procuraDiretiva(diretivas,mnemonico);
        if(aux == 3){
            if(text_flag == 1 || data_flag == 1){
                cout << "Erro!(Semantico) Diretiva EQU em secao não autorizada." << " ::linha:: " << contador_de_linha_original << endl;
            }
            insereEQU(simbolo, operando1, Tdiretivas);
            simbolo = "\0";
            if_flag = 1;
            if_flag_count = 1;
        }

        /// Tratamento para if
        aux = procuraDiretiva(diretivas,mnemonico);
        if(aux == 4){
            aux = procuraEQU(operando1, Tdiretivas);
            if(aux == -1){
                cout << "Erro!(Semantico) IF sem operando EQU" << " ::linha:: " << contador_de_linha_original << endl;
            }else{
                operando1 = to_string(Tdiretivas[aux].valor);
                if(stoi(operando1, nullptr, 0)){
                    if_flag = 1;
                    if_flag_count = 1;
                }else{
                    if_flag = 1;
                    if_flag_count = 2;
                }
            }
        }else{
            // Tratamento de EQU sem IF
            aux = procuraEQU(operando1, Tdiretivas);
            if(aux != -1){
                operando1 = to_string(Tdiretivas[aux].valor);
            }
            //cout << aux << endl;

            aux = procuraEQU(operando2, Tdiretivas);
            if(aux != -1){
                operando2 = to_string(Tdiretivas[aux].valor);
            }
        }

        /// ESCREVE NO ARQUIVO PRE-PROCESSADO.
        if(if_flag == 1){
            if(if_flag_count == 0){
                contador_de_linha_original++;
                preProcessamento(arquivo_pre_processado, simbolo, mnemonico, operando1, operando2, vec_op_1, vec_op_2);
                cout << linha << " ::linha:: " << contador_de_linha_original + 1 << endl;
                if_flag = 0;
            }
            if_flag_count = if_flag_count - 1;
        }else{
            contador_de_linha_original++;
            preProcessamento(arquivo_pre_processado, simbolo, mnemonico, operando1, operando2, vec_op_1, vec_op_2);
            cout << linha << " ::linha:: " << contador_de_linha_original + 1 << endl;
        }

        ///////
        //Reorganizando seção data e text parte 2
        if(ordem_section == 1 || (ordem_section == 2 && text_flag == 1)){
            /// Verificar se eh diretiva antes de add a tabela de simbolo;
            /// Faz o controle das labels para Space e para Const;
            aux = procuraDiretiva(diretivas,mnemonico);
            if(aux != -1 && aux != 1 && aux != 2){
                operando1 = "\0";
                operando2 = "\0";
            }

            // Erro de DIretiva na secao errada;
            aux = procuraDiretiva(diretivas,mnemonico);
            if(aux == 1 || aux == 2){
                if(data_flag == 0){
                    cout << "Erro!(Semantico) Diretiva na secao errada." << " ::linha:: " << contador_de_linha_original << endl;
                }
            }else if(aux == -1){
                if(text_flag == 0){
                    cout << "Erro!(Semantico) Instrucao na secao errada." << " ::linha:: " << contador_de_linha_original << endl;
                }
            }

            aux = insereSimbolo(Tsimbolos, simbolo, mnemonico, operando1, operando2, vec_op_1, vec_op_2);
            aux = procuraInstrucao(operacoes, mnemonico);
            if(aux!=-1){
                contador_de_posicao = contador_de_posicao + operacoes[aux].tamanho;
            }

            /// MONTAGEM DO CODIGO - ESCREVE NO ARQUIVO OBJETO.
            if(mnemonico.compare("SECTION")!=0){
                if(if_flag == 1){
                    if(if_flag_count == 0){
                        montagem(arquivo_objeto, simbolo, mnemonico, operando1, operando2, vec_op_1, vec_op_2, code, operacoes, Tsimbolos, diretivas);
                        if_flag = 0;
                    }
                    if_flag_count = if_flag_count - 1;
                }else{
                    montagem(arquivo_objeto, simbolo, mnemonico, operando1, operando2, vec_op_1, vec_op_2, code, operacoes, Tsimbolos, diretivas);
                }
            }
        }
    }

    ////////////
    //Reorganizando seção data e text parte 3
    if(ordem_section == 2){
        for(int i = 0; i<=contador_de_dados; i++){

        simbolo = Tdata[i].simbolo;
        mnemonico = Tdata[i].mnemonico;
        operando1 = Tdata[i].operando1;
        operando2 = Tdata[i].operando2;
        vec_op_1 = Tdata[i].vec_op_1;
        vec_op_2 = Tdata[i].vec_op_2;
        ////////////////////////////////////////
        ////////////////////////////////////////
            /// Verificar se eh diretiva antes de add a tabela de simbolo;
            /// Faz o controle das labels para Space e para Const;
            aux = procuraDiretiva(diretivas,mnemonico);
            if(aux != -1 && aux != 1 && aux != 2){
                operando1 = "\0";
                operando2 = "\0";
            }

            aux = insereSimbolo(Tsimbolos, simbolo, mnemonico, operando1, operando2, vec_op_1, vec_op_2);
            aux = procuraInstrucao(operacoes, mnemonico);
            if(aux!=-1){
                contador_de_posicao = contador_de_posicao + operacoes[aux].tamanho;
            }

            /// MONTAGEM DO CODIGO - ESCREVE NO ARQUIVO OBJETO.
            if(mnemonico.compare("SECTION")!=0){
                if(if_flag == 1){
                    if(if_flag_count == 0){
                        montagem(arquivo_objeto, simbolo, mnemonico, operando1, operando2, vec_op_1, vec_op_2, code, operacoes, Tsimbolos, diretivas);
                        if_flag = 0;
                    }
                    if_flag_count = if_flag_count - 1;
                }else{
                    montagem(arquivo_objeto, simbolo, mnemonico, operando1, operando2, vec_op_1, vec_op_2, code, operacoes, Tsimbolos, diretivas);
                }
            }
        }
        ////////////////////////////////////////
        ////////////////////////////////////////
    }

    // aqui verificar-se se já chegou ao fim do arquivo, e assim podemos corrigir os endereços relativos e "imprimir" no arquivo objeto.
    if(arquivo_formatado.eof()){
        if(text_flag == 0){
            cout << "Erro!(Semantico) Seção texto faltando" << " ::linha:: " << contador_de_linha_original << endl;
        }else if(data_flag == 0){
            cout << "Erro!(Semantico) Seção data faltando" << " ::linha:: " << contador_de_linha_original << endl;
        }
        cout << "-------------------------" << endl;
        corrige_end(Tsimbolos, code, arquivo_objeto);
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

void preProcessamento(fstream& arquivo_pre_processado, string simbolo, string mnemonico, string operando1, string operando2, string vec_op_1, string vec_op_2)
{
    string linha_pre = "\0";

    if(simbolo.compare("\0")!=0){
        linha_pre.append(simbolo);
        linha_pre.append(": ");
    }
    if(mnemonico.compare("\0")!=0){
        linha_pre.append(mnemonico);
        if(operando1.compare("\0")!=0){
            linha_pre.append(" ");
            linha_pre.append(operando1);
            if(vec_op_1.compare("\0")!=0){
                linha_pre.append("+");
                linha_pre.append(vec_op_1);
            }
        }
        if(operando2.compare("\0")!=0){
            linha_pre.append(",");
            linha_pre.append(operando2);
            if(vec_op_2.compare("\0")!=0){
                linha_pre.append("+");
                linha_pre.append(vec_op_2);
            }
        }
    }
    arquivo_pre_processado << linha_pre << endl;
}

void montagem(fstream& arquivo_objeto, string simbolo, string mnemonico, string operando1, string operando2, string vec_op_1, string vec_op_2, struct objeto_code code[], struct operacoes_info operacoes[], struct simbolo_tab Tsimbolos[], struct diretivas_info diretivas[])
{

    int index = -1;
    int aux;

    index = procuraInstrucao(operacoes,mnemonico);
    if(index != -1){
        contador_de_linha_obj++;
        code[contador_de_linha_obj].mnemonico = mnemonico;
        code[contador_de_linha_obj].opcode = operacoes[index].opcode;
        code[contador_de_linha_obj].tam = operacoes[index].tamanho;

        //contador de posicao para corrigir endereços (verificar redundancia com a variavel contador_de_posicao);
        if(contador_de_linha_obj!=0){
            code[contador_de_linha_obj].p_abs = code[contador_de_linha_obj-1].p_abs + code[contador_de_linha_obj-1].tam;
        }


        if(operacoes[index].numero_de_operandos == 1) // verifirar erro de quantidade de operandos
        {
            aux = procuraSimbolo(operando1, Tsimbolos);
            code[contador_de_linha_obj].operador1 = Tsimbolos[aux].simbolo;
            // a principio deixa em -1 o vec_op_x

            // Já substitiu endereço da label se já estiver disponível;
            if(aux != -1 && Tsimbolos[aux].flag_def == 1)
            {
                code[contador_de_linha_obj].end_op_1 = Tsimbolos[aux].valor;
                if(vec_op_1.compare("\0")!=0){
                    code[contador_de_linha_obj].end_op_1 = code[contador_de_linha_obj].end_op_1 + stoi(vec_op_1, nullptr, 0);
                }
            }
        }else if(operacoes[index].numero_de_operandos == 2)
        {
            // Verifica simbolo do op1
            aux = procuraSimbolo(operando1, Tsimbolos);
            code[contador_de_linha_obj].operador1 = Tsimbolos[aux].simbolo;

            // Já substitiu endereço da label se já estiver disponível;
            if(aux != -1 && Tsimbolos[aux].flag_def == 1)
            {
                code[contador_de_linha_obj].end_op_1 = Tsimbolos[aux].valor;
                if(vec_op_1.compare("\0")!=0){
                    code[contador_de_linha_obj].end_op_1 = code[contador_de_linha_obj].end_op_1 + stoi(vec_op_1, nullptr, 0);
                }
            }
            // Verifica simbolo do op2
            aux = procuraSimbolo(operando2, Tsimbolos);
            code[contador_de_linha_obj].operador2 = Tsimbolos[aux].simbolo;

            // Já substitiu endereço da label se já estiver disponível;
            if(aux != -1 && Tsimbolos[aux].flag_def == 1)
            {
                code[contador_de_linha_obj].end_op_2 = Tsimbolos[aux].valor;
                if(vec_op_1.compare("\0")!=0){
                    code[contador_de_linha_obj].end_op_2 = code[contador_de_linha_obj].end_op_2 + stoi(vec_op_2, nullptr, 0);
                }
            }

        }else if(operacoes[index].numero_de_operandos > 2){
            cout << "Erro!(Sintatico) Número de Argumentos Invalido." << " ::linha:: " << contador_de_linha_original << endl;
        }
    }

    // Insere Space para o código
    index = procuraSimbolo(simbolo, Tsimbolos);
    if(Tsimbolos[index].space == 1){
        for(aux = 0; aux < Tsimbolos[index].space_vec; aux++){
            contador_de_linha_obj++;
            code[contador_de_linha_obj].mnemonico = simbolo;
            code[contador_de_linha_obj].space = "OO";
            code[contador_de_linha_obj].p_abs = code[contador_de_linha_obj-1].p_abs + code[contador_de_linha_obj-1].tam;
            code[contador_de_linha_obj].tam = 1;
        }
    }

    // Insere const para o código
    index = procuraSimbolo(simbolo, Tsimbolos);
    if(Tsimbolos[index].cte == 1){
        contador_de_linha_obj++;
        code[contador_de_linha_obj].mnemonico = simbolo;
        code[contador_de_linha_obj].cte = Tsimbolos[index].cte_val;
        code[contador_de_linha_obj].conf_cte = "1";
        code[contador_de_linha_obj].p_abs = code[contador_de_linha_obj-1].p_abs + code[contador_de_linha_obj-1].tam;
        code[contador_de_linha_obj].tam = 1;
    }

}

/// Arrumar essa funcao - nao ta funcionando
void corrige_end(struct simbolo_tab Tsimbolos[], struct  objeto_code code[], fstream& arquivo_objeto)
{
    int posicao = -1;

    for(int i = 0; i <= contador_de_simbolos; i++){
        for(int j = 0; j <= Tsimbolos[i].pos_fim_lista; j++){
            posicao = Tsimbolos[i].lista[j][0];
            for(int k = 0; k <= contador_de_linha_obj; k++){
                if((code[k].p_abs+code[k].tam) > posicao && posicao > code[k].p_abs){
                    posicao = posicao - code[k].p_abs;
                    if(posicao == 2){
                        code[k].end_op_2 = Tsimbolos[i].valor;
                        if(Tsimbolos[i].lista[j][1] != -1){
                            code[k].end_op_2 = code[k].end_op_2 + Tsimbolos[i].lista[j][1];
                        }
                    }else if(posicao == 1){
                        code[k].end_op_1 = Tsimbolos[i].valor;
                        if(Tsimbolos[i].lista[j][1] != -1){
                            code[k].end_op_1 = code[k].end_op_1 + Tsimbolos[i].lista[j][1];
                        }
                    }
                }
            }
        }
    }

    for(int k = 0; k <= contador_de_linha_obj; k++){

        // Escrevendo no arquivo objeto;
        if(k!=0){
            arquivo_objeto << " ";
        }
        if(code[k].opcode!=-1){
            arquivo_objeto << code[k].opcode;
        }
        if(code[k].operador1.compare("\0")!=0){
            arquivo_objeto << " ";
            arquivo_objeto << code[k].end_op_1;
        }
        if(code[k].operador2.compare("\0")!=0){
            arquivo_objeto << " ";
            arquivo_objeto << code[k].end_op_2;
        }
        if(code[k].space.compare("\0")!=0){
            arquivo_objeto << code[k].space;
        }
        if(code[k].conf_cte.compare("\0") != 0){
            arquivo_objeto << code[k].cte;
        }

        //Verificando divisão por zero.
        if(code[k].opcode==4){
            posicao = code[k].end_op_1;
            for(int q = 0; q <= contador_de_linha_obj;q++){
                if((code[q].p_abs+code[q].tam) > posicao && posicao >= code[q].p_abs){
                    if(code[q].cte == 0){
                        cout << "Erro!(Semantico) Divisao por zero." << " ::linha:: " << k+2 << endl;
                    }
                }
            }
        }

        //modificacao de constante.
        if(code[k].opcode == 11 || code[k].opcode == 12){
            posicao = code[k].end_op_1;
            for(int q = 0; q <= contador_de_linha_obj;q++){
                if((code[q].p_abs+code[q].tam) > posicao && posicao >= code[q].p_abs){
                    if(code[q].conf_cte.compare("\0") != 0 && code[q].cte != -1){
                        cout << "Erro!(Semantico) Tentativa de Modificar constante." << " ::linha:: " << k+2 << endl;
                    }
                }
            }
        }

    }
}

// Retorna 1 se adicionou algum simbolo, retorna -1 se não adicionou nada;
int insereSimbolo(struct simbolo_tab Tsimbolos[], string simbolo, string mnemonico, string operando1, string operando2, string vec_op_1, string vec_op_2)
{

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
                Tsimbolos[index].space = 1;
                contador_de_posicao = contador_de_posicao + 1;
                if(operando1.compare("\0")!=0){
                    Tsimbolos[index].space_vec = stoi(operando1, nullptr, 0); //Verificar aqui se o space funciona;
                    contador_de_posicao = contador_de_posicao + stoi(operando1, nullptr, 0) - 1; // soma a o vetor no endereço
                    operando1 = "\0";
                }else if(operando1.compare("\0")==0){
                    Tsimbolos[index].space_vec = 1; //Verificar aqui se o space funciona;
                }
            }else if(mnemonico.compare("CONST")==0)
            {
                Tsimbolos[index].valor = contador_de_posicao + 1; //Endereço da label;
                Tsimbolos[index].cte = 1;
                Tsimbolos[index].cte_val = stoi(operando1, nullptr, 0);
                contador_de_posicao = contador_de_posicao + 1; //Endereço da label;
                operando1 = "\0";
            }else{
                cout << "Erro!(Semantico) Label Já existente." << " ::linha:: " << contador_de_linha_original << endl;
            }
        // Verificar erro de label já existente aqui.
        }else{  // Simbolo não existe na Tsimbolos
            contador_de_simbolos++;
            Tsimbolos[contador_de_simbolos].simbolo = simbolo;
            Tsimbolos[contador_de_simbolos].flag_def = 1; //flag de endereço da label encontrado;
            Tsimbolos[contador_de_simbolos].valor = contador_de_posicao + 1; //Endereço da label;

            if(mnemonico.compare("SPACE")==0)
            {
                Tsimbolos[contador_de_simbolos].space = 1;
                contador_de_posicao = contador_de_posicao + 1;
                if(operando1.compare("\0")!=0){
                    Tsimbolos[contador_de_simbolos].space_vec = stoi(operando1, nullptr, 0); //Verificar aqui se o space funciona;
                    contador_de_posicao = contador_de_posicao + stoi(operando1, nullptr, 0) - 1; // soma a o vetor no endereço
                    operando1 = "\0";
                }else if(operando1.compare("\0")==0){
                    Tsimbolos[contador_de_simbolos].space_vec = 1; //Verificar aqui se o space funciona;
                }
            }else if(mnemonico.compare("CONST")==0)
            {
                Tsimbolos[contador_de_simbolos].valor = contador_de_posicao + 1; //Endereço da label;
                Tsimbolos[contador_de_simbolos].cte = 1;
                Tsimbolos[contador_de_simbolos].cte_val = stoi(operando1, nullptr, 0);
                //cout << "???? ::cte:: " << Tsimbolos[contador_de_simbolos].cte << " ???? ::cte_val:: " << Tsimbolos[contador_de_simbolos].cte_val << " ???? ::::index:: " << contador_de_simbolos << endl;
                contador_de_posicao = contador_de_posicao + 1; //Endereço da label;
                operando1 = "\0";
            }else{
                cout << "Erro!(Semantico) Label Já existente." << " ::linha:: " << contador_de_linha_original << endl;
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
            }
        }else{  // Simbolo não existe na Tsimbolos
            contador_de_simbolos++;
            Tsimbolos[contador_de_simbolos].simbolo = operando1;
            Tsimbolos[contador_de_simbolos].pos_fim_lista++;
            Tsimbolos[contador_de_simbolos].lista[Tsimbolos[contador_de_simbolos].pos_fim_lista][0] = contador_de_posicao + 2; // posição onde a label é utilizada;

            if(vec_op_1.compare("\0")!=0)
            {
                Tsimbolos[contador_de_simbolos].lista[Tsimbolos[contador_de_simbolos].pos_fim_lista][1] = stoi(vec_op_1, nullptr, 0);
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
            }
        }else{  // Simbolo não existe na Tsimbolos
            contador_de_simbolos++;
            Tsimbolos[contador_de_simbolos].simbolo = operando2;
            Tsimbolos[contador_de_simbolos].pos_fim_lista++;
            Tsimbolos[contador_de_simbolos].lista[Tsimbolos[contador_de_simbolos].pos_fim_lista][0] = contador_de_posicao + 3; // posição onde a label é utilizada;
            if(vec_op_2.compare("\0")!=0)
            {
                Tsimbolos[contador_de_simbolos].lista[Tsimbolos[contador_de_simbolos].pos_fim_lista][1] = stoi(vec_op_2, nullptr, 0);
            }
        }
    }
    if(simbolo.compare("\0")==0 && operando1.compare("\0")==0 && operando2.compare("\0")==0){
        return -1;
    }
    return 1;
}

//Retorna o Index do simbolo
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

//Retorna o Index da diretiva EQU
int procuraEQU(string simbolo, struct diretiva_tab Tdiretivas[])
{
    int i = 0;
    int index = -1;

    for(i = 0; i <= contador_de_diretivas; i++){
        if(simbolo.compare(Tdiretivas[i].diretiva) == 0){
            index = i;
            break;
        }
    }
    return index;
}

void insereEQU(string simbolo, string operando1, struct diretiva_tab Tdiretivas[])
{
    int index = -1;

    index = procuraEQU(simbolo, Tdiretivas);
    if(index==-1) // Diretiva não existe
    {
        if(simbolo.compare("\0")!=0){
            if(operando1.compare("\0")!=0){
                contador_de_diretivas++;
                Tdiretivas[contador_de_diretivas].diretiva = simbolo;
                Tdiretivas[contador_de_diretivas].valor = stoi(operando1, nullptr, 0);
            }else{
            cout << "Erro!(Sintatico) diretiva EQU sem parametro" << " ::linha:: " << contador_de_linha_original << endl;
            }
        }
    }else{
        cout << "Erro!(Semantico) Label de Diretiva ja Existente." << " ::linha:: " << contador_de_linha_original << endl;
    }
}

int varSyntax(string var){

	int i, aux;

	if(var.size() >= 50){
		cout<<"erro, o tamanho maximo da label e de 50 caracteres: "<< var << " ::linha:: " << contador_de_linha_original << endl;
		return(-1);
	}
	aux = var[0];
	if(48<= aux  && aux<= 57){
		cout<<"erro, o primeiro caractere da LABEL nao pode ser um numero: "<< var << " ::linha:: " << contador_de_linha_original << endl;
		return(-1);
	}else{
		for(i=0;i<(int)var.size();i++){
			aux = var[i];
			if(!(48 <= aux && aux<= 57) && !(65<= aux && aux<=90) && !(97<= aux && aux<=122) && var[i] != '_' ){
				cout<<"erro, token com caractere invalido: "<< var << " ::linha:: " << contador_de_linha_original << endl;
				return(-1);
			}
		}
	}

	return (0);
}
