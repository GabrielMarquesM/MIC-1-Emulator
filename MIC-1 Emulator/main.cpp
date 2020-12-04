#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

//definiÃ§es de tipos
typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long microcode;

//estrutura para guardar uma microinstruÃ§ao decodificada
struct decoded_microcode
{
    word nadd; 
    byte jam;
    byte sft;
    byte alu;
    word reg_w;
    byte mem;
    byte reg_r;
};

//FunÃ§Ãµes utilitÃ¡rias ======================
void write_microcode(microcode w) //Dado uma microinstrucao, exibe na tela devidamente espaÃ§ado pelas suas partes.
{
   unsigned int v[36];
   for(int i = 35; i >= 0; i--)
   {
    v[i] = (w & 1);
    w = w >> 1;
   }

   for(int i = 0; i < 36; i++)
   {
    cout << v[i];
    if(i == 8 || i == 11 || i == 13 || i == 19 || i == 28 || i == 31) cout << " ";
   }
}

void write_word(word w) //Dada uma palavra (valor de 32 bits / 4 bytes), exibe o valor binÃ¡rio correspondente.
{
   unsigned int v[32];
   for(int i = 31; i >= 0; i--)
   {
      v[i] = (w & 1);
      w = w >> 1;
   }

   for(int i = 0; i < 32; i++)
      cout << v[i];
}

void write_byte(byte b) //Dado um byte (valor de 8 bits), exibe o valor binÃ¡rio correspondente na tela.
{
   unsigned int v[8];
   for(int i = 7; i >= 0; i--)
   {
      v[i] = (b & 1);
      b = b >> 1;
   }

   for(int i = 0; i < 8; i++)
      cout << v[i];
}

 

void write_dec(word d) //Dada uma palavra (valor de 32 bits / 4 bytes), exibe o valor decimal correspondente.
{
  cout << (int)d << endl;
}
//=========================================

//sinalizador para desligar mÃ¡quina
bool halt = false;

//memoria principal
#define MEM_SIZE 0xFFFF+1 //0xFFFF + 0x1; // 64 KBytes = 64 x 1024 Bytes = 65536 (0xFFFF+1) x 1 Byte;
byte memory[MEM_SIZE]; //0x0000 a 0xFFFF (0 a 65535)

//registradores
word mar=0, mdr=0, pc=0, sp=0, lv=0, cpp=0, tos=0, opc=0, h=0;
byte mbr=0;

//barramentos
word bus_a=0, bus_b=0, bus_c=0, alu_out=0;

//estado da ALU para salto condicional
byte n=0, z=1;

//registradores de microprograma
word mpc = 0;

//memÃ³ria de microprograma: 512 x 64 bits = 512 x 8 bytes = 4096 bytes = 4 KBytes.
//Cada microinstruÃ§Ã£o Ã© armazenada em 8 bytes (64 bits), mas apenas os 4,5 bytes (36 bits) de ordem mais baixa sÃ£o de fato decodificados.
//Os 28 bits restantes em cada posiÃ§Ã£o da memÃ³ria sÃ£o ignorados, mas podem ser utilizados para futuras melhorias nas microinstruÃ§Ãµes para controlar microarquiteturas mais complexas.
microcode microprog[512];

//carrega microprograma
//Escreve um microprograma de controle na memÃ³ria de controle (array microprog, declarado logo acima)
void load_microprog()
{
  
  //Leitura do microprograma
  FILE *arquivo;
  arquivo = fopen("microprog.rom", "rb"); 
  fread(&microprog[0], sizeof(long), 512, arquivo);
  fclose(arquivo);
  

  //Testes
  /*
  //Aula 9:)B
  microprog[0] = 0b000000000100001101010000001000010001;  
  microprog[1] = 0b000000010000001101010000001000010001;  
  microprog[2] = 0b000000011000000101001000000000000010;  
  microprog[3] = 0b000000100000001101010000001000010001;  
  microprog[4] = 0b000000101000000101000100000000000010;  
  microprog[5] = 0b000000110000001111000000000100000010;  
  microprog[6] = 0b000000111000000101000001000000000001;  
  microprog[7] = 0b000001000000001101100001000000000110;  
  microprog[8] = 0b000001001000001101100001000000000110;  
  microprog[9] = 0b000001010000000101000000000010000110;  
  microprog[10] = 0b000000000000000000000000000001000000;  
  */

  /*
  //Aula 10: 
  microprog[0] = 0b000000000100001101010000001000010001; 
  microprog[2] = 0b000000011000001101010000001000010001; 
  microprog[3] = 0b000000100000000101000000000010100010; 
  microprog[4] = 0b000000101000000101001000000000000000; 
  microprog[5] = 0b000000000000001111000100000000001000; 
  microprog[6] = 0b000000111000001101010000001000010001; 
  microprog[7] = 0b000001000000000101000000000010000010; 
  microprog[8] = 0b000000000000000101000000000101001000; 
  microprog[9] = 0b000001010000001101010000001000010001; 
  microprog[10] = 0b000000000100000101000000001000010010; 
  microprog[11] = 0b000001100001000101000100000000001000; 
  microprog[12] = 0b000000000000001101010000001000000001; 
  microprog[268] = 0b100001101000001101010000001000010001;
  microprog[269] = 0b000000000100000101000000001000010010; 
  microprog[13] = 0b000001110000001101010000001000010001; 
  microprog[14] = 0b000001111000000101000000000010100010; 
  microprog[15] = 0b000010000000000101001000000000000000; 
  microprog[16] = 0b000000000000001111110100000000001000;
  */

}

//carrega programa na memÃ³ria principal para ser executado pelo emulador.
//programa escrito em linguagem de mÃ¡quina (binÃ¡rio) direto na memÃ³ria principal (array memory declarado mais acima).
void load_prog()
{
  
 // Leitura do programa
  byte q[32];
  FILE *prog_q;
  prog_q = fopen("instructionsfinal.bin", "rb"); 

// Q binario
  if (prog_q != NULL)
  {
    fread(&q[0], sizeof(byte), 32, prog_q);
    fclose(prog_q);
  }
  
// Conversão de Q p/ decimal
  unsigned int q_convertido;
  for(int i = 0; i<32; i++)
  {
    q_convertido = q_convertido + (1 & q[i]);
    
    if(i != 31)
    {
      q_convertido = q_convertido << 1;
    }
  }


  // Cria num_bits, um vetor com posições para todos os bits do programa, e guarda-os a partir do arquivo aberto em prog_em_bits.

  char num_bits[(q_convertido + 4)*8];
  FILE* prog_em_bits;
  prog_em_bits = fopen("instructionsfinal.bin", "rb");

  if (prog_em_bits != NULL)
  {
    fread(&num_bits[0], sizeof(char), (q_convertido + 4) * 8, prog_em_bits);
    fclose(prog_em_bits);
  }

  int ultimo_bit = 0; // Conta até chegar no ultimo bit do byte 
  int cont_memoria = 0; //Conta o valor do próx endereço de memoria para inserir o byte
  byte prog_sem_q;

  // Tira os bits de Q da lista de bits e salva os bytes da inicializacao na memoria

  for(int i = 32; i < 32 + 20 * 8; i++)
  {
    prog_sem_q = prog_sem_q + (1 & num_bits[i]);
    
    if((i+1) % 8 !=0)
    {
      prog_sem_q = prog_sem_q << 1;
    }
    ultimo_bit += 1;

    if(ultimo_bit == 8)
    {
      memory[cont_memoria] = prog_sem_q;
      prog_sem_q = 0;
      ultimo_bit = 0;
      cont_memoria += 1;
    }  
  }
  ultimo_bit = 0;
  cont_memoria = 1025;
  prog_sem_q = 0;

  //Salva os bytes do programa na memória depois dos 24 bytes de Q + inicialização.

  for(int i = 32 + 20 * 8; i < (q_convertido + 4) * 8; i++)
  {
    prog_sem_q = prog_sem_q + (1 & num_bits[i]);
    
    if((i+1) % 8 !=0)
    {
      prog_sem_q = prog_sem_q << 1;
    }
    ultimo_bit += 1;

    if(ultimo_bit == 8)
    {
      memory[cont_memoria] = prog_sem_q;
      prog_sem_q = 0;
      ultimo_bit = 0;
      cont_memoria += 1;
    }
  }
}

//exibe estado da mÃ¡quina
void debug(bool clr = true)
{ 
    if(clr) system("clear");

    cout << "MicroinstruÃ§Ã£o: ";
    write_microcode(microprog[mpc]);

    cout << "\n\nMemÃ³ria principal: \nPilha: \n";
    for(int i = lv*4; i <= sp*4; i+=4)
    {
        write_byte(memory[i+3]);
        cout << " ";
        write_byte(memory[i+2]);
        cout << " ";
        write_byte(memory[i+1]);
        cout << " ";
        write_byte(memory[i]);
        cout << " : ";
        if(i < 10) cout << " ";
        cout << i << " | " << memory[i+3] << " " << memory[i+2] << " " << memory[i+1] << " " << memory[i];
        word w;
        memcpy(&w, &memory[i], 4);
        cout << " | " << i/4 << " : " << w << endl;
    }

    cout << "\n\nPC: \n";
    for(int i = (pc-1); i <= pc+20; i+=4)
    {
        write_byte(memory[i+3]);
        cout << " ";
        write_byte(memory[i+2]);
        cout << " ";
        write_byte(memory[i+1]);
        cout << " ";
        write_byte(memory[i]);
        cout << " : ";
        if(i < 10) cout << " ";
        cout << i << " | " << memory[i+3] << " " << memory[i+2] << " " << memory[i+1] << " " << memory[i];
        word w;
        memcpy(&w, &memory[i], 4);
        cout << " | " << i/4 << " : " << w << endl;
    }

    cout << "\nRegistradores - \nMAR: " << mar << " ("; write_word(mar);
    cout << ") \nMDR: " << mdr << " ("; write_word(mdr);
    cout << ") \nPC : " << pc << " ("; write_word(pc);
    cout << ") \nMBR: " << (int) mbr << " ("; write_byte(mbr);
    cout << ") \nSP : " << sp << " (";  write_word(sp);
    cout << ") \nLV : " << lv << " ("; write_word(lv);
    cout << ") \nCPP: " << cpp << " ("; write_word(cpp);
    cout << ") \nTOS: " << tos << " ("; write_word(tos);
    cout << ") \nOPC: " << opc << " ("; write_word(opc);
    cout << ") \nH  : " << h << " ("; write_word(h);
    cout << ")" << endl;
}

decoded_microcode decode_microcode(microcode code) //Recebe uma microinstruÃ§Ã£o binÃ¡ria e separa suas partes preenchendo uma estrutura de microinstrucao decodificada, retornando-a.
{
  decoded_microcode dec;

  dec.nadd = code >> 27;
  dec.nadd = (dec.nadd & 0b00000000000000000000000111111111);

  dec.jam = code  >> 24;
  dec.jam = (dec.jam & 0b00000111);

  dec.sft = code >> 22;
  dec.sft = (dec.sft & 0b00000011);

  dec.alu = code >> 16;
  dec.alu = (dec.alu & 0b00111111);

  dec.reg_w = code >> 7;
  dec.reg_w = (dec.reg_w & 0b00000000000000000000000111111111);

  dec.mem = code >> 4;
  dec.mem = (dec.mem & 0b00000111);

  dec.reg_r = code;
  dec.reg_r = (dec.reg_r & 0b00001111);
    
  return dec;
}

//alu
//recebe uma operaÃ§Ã£o de alu binÃ¡ria representada em um byte (ignora-se os 2 bits de mais alta ordem, pois a operaÃ§Ã£o Ã© representada em 6 bits)
//e duas palavras (as duas entradas da alu), carregando no barramento alu_out o resultado da respectiva operaÃ§Ã£o aplicada Ã s duas palavras.
void alu(byte func, word a, word b)
{
  if (func == 24)
  {
    alu_out = a;
  }

  else if (func == 20)
  {
    alu_out = b;
  }
 
  else if (func == 26)
  {
    alu_out = ~a;
  }
  
  else if (func == 44)
  {
    alu_out = ~b;
  }

  else if (func == 60)
  {
    alu_out = a + b;
  }

  else if (func == 61)
  {
    alu_out = a + b + 1;
  }

  else if (func == 57)
  {
    alu_out = a + 1;
  }

  else if (func == 53)
  {
    alu_out = b + 1;
  }

  else if (func == 63)
  {
    alu_out = b - a;
  }

  else if (func == 54)
  {
    alu_out = b - 1;
  }

  else if (func == 59)
  {
    alu_out = -a;
  }

  else if (func == 12)
  {
    alu_out = a & b;
  }

  else if (func == 28)
  {
    alu_out = a | b;
  }

  else if (func == 16)
  {
    alu_out = 0;
  }

  else if (func == 49)
  {
    alu_out = 1;
  }

  else if (func == 50)
  {
    alu_out = -1;
  }

  if (alu_out == 0)
  {
    z = 1;
    n = 0;
  }

  else 
  { 
    z = 0;
    n = 1;
  }
    
}

//Deslocamento. Recebe a instruÃ§Ã£o binÃ¡ria de deslocamento representada em um byte (ignora-se os 6 bits de mais alta ordem, pois o deslocador eh controlado por 2 bits apenas)
//e uma palavra (a entrada do deslocador) e coloca o resultado no barramento bus_c.
void shift(byte s, word w)
{
  unsigned int v[2];
  
  for(int i = 1; i >= 0; i--)
  {
    v[i] = (s & 1);
    s = s >> 1;
  }

  bus_c = w;

  if (v[0] == 1) // shift logico para esquerda
  {
    word msb = w >> 31 << 31; 
    word palavra = w >> 1;
    bus_c = msb + palavra;   
  }

  if (v[1] == 1) // shift aritmetico para direita (mantem o msb e desloca 1 para esquerda)
  {
    bus_c = w << 8;
  }
}

//Leitura de registradores. Recebe o nÃºmero do registrador a ser lido (0 = mdr, 1 = pc, 2 = mbr, 3 = mbru, ..., 8 = opc) representado em um byte,
//carregando o barramento bus_b (entrada b da ALU) com o valor do respectivo registrador e o barramento bus_a (entrada A da ALU) com o valor do registrador h.
void read_registers(byte reg_end)
{
  bus_a = h;
  word mbru =  mbr; // MBRU é o MBR expandido para uma palavra(acrescimo de 24 bits 0's mais significativos)  

  switch(reg_end) 
  {
    case 0:
      bus_b = mdr;
      break;

    case 1:
      bus_b = pc;
      break;
  
    case 3: 
      bus_b = mbru;
      break;
    
    case 4:
      bus_b = sp;
      break;
    
    case 5:
      bus_b = lv;
      break;
    
    case 6:
      bus_b = cpp;
      break;
    
    case 7:
      bus_b = tos;
      break;

    case 8:
      bus_b = opc;
      break;     
 }

  if (reg_end == 2)
  {    
      byte sinal = mbr >> 7;
      
      if (sinal == 1)
      {
        bus_b = (0b11111111111111111111111100000000 | mbr);
      }
      
      if (sinal == 0)
      {
        bus_b = mbr;
      }
  }

}

//Escrita de registradores. Recebe uma palavra (valor de 32 bits) cujos 9 bits de ordem mais baixa indicam quais dos 9 registradores que
//podem ser escritos receberao o valor que estÃ¡ no barramento bus_c (saÃ­da do deslocador).
void write_register(word reg_end)
{
  unsigned int v[9];
   for(int i = 8; i >= 0; i--)
   {
      v[i] = (reg_end & 1);
      reg_end = reg_end >> 1;
   }

  if (v[8] == 1) mar = bus_c;
  if (v[7] == 1) mdr = bus_c;
  if (v[6] == 1) pc = bus_c;
  if (v[5] == 1) sp = bus_c;
  if (v[4] == 1) lv = bus_c;
  if (v[3] == 1) cpp = bus_c;
  if (v[2] == 1) tos = bus_c;
  if (v[1] == 1) opc = bus_c;
  if (v[0] == 1) h = bus_c;
}

//Leitura e escrita de memÃ³ria. Recebe em um byte o comando de memÃ³ria codificado nos 3 bits de mais baixa ordem (fetch, read e write, podendo executar qualquer conjunto dessas trÃªs operaÃ§Ãµes ao
//mesmo tempo, sempre nessa ordem) e executa a respectiva operaÃ§Ã£o na memÃ³ria principal.
//fetch: lÃª um byte da memÃ³ria principal no endereÃ§o constando em PC para o registrador MBR. EndereÃ§amento por byte.
//write e read: escreve e lÃª uma PALAVRA na memÃ³ria principal (ou seja, 4 bytes em sequÃªncia) no endereÃ§o constando em MAR com valor no registrador MDR. Nesse caso, o endereÃ§amento Ã© dado em palavras.
//Mas, como a memoria principal eh um array de bytes, deve-se fazer a conversÃ£o do endereÃ§amento de palavra para byte (por exemplo, a palavra com endereÃ§o 4 corresponde aos bytes 16, 17, 18 e 19).
//Lembrando que esta Ã© uma mÃ¡quina "little endian", isto Ã©, os bits menos significativos sÃ£o os de posiÃ§Ãµes mais baixas.
//No exemplo dado, suponha os bytes:
//16 = 00110011
//17 = 11100011
//18 = 10101010
//19 = 01010101
//Tais bytes correspondem Ã  palavra 01010101101010101110001100110011
void mainmemory_io(byte control)
{
  unsigned int v[8];
   for(int i = 7; i >= 0; i--)
   {
    v[i] = (control & 1);
    control = control >> 1;
   }

  if (v[7] == 1) //Fetch
  { 
    mbr = memory[pc];
  }

  if (v[6] == 1) //Read
  {
    word palavra = 0;
    for(int i = ((int)mar)*4 + 3; i>= ((int)mar)*4; i--)
    {
      palavra = palavra << 8;
      palavra += memory[i];
    }
    
    mdr = palavra;
  }

  
  if (v[5] == 1) //Write
  { 
    byte teste[4];
    // Move os 4 bytes separadamente para as unidades menos significativas, "limpando a palavra e colocando-os em cada espaço do vetor teste"
    teste[0] = mdr << 24 >> 24;
    teste[1] = mdr << 16 >> 24;
    teste[2] = mdr << 8 >> 24;
    teste[3] = mdr >> 24;

    int cont = 0;

    for(int i = ((int)mar)*4; i<= ((int)mar*4) + 3; i++)
    {
      memory[i] = teste[cont];
      cont += 1;
    }
  }
}

//Define prÃ³xima microinstruÃ§Ã£o a ser executada. Recebe o endereÃ§o da prÃ³xima instruÃ§Ã£o a ser executada codificado em uma palavra (considera-se, portanto, apenas os 9 bits menos significativos)
//e um modificador (regra de salto) codificado em um byte (considera-se, portanto, apenas os 3 bits menos significativos, ou seja JAMZ (bit 0), JAMN (bit 1) e JMPC (bit 2)), construindo e
//retornando o endereÃ§o definitivo (codificado em uma word - desconsidera-se os 21 bits mais significativos (sÃ£o zerados)) da prÃ³xima microinstruÃ§Ã£o.
word next_address(word next, byte jam)
{
  unsigned int v[3];
  
  for(int i = 2; i >= 0; i--)
  {
    v[i] = (jam & 1);
    jam = jam >> 1;
  }

  if (v[0] == 1) //JMPC
  {
    next = (mbr | next);
  }

  if (v[1] == 1) //JAMN 
  {
    word msb_n = next >> 8; 
    msb_n = (msb_n | n);
    msb_n = msb_n << 8;
    word next_without_msb_n = next << 24 >> 24;

    next = msb_n + next_without_msb_n;
  }


  if (v[2] == 1) //JAMZ
  {
    word msb_z = next >> 8; 
    msb_z = (msb_z | z);
    msb_z = msb_z << 8;
    word next_without_msb_z = next << 24 >> 24;

    next = msb_z + next_without_msb_z;
  }
  return next;
}

int main(int argc, char* argv[])
{

    load_microprog(); //carrega microprograma de controle

    load_prog(); //carrega programa na memÃ³ria principal a ser executado pelo emulador. JÃ¡ que nÃ£o temos entrada e saÃ­da, jogamos o programa direto na memÃ³ria ao executar o emulador.

    decoded_microcode decmcode;

    //laÃ§o principal de execuÃ§Ã£o do emulador. Cada passo no laÃ§o corresponde a um pulso do clock.
    //o debug mostra o estado interno do processador, exibindo valores dos registradores e da memÃ³ria principal.
    //o getchar serve para controlar a execuÃ§Ã£o de cada pulso pelo clique de uma tecla no teclado, para podermos visualizar a execuÃ§Ã£o passo a passo.
    //Substitua os comentÃ¡rios pelos devidos comandos (na ordem dos comentÃ¡rios) para ter a implementaÃ§Ã£o do laÃ§o.
    while(!halt)
    {
        debug();

        //Pega uma microinstruÃ§Ã£o no armazenamento de controle no endereÃ§o determinado pelo registrador contador de microinstruÃ§Ã£o e decodifica (gera a estrutura de microinstruÃ§Ã£o, ou seja, separa suas partes). Cada parte Ã© devidamente passada como parÃ¢metro para as funÃ§Ãµes que vÃªm a seguir.
        decmcode = decode_microcode(microprog[mpc]);
        //LÃª registradores
        read_registers(decmcode.reg_r);
        //Executa alu
        alu(decmcode.alu,bus_a,bus_b);
        //Executa deslocamento
        shift(decmcode.sft,alu_out);
        //Escreve registradores
        write_register(decmcode.reg_w);
        //Manipula memÃ³ria principal
        mainmemory_io(decmcode.mem);
        //Determina endereÃ§o da microinstruÃ§Ã£o (mpc) a ser executada no prÃ³ximo pulso de clock
        mpc = next_address(decmcode.nadd,decmcode.jam);
	      getchar();
        
  }
  
    debug(false);

    return 0;
}
