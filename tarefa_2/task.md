Implemente três laços em C para investigar os efeitos do paralelismo ao nível de instrução (ILP): 
1) inicialize um vetor com um cálculo simples; 
2) some seus elementos de forma acumulativa, criando dependência entre as iterações; 
3) quebre essa dependência utilizando múltiplas variáveis. 


Compare o tempo de execução das versões compiladas com diferentes níveis de otimização (O0, O2, O3) e analise como o estilo do código e as dependências influenciam o desempenho.

----
# -O0
## Vector Sum Benchmark — size: 10000000 #

cumulative (1 var)             sum: 100000000    time: 45.317906 ms
2 vars                         sum: 100000000    time: 20.926982 ms
3 vars                         sum: 100000000    time: 13.572171 ms
4 vars                         sum: 100000000    time: 13.019616 ms
5 vars                         sum: 100000000    time: 12.065758 ms
6 vars                         sum: 100000000    time: 12.040980 ms
7 vars                         sum: 100000000    time: 10.980458 ms
8 vars                         sum: 100000000    time: 10.882566 ms
12 vars                        sum: 100000000    time: 10.446088 ms


# -O2
# Vector Sum Benchmark — size: 10000000 #

cumulative (1 var)             sum: 100000000    time: 6.774345 ms
2 vars                         sum: 100000000    time: 5.021022 ms
3 vars                         sum: 100000000    time: 4.380572 ms
4 vars                         sum: 100000000    time: 4.258989 ms
5 vars                         sum: 100000000    time: 4.179113 ms
6 vars                         sum: 100000000    time: 3.412708 ms
7 vars                         sum: 100000000    time: 4.120415 ms
8 vars                         sum: 100000000    time: 3.779281 ms
12 vars                        sum: 100000000    time: 4.098870 ms

# -O3
# Vector Sum Benchmark — size: 10000000 #

cumulative (1 var)             sum: 100000000    time: 3.227233 ms
2 vars                         sum: 100000000    time: 3.445889 ms
3 vars                         sum: 100000000    time: 4.116750 ms
4 vars                         sum: 100000000    time: 3.766326 ms
5 vars                         sum: 100000000    time: 4.315747 ms
6 vars                         sum: 100000000    time: 3.423575 ms
7 vars                         sum: 100000000    time: 4.005223 ms
8 vars                         sum: 100000000    time: 3.579470 ms
12 vars                        sum: 100000000    time: 3.988477 ms



# Explicação compilação O0:
Nesta flag de compilação compilador não faz nenhuma otimização forçada de vetorização das instruções, então executa praticamente como é definido em código.
E com isto podemos perceber a perca de tempo que a soma acumulativa trás para o processador em sua pipeline de instruções 45ms em comparação aos 13ms da soma
com 5 vars, o que aliás das 6vars para baixo percebemos um platô de otimização, o que indica que o pipeline de instruções de meu processador é de 5 stages, isto é,
consegue executar 5 instruções de soma paralelas com ganho de performance, acima disso temos o teto do paralelismo de instrução (ILP) desta CPU.

Meu processador: 13th Gen Intel(R) Core(TM) i7-13620H
Specs de latência

Nas compilações de flag O2 e O3 o compilador já aplica vetorizações nas instruções, na compilação O2 ainda percebemos a melhoria de tempo
ao aplicar a estratégia de multiplas somas (exceto em número impar de variáveis que regride )
As unidades lógicas de processadores modernos processam dados em paralelo (SIMD) usando registradores largos (como 256 bits nas instruções AVX2). Isso significa processar pacotes exatos de 8 inteiros de 32 bits simultaneamente. O hardware é matematicamente otimizado para lidar com potências de 2.
o compilador mapeia as leituras de memória de forma limpa. Os blocos de dados se alinham perfeitamente com a largura dos barramentos e dos registradores.
Para processar esses blocos irregulares, o compilador não pode usar apenas uma leitura ampla de vetor


Agora na compilação O3 há pouca mudança durante as execuções, e aplicar múltiplas variáveis acaba sendo pior em alguns casos
instrui o compilador a ignorar a estrutura literal do código em C e reescrevê-lo da forma mais rápida possível para a máquina.
Em um laço simples de uma única variável (cumulative), o otimizador reconhece imediatamente o padrão matemático (redução de vetor). Ele converte a soma simples em código SIMD puro, calculando múltiplos elementos da matriz em um único ciclo de clock sem qualquer interferência,
Ao forçar dezenas de variáveis locais no código-fonte (s0, s1, s2...), o compilador é obrigado a respeitar essas dependências de dados. O algoritmo de vetorização precisa agora manter o estado individual de cada um desses acumuladores distintos.
