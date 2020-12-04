#Transforma número para binário e retorna-o em string dentro de uma lista

def decimal_para_binario(num,size):

  vetor = []
  aux = ''
  aux_2 = ["0"] * size
  transforma_bin = bin(num)
  indice = len(aux_2) - 1

  if num < 0:
    transforma_bin = transforma_bin[1:]
    
    if size == 16:
      transforma_dec = int("0b1111111111111111", 2)
      transforma_bin = int(transforma_bin, 2)
      transforma_dec = transforma_dec - transforma_bin + 1
      transforma_dec = bin(transforma_dec)
      transforma_dec = transforma_dec[2:]
      vetor.append(transforma_dec[0:8])
      vetor.append(transforma_dec[8:16])

    if size == 8:
      transforma_dec = int("0b11111111", 2)
      transforma_bin = int(transforma_bin,2)
      transforma_dec = transforma_dec - transforma_bin + 1
      transforma_dec = bin(transforma_dec)
      transforma_dec = transforma_dec[2:]
      vetor.append(transforma_dec)

    return vetor

  else:
    for i in range(len(transforma_bin)-1, 1, -1):
        aux_2[indice] = transforma_bin[i]
        indice = indice - 1
        
    for i in aux_2:
      aux += i

    if size == 8:
      vetor.append(aux)
      return vetor

    elif size == 16:
      vetor.append(aux[0:8])
      vetor.append(aux[8:16])
      return vetor  

    elif size == 24:
      vetor.append(aux[0:8])
      vetor.append(aux[8:16])
      vetor.append(aux[16:24])
      return vetor  

    elif size == 32:
      vetor.append(aux[0:8])
      vetor.append(aux[8:16])
      vetor.append(aux[16:24])
      vetor.append(aux[24:32])
      return vetor  

arquivo_r = open("instructionsread.txt","r")
smarcador = open("semmarcador.txt","w")
arquivo2_r = open("semmarcador.txt","r")
arquivo_w = open("write_bin.txt","w")
arquivo_final = open("instructionsfinal.bin","w")

# numero de bytes menos da função: byte_0 = 1 byte...
byte_0 = {"dup":0x0e,"iadd":0x02,"iand":0x08,"ior":0x0b,"ireturn":0x6b,"isub":0x05,"nop":0x01,"pop":0x10,"swap":0x13,"wide":0x28}

byte_1 = {"bipush":0x19,"istore":0x22,"iload":0x1c} 

byte_2 = {"goto":0x3c,"ifeq":0x47,"iflt":0x43,"if_icmpeq":0x4b,"iinc":0x36,"invokevirtual":0x55,"ldc_w":0x32}

var_offset = ["goto","ifeq","iflt","if_icmpeq"]
marcador = []
guarda_variaveis = []

pos_byte = 1

#salva as instrucoes sem marcador e guarda os marcadores na lista marcador

for linha in arquivo_r:
  instrucoes = linha.split()
  
  for i in range(len(instrucoes)):
    instrucoes[i] = instrucoes[i].lower()

  if len(instrucoes) != 0:
    #ver se a marcação existe em instrucoes[0]
    if(instrucoes[0] not in byte_0) and (instrucoes[0] not in byte_1) and (instrucoes[0] not in byte_2):

      marcador.append(instrucoes[0])
      marcador.append(pos_byte)

      #guarda os que nao sao marcadores no arquivo smarcador.txt
      for i in range(len(instrucoes)): 
        if instrucoes[i] != instrucoes[0]:
          smarcador.write(instrucoes[i] + " ")
          if i == len(instrucoes) - 1:
            smarcador.write("\n")

      #checar o número de bytes da função seguinte e adicioná-lo a posição dos bytes
      if instrucoes[1] in byte_0:
        pos_byte += 1
      if instrucoes[1] in byte_1:
        pos_byte += 2
      if instrucoes[1] in byte_2:
        pos_byte += 3  

    #guarda os que nao sao marcadores
    else:
      for i in range(len(instrucoes)):
        smarcador.write(instrucoes[i] + " ")
        if i == len(instrucoes) - 1:
          smarcador.write("\n")

    #se n existir marcador, soma os bytes das funções na posicao 0
    if instrucoes[0] in byte_0:
      pos_byte += 1
    if instrucoes[0] in byte_1:
      pos_byte += 2
    if instrucoes[0] in byte_2:
      pos_byte += 3  

#guarda o numero de bytes, incluindo marcadores
cont_bytes = pos_byte - 1

arquivo_r.close()
smarcador.close()



pos_byte = 1 #contador de bytes das instrucoes sem marcadores
for linha in arquivo2_r:
    instrucoes_2 = linha.split()
    #converte as instrucoes para binario por meio de uma funcao(decimal_para_binario), escrevendo-os em um arquivo

    if instrucoes_2[0] in byte_0: 
      bin_convertido = decimal_para_binario(int(byte_0[instrucoes_2[0]]),8)
      arquivo_w.write(bin_convertido[0] + "\n")
      pos_byte += 1

    elif instrucoes_2[0] in byte_1:
      bin_convertido = decimal_para_binario(int(byte_1[instrucoes_2[0]]),8)
      arquivo_w.write(bin_convertido[0] + "\n")

      #armazenando as variaveis do iload e istore,
      if (instrucoes_2[0] == "iload") or (instrucoes_2[0] == "istore"):
        if instrucoes_2[1] not in guarda_variaveis:
          variavel_bin = decimal_para_binario(len(guarda_variaveis),8)
          arquivo_w.write(variavel_bin[0] + "\n")
          guarda_variaveis.append(instrucoes_2[1])
      
        # se a variavel ja tiver sido criada, apenas guarda os valores de iload e istore convertidos para bin
        else:
          variavel_bin = decimal_para_binario(guarda_variaveis.index(instrucoes_2[1]),8)
          arquivo_w.write(variavel_bin[0] + "\n")

      else:
        variavel_bin = decimal_para_binario(int(instrucoes_2[1]),8)
        arquivo_w.write(variavel_bin[0] + "\n")
      pos_byte += 2


    elif instrucoes_2[0] in byte_2:
      bin_convertido = decimal_para_binario(int(byte_2[instrucoes_2[0]]),8)
      arquivo_w.write(bin_convertido[0] + "\n")
      
      if instrucoes_2[0] in var_offset:
        variavel_bin_16 = decimal_para_binario(int(marcador[marcador.index(instrucoes_2[1]) + 1]) - pos_byte,16)
        arquivo_w.write(variavel_bin_16[0] + "\n")
        arquivo_w.write(variavel_bin_16[1] + "\n")

      elif instrucoes_2[0] == 'iinc':

        if instrucoes_2[1] not in guarda_variaveis:
          variavel_bin = decimal_para_binario(len(guarda_variaveis),8)
          arquivo_w.write(variavel_bin[0] + "\n")
          guarda_variaveis.append(instrucoes_2[1])
        
        else:
          variavel_bin = decimal_para_binario(guarda_variaveis.index(instrucoes_2[1]),8)
          arquivo_w.write(variavel_bin[0] + "\n")

        variavel_bin = decimal_para_binario(int(instrucoes_2[2]),8)
        arquivo_w.write(variavel_bin[0] + "\n")

      else:
        variavel_bin_16 = decimal_para_binario(instrucoes_2[1],16)
        arquivo_w.write(variavel_bin_16[0] + "\n")
        arquivo_w.write(variavel_bin_16[1] + "\n")
      
      pos_byte += 3

arquivo_w.close()

num_variaveis = len(guarda_variaveis)
inicializador = [0x73,0x0006,0x1001,0x0400,0x1001 + num_variaveis]
q = cont_bytes + 20
q_binario = decimal_para_binario(q,32)

#escreve o Q no arquivo final
for i in q_binario:
  arquivo_final.write(i + "\n")

#0x00
hex_para_bytes = decimal_para_binario(0,8)
arquivo_final.write(hex_para_bytes[0] + "\n")

#0x73, 0x0006, 0x1001, 0x0400, (0x1001 + num_variaveis)
#salva os valores binarios dos dados de inicialização no arq final

for i in range(5):
  if i == 0:
    hex_para_bytes = decimal_para_binario(inicializador[i],24)
    
    for j in range(len(hex_para_bytes) - 1,-1,-1):
      arquivo_final.write(hex_para_bytes[j] + "\n")

  if i > 0:
    hex_para_bytes = decimal_para_binario(inicializador[i],32)
    
    for j in range(len(hex_para_bytes) - 1,-1,-1):
      arquivo_final.write(hex_para_bytes[j] + "\n")  

#salva os valores das instrucões convertidos em binario  no arquivo final
arquivo_w2 = open("write_bin.txt","r")
for i in arquivo_w2:
  arquivo_final.write(i)
arquivo_w2.close()
arquivo2_r.close()
arquivo_final.close()

arquivo_sem_espaco = open('instructionsfinal.bin', 'r')
lines = arquivo_sem_espaco.readlines()
instrucao_sem_espaco = ''.join([line.strip() for line in lines])
arquivo_sem_espaco.close()

arquivo_sem_espaco_w = open('instructionsfinal.bin', 'w')
arquivo_sem_espaco_w.write(instrucao_sem_espaco) 
arquivo_sem_espaco_w.close()