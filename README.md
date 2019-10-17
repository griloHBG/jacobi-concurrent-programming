# Jacobi-Richardson PCAM

O problema resume-se em:
 - Criar a matriz A que garanta convergência e criar o vetor coluna B
 - Calcular a matriz A* e B* (a partir de A e B)
 - Calcular o vetor coluna x a cada iteração (a partir de A* e B*)
 - Calcular o vetor Dif a cada iteração (a partir de x)
 - Calcular Mr a cada iteração (a partir de Diff)
 
##### Variáveis principais do problema

|Variável  |Escalar|Linhas|Colunas|Comentário                           |
|----      |----   |----  |----   |---                                  |
|N         |Sim    |-     |-      |ordem do problema                    |
|A         |Não    |N     |N      |matriz do problema                   |
|A*        |Não    |N     |N      |matriz A alterada                    |
|B         |Não    |N     |1      |vetor coluna independente            |
|B*        |Não    |N     |1      |vetor coluna B alterado              |
|x         |Não    |N     |1      |vetor solução atual                  |
|x<sub>new</sub>    |Não    |N     |1      |vetor solução novo                   |
|Dif       |Não    |N     |1      |diferença entre soluções consecutivas|
|Mr        |Sim    |-     |-      |erro                                 |
|max<sub>x_new</sub>|Sim    |-     |-      |max elemento absoluto de x<sub>new</sub>      |
|max<sub>Dif</sub>  |Sim    |-     |-      |max elemento de Dif                  |

Precedência de variáveis:

![](.README_images/variables_precedence.png)

### PCAM

Serão utilizadas, no máximo, **T** threads.

#### Particionamento

*DISCLAIMER: o termo bloco foi utilizado para facilitar a referência aos conjuntos que apareceram juntos durante a fase de particionamento. **não necessariamente reflete o resultado da fase de aglomeração*** 

![](.README_images/partitioning.png)

O particionamento será realizado tanto por funcão quanto por dados.

Uma tarefa no início receberá/lerá o valor de **N**.

É necessário gerar **A** e **B**. Cada elemento de **A** e de **B** será gerado por uma tarefa distinta, ou seja, **N\*N** tarefas para **A** (**bloco 1**) e **N** tarefas para **B** (**bloco 2**)

Após gerados todos os elementos, será realizada a análise de convergência a fim de saber se o método convergirá com a matriz **A** gerada. Para realizar o teste, será feita uma tarefa por linha **A**(ou seja, **N** tarefas). Este conjunto de tarefas será denominado **bloco 3**.
Para cada tarefa, enquanto o teste de convergência falhar, a respectiva tarefa incrementará um número aleatório em seu respectivo elemento da diagonal.

Após ter convergência assegurada, serão calculadas as matrizes **A\*** e **B\***.
Para o cálculo de **A\***, será utilizada uma tarefa para cada elemento de **A\*** (**N\*N** tarefas, **bloco 4**) e uma tarefa para cada elemento de **B\*** (**N** tarefas, **bloco 5**)

**N** tarefas (**bloco 6**) serão utilizadas para definir o valor inicial de **x**: uma tarefa para cada elemento de **x**. O valor inicial para **x** será uma cópia de **B\***.

Será realizado pelo **bloco 7** de tarefas em que **N\*N** tarefas realizarão a multiplicação de cada uma das linhas de **A\*** pelos elementos de **x**, cujo resultado será utilizado para o cálculo de **x<sub>new</sub>**. Então **N** tarefas (**bloco 8**) realizarão a subtração de cada elemento de **B** com a soma das multiplicações das respectivas linhas, assim obtendo **x<sub>new</sub>** e, após isso, calcularão, cada uma, um elemento de **Dif**, que é dado pelo valor absoluto da subtração entre **x<sub>new</sub>** e **x**. Aproveitando que cada uma das **N** tarefas terão um valor para **x<sub>new</sub>** e para **Dif**, a variável **max<sub>x_new</sub>** armazenará o maior valor absoluto dentre os elementos de **x<sub>new</sub>** e a variável **max<sub>Dif</sub>** armazenará o maior valor dentre os elementos de **Dif**.

De posse de **max<sub>Dif</sub>** e **max<sub>x_new</sub>**, uma única tarefa (**bloco 9**) realizará a divisão entre aquele e este para obter **mr**. Se **mr** for menor que 0.001, então houve convergência. Caso contrário, o algoritmo voltará a executar as tarefas do **bloco 7** em diante.



#### Comunicação

A imagem abaixo ilustra as variáveis que precisam ser enviadas de um bloco de tarefas a outro.

![](.README_images/communication.png)

A tabela abaixo indica como cada bloco utiliza cada variável do problema

|Variável           |início |bloco 1|bloco 2|bloco 3|bloco 4|bloco 5|bloco 6|bloco 7|bloco 8|bloco 9|
|----               |----   |----   |----   |----   |----   |----   |----   |----   |----   |----   |
|N                  |   w   |   r   |   r   |   r   |   r   |   r   |   r   |   r   |   r   |   -   |
|A                  |   -   |   w   |   -   |  r/w  |   r   |   r   |   -   |   -   |   -   |   -   |
|A*                 |   -   |   -   |   -   |   -   |   w   |   -   |   -   |   r   |   -   |   -   |
|B                  |   -   |   -   |   w   |   -   |   -   |   r   |   -   |   -   |   -   |   -   |
|B*                 |   -   |   -   |   -   |   -   |   -   |   w   |   r   |   -   |   -   |   -   |
|x                  |   -   |   -   |   -   |   -   |   -   |   -   |   w   |   r   |   r   |   -   |
|x<sub>new</sub>    |   -   |   -   |   -   |   -   |   -   |   -   |   -   |   w   |   w   |   -   |
|Dif                |   -   |   -   |   -   |   -   |   -   |   -   |   -   |   -   |   w   |   -   |
|max<sub>x_new</sub>|   -   |   -   |   -   |   -   |   -   |   -   |   -   |   -   |   w   |   r   |
|max<sub>Dif</sub>  |   -   |   -   |   -   |   -   |   -   |   -   |   -   |   -   |   w   |   r   |
|Mr                 |   -   |   -   |   -   |   -   |   -   |   -   |   -   |   -   |   -   |   w   |

*No bloco 8 (ou no bloco 9) deve ocorrer o swap entre o


###### Início

A variável N é global, já que todos os outros blocos apenas realizam leitura.

###### Bloco 1: Gera Matriz A

Não há comunicação interna entre as tarefas deste bloco.

###### Bloco 2: Gera Vetor Coluna B

Não há comunicação interna entre as tarefas deste bloco.

###### Bloco 3: Altera Matriz A

Não há comunicação interna entre as tarefas deste bloco.

###### Bloco 4: Cria Matriz A*

Não há comunicação interna entre as tarefas deste bloco.

###### Bloco 5: Cria Vetor Coluna B*

Não há comunicação interna entre as tarefas deste bloco.

###### Bloco 6: Cria x (chute inicial)

Não há comunicação interna entre as tarefas deste bloco.

*Comentário: aqui eu percebi que particionamento por dados (cada tarefa em uma linha) deve ser mais vantajoso, mas vou continuar com particionamento por função para fazer diferente no que fiz nos outros trabalhos.*

###### Bloco 7: Começa cálculo de x<sub>new</sub>

Cada tarefa realiza a multiplicação de um elemento de **A\*** por um elemento de **x**. As multiplicações em uma mesma linha de **A\*** sofrem redução de soma, cujo resultado é armazenado na respectiva linha de **x<sub>new</sub>**.

###### Bloco 8: Cálculo de x<sub>new</sub>, Dif, max<sub>x_new</sub>, max<sub>Dif</sub>

Este bloco conclui o cálculo de cada elemento de **x<sub>new</sub>** e também calcula cada elemento de **Dif** e, durante este cálculo, realiza também a redução de max para encontrar o **max<sub>x_new</sub>** e o **max<sub>Dif</sub>**. 

###### Bloco 9: Cálculo de Mr

É uma única tarefa. O resultado deste cálculo é utilizado de sinalização para o algoritmo prosseguir ou parar. Essa sinalização é enviada para o **bloco 7**.



#### Aglomeração

O bloco 1 e o bloco 3 são realizados de maneira serial entre si, sendo que bloco 1 cria a matriz A e o bloco 3 altera a matriz A para que esta garanta a convergência do método. Ambos os blocos podem ser fusionados para que, ao fim da geração de cada linha de A, a estratégia de garantia de convergência já seja executada.
Ao mesmo tempo, a cada linha de A com convergência garantida, a respectiva linha de A* pode ser calculada, assim como o respectivo elemento de B* também pode ser calculado, que também pode já definir o valor inicial para x (que é baseado no valor de B*). Ou seja, Os blocos 1, 2, 3, 4, 5 e 6 tornam-se um bloco só.

O que não foi comentado, pernamece como está.

#### Mapeamento

O mapeamento fica a cargo do Sistema Operacional.


Todos os testes foram realizados em uma maquina com Core i7-8750H e 8Gb de RAM.

# Execução no Ubuntu 18.04


### Sequencial

| sequencial                 | ordem    | med          | stdev        |
|-------------------|------|--------------|--------------|
| Ordem do problema | 100  | 0\.041257667 | 0\.031657394 |
|                  | 500  | 4\.880552667 | 0\.823620177 |
|                  | 1000 | 30\.86822067 | 1\.857167229 |

### Paralelo

|Ordem do problema	|Qtdd de Threads	|Sppedup	        |    Eficiência     |
|-------------------|-------------------|-------------------|-----------------  |
|100	            |12	                | 2.083             |0.174              |
|	                |50	                | 3.953             |0.079              |
|	                |100	            | 0.129             |0.001              |
|500	            |12	                | 4.081             |0.340              |
|	                |50	                |14.136             |0.283              |
|	                |100	            |22.145             |0.221              |
|1000	            |12	                | 3.323             |0.277              |
|	                |50	                |12.647             |0.253              |
|	                |100	            |22.262             |0.223              |



![](.README_images/execution_graph.png)



| Ordem do problema | Qtdd de Threads | Sppedup      | Eficiência   |
|-------------------|-----------------|--------------|--------------|
| 100               | 12              | 2\.082914023 | 0\.173576169 |
|                  | 50              | 3\.952640991 | 0\.07905282  |
|                  | 100             | 0\.129236877 | 0\.001292369 |
| 500               | 12              | 4\.081396237 | 0\.340116353 |
|                  | 50              | 14\.13649051 | 0\.28272981  |
|                  | 100             | 22\.14487217 | 0\.221448722 |
| 1000              | 12              | 3\.322988522 | 0\.27691571  |
|                  | 50              | 12\.64744243 | 0\.252948849 |
|                  | 100             | 22\.26232546 | 0\.222623255 |

            
            
# Execução no Windows 10

### Sequencial

| ordem | med          | stdev        |
|-------|--------------|--------------|
| 100   | 0\.007933333 | 0\.000639684 |
| 500   | 1\.401766667 | 0\.169449145 |
| 1000  | 11\.0796     | 0\.426705309 |


### Paralelo

|      |      | threads      |              |              |              |              |              |
|-------|------|--------------|--------------|--------------|--------------|--------------|--------------|
|       |      | 12           |              | 50          |  |  100  |  |
|       |      | med          | stdev        | med          | stdev        | med          | stdev        |
| ordem | 100  | 0\.01243333  | 0\.006339166 | 0\.021766647 | 0\.007890156 | 1\.921700017 | 0\.069947858 |
|       | 500  | 0\.036366669 | 0\.007513158 | 0\.015566675 | 0\.000503986 | 0\.030666661 | 0\.002795719 |
|       | 1000 | 0\.186266653 | 0\.007109651 | 0\.062400007 | 0\.00049827  | 0\.046799994 | 0\.000406833 |

| ordem    |  threads   | Speedup      | Eficiência    |
|------|-----|--------------|---------------|
| 100  | 12  | 0\.638069865 | 0\.053172489  |
|     | 50  | 0\.364472004 | 0\.00728944   |
|     | 100 | 0\.004128289 | 4\.12829E\-05 |
| 500  | 12  | 38\.5453684  | 3\.212114034  |
|     | 50  | 90\.04920316 | 1\.800984063  |
|     | 100 | 45\.7097907  | 0\.457097907  |
| 1000 | 12  | 59\.48246687 | 4\.956872239  |
|     | 50  | 177\.5576717 | 3\.551153434  |
|     | 100 | 236\.7436226 | 2\.367436226  |
