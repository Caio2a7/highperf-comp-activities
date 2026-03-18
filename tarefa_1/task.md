Implemente duas versões da multiplicação de matriz por vetor (MxV) em C
- uma com acesso à matriz por linhas (linha externa, coluna interna) 
- outra por colunas (coluna externa, linha interna). 

Meça o tempo de execução de cada versão com uma função apropriada e execute testes com diferentes tamanhos de matriz. 

Identifique, a partir de que tamanho, os tempos passam a divergir significativamente e explique por que isso ocorre, relacionando suas observações ao uso da memória cache e ao padrão de acesso à memória.

----
vou tentar explicar para ver se eu entendi
digamos essa matriz:
int mat[10][10]
ela tem 4*10*10 = 400 bytes, se iterarmos sobre ela vide row-major pelo meu entendimento ele acessa endereço da linha 0 e percorre colunas, o que na memória é mais sequencial likely
0x00, 0x01, 0x02 .... 0x10, agora se ele for de coluna-major é basicamente ele pulando e voltando, quebrando a sequencialidade

