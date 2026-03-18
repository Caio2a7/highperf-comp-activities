Implemente um programa em C que calcule uma aproximação de π por meio de uma série matemática variando o número de iterações e medindo o tempo de execução. 

Compare os valores obtidos com o valor real de π e analise como a acurácia melhora com mais processamento. 

Reflita sobre como esse comportamento se repete em aplicações reais que exigem resultados cada vez mais precisos
como simulações físicas e inteligência artificial.

----
 * Executando para 10000000 iterações * 
- Resultado: 3.141592553589792
- Tempo executado: 189.0304 ms

 * Executando para 100000000 iterações * 
- Resultado: 3.141592643589326
- Tempo executado: 1869.3956 ms

 * Executando para 1000000000 iterações * 
- Resultado: 3.141592652588050
- Tempo executado: 18549.4253 ms

Só isto já trás muita reflexão, a escala exponencial da iterações escala exponencialmente com o tempo também
10 milhões - 0.189s
100 milhões - 1.869s
1 bilhão - 18.549s

Com 1 bilhão de iterações conseguimos precisão de 9 casas decimais, acima disto é impreciso  
3.141592652... - 3.141592653... (pi retirado da internet)

----
1 bilhão de iterações, 18 segundos, série matemática simples com precisão de 9 casas decimais.

Considerando cenários reais de simulações físicas de quantidade imensa de dados, com números inimagináveis de variáveis, operações e casas decimais de precisão
esse tempo não está aceitável, pois executamos essa série matemática em apenas uma "partícula" imagino se fosse X partículas
para executar esta série 1 bilhão de vezes, 18s * 1 milhão de partículas = 18.000.000 segundos = 208,33 dias

E todo este tempo para apenas um tipo de operação executada, claramente inviável para aplicações de simulações físicas extremas e IA
