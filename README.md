# Jacobi-Richardson PCAM

O problema resume-se em:
 - Criar a matriz A que garanta convergência e criar o vetor coluna B
 - Calcular a matriz A* e B* (a partir de A e B)
 - Calcular o vetor coluna x a cada iteração (a partir de A* e B*)
 - Calcular o vetor Dif a cada iteração (a partir de x)
 - Calcular Mr a cada iteração (a partir de Diff)
 
##### Variáveis principais do problema

|variável |Escalar|linhas |colunas|comentário                           |
|----     |----   |----   |----   |---                                  |
|N        |Sim    |-      |-      |ordem do problema                    |
|A        |Não    |N      |N      |matriz do problema                   |
|A*       |Não    |N      |N      |matriz A alterada                    |
|B        |Não    |N      |1      |vetor coluna independente            |
|B*       |Não    |N      |1      |vetor coluna B alterado              |
|x        |Não    |N      |1      |vetor solução atual                  |
|x_new    |Não    |N      |1      |vetor solução novo                   |
|Dif      |Não    |N      |1      |diferença entre soluções consecutivas|
|Mr       |Sim    |-      |-      |erro                                 |

### PCAM

Serão utilizadas, no máximo, **T** threads.

#### Particionamento

O particionamento será realizado tanto por funcão quanto por dados.

Primeiramente, é necessário gerar **A** e **B**. Cada elemento de **A** e de **B** será gerado por uma tarefa distinta (ou seja, **N\*(N+1)** tarefas). Estas tarefas receberão o nome de **bloco 1**.

Após gerados todos os elementos, será realizada a análise de convergência a fim de saber se o método convergirá com a matriz **A** gerada. Para realizar o teste, será feita uma tarefa por linha **A**(ou seja, **N** tarefas). Este conjunto de tarefas será denominado **bloco 2**.
Para cada tarefa, enquanto o teste de convergência falhar, a respectiva tarefa incrementará um número aleatório em seu respectivo elemento da diagonal.

Após ter convergência assegurada, serão calculadas as matrizes **A\*** e **B\***.
Para o cálculo de **A\***, será utilizada uma tarefa para cada elemento de **A\*** (**N\*N** tarefas) e uma tarefa para cada elemento de **B\*** (**N** tarefas), sendo que este bloco de tarefas é denominado **bloco 3** (total de **N\*(N+1)** tarefas).

**N** tarefas (**bloco 4**) serão utilizadas para definir o valor inicial de **x**: uma tarefa para cada elemento de **x**. O valor inicial para **x** será uma cópia de **B\***.

Cálculo de cada novo valor de **x** (**x~new~**) será realizado pelo **bloco 5** de tarefas em que **N\*N** tarefas realizarão a multiplicação de cada uma das linhas de **A\*** pelos elementos de **x**. Então **N** tarefas realizarão a subtração de cada elemento de **B** com a soma das multiplicações das respectivas linhas.