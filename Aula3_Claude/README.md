# Aula 3 — C, programação modular, Makefiles e testes unitários (versão Claude)

Resolução completa do guião *Review of C, Modular Programming in C, Makefiles and
Unit Testing* (SETR 2026/27), feita com o Claude a partir do trabalho que estava em
[`../Aula3`](../Aula3). Essa pasta não foi alterada.

## Compilar e testar

Em Linux / WSL:

```bash
make          # compila todos os exercícios
make test     # corre os 61 testes Unity
make check    # corre todos os programas (cada um verifica os seus resultados) e os testes
make clean    # apaga as pastas build/
```

No Windows, no Git Bash com o MinGW: `mingw32-make`, `mingw32-make test`, etc.

Cada pasta tem também o seu Makefile, por isso cada exercício compila sozinho
(`make`, `make run` e, quando há testes, `make test`).

## Exercícios

| Exercício | Pasta | Conteúdo |
|---|---|---|
| 1.1 Keeping local | `ex1_1/` | `MyRand()` com contador `static` local (sem variáveis globais) e o teu módulo `cal` (sum/sub) |
| 1.2 Sharing variables | `ex1_2/` | `maxConnections` partilhada com `extern`; o `main` lê-a e altera-a diretamente |
| 1.3 Interfaces | `ex1_3/` | `maxConnections` escondida com `static`, acesso só por `configInit/Set/Get`, com 6 testes Unity |
| 2.1 Dynamic array | `ex2_1/` | `random_array_stats()`: `malloc`, valores em [1, M], média, máximo e mínimo (`make run ARGS="N M seed"`) |
| 2.2 Swap | `ex2_2/` | `swap_int(int *, int *)` e `swap_int_ptr(int **, int **)` |
| 2.3 String reversal | `ex2_3/` | `str_reverse()` só com aritmética de ponteiros |
| 3.1 Linked list | `ex3_1/` | Lista ligada num só ficheiro com insert/delete/print (usa `Node **`) |
| 3.2 Modular list | `ex3_2/` | `linked_list.c/.h` com tipo opaco, com 16 testes Unity |
| 4 Word Analyzer | `ex4_word_analyzer/` | `main.c`, `file_utils.c/.h`, `word_list.c/.h` e Makefile com compilação separada; ordena por frequência; 39 testes Unity |

## Word Analyzer

```text
$ make -C ex4_word_analyzer run N=5
10 lines
105 words
627 characters
Word frequency:
- the: 9
- in: 6
- and: 5
- a: 3
- must: 3
- ... (65 more)
```

Os três totais são iguais aos de `wc -l -w -m data/sample.txt`. Definições usadas:

- **carácter**: um carácter Unicode, incluindo espaços e mudanças de linha (como `wc -m`);
- **linha**: termina em `\n`; uma última linha sem `\n` também conta;
- **palavra**: sequência de letras (acentuadas incluídas) e dígitos. Um apóstrofo ou hífen
  entre letras faz parte da palavra (`don't`, `real-time`). As palavras são guardadas em
  minúsculas.

O texto é lido como UTF-8, por isso "Ação", "ação" e "AÇÃO" contam como a mesma palavra
(experimenta `make -C ex4_word_analyzer run FILE=data/exemplo_pt.txt`). Ficheiros em Latin-1
também funcionam. Se houver empate na frequência, as palavras ficam por ordem alfabética.
A ordenação é um merge sort feito sobre a própria lista ligada.

Limitações:

- A procura numa lista ligada é O(n). Um ficheiro de 1,5 MB com 17 000 palavras
  distintas demora cerca de 3 s; para ficheiros grandes, o passo seguinte seria uma
  tabela de hash.
- Nos empates, a ordem alfabética é a dos bytes (`strcmp`), por isso as palavras que
  começam por uma letra acentuada aparecem depois do "z".

## O que mudou em relação à pasta Aula3

- **`ex1.c`**
  - `printf(..., MyRand(&x, &y))` não compilava, porque `MyRand` devolve `void`.
  - O enunciado não permite variáveis globais, e o contador era mantido pelo `main`.
    Agora é um `static` dentro de `MyRand`.
  - O intervalo passou a ser 0–100 com `rand() % 101`. Antes era `rand() % 100 + 1`,
    que dá 1–100. Foi também adicionado o `srand()`.
  - `sum` e `sub` estavam definidas tanto em `ex1.c` como em `cal.c` (definição
    múltipla ao ligar os dois) e agora ficam só em `cal.c`.
- **`ex1.h`**: faltava o `;` no protótipo. `ex1.h` e `cal.h` têm agora include guards.
- **`configData`**: os exercícios 1.2 e 1.3 estavam no mesmo módulo, com a variável
  `extern`. No 1.3 a variável tem de ficar escondida (`static`), por isso cada exercício
  tem agora a sua pasta. No 1.3, `maxConnections = 10;` no `main` dá erro de compilação,
  e declará-la `extern` dá erro de ligação.
- **`restoremaxConnections(&maxConnections)`** passou a ser `configInit()`. O módulo já é
  dono da variável e não precisa de um ponteiro, e o parâmetro escondia a variável
  global com o mesmo nome.
- **`makefile`**: o alvo `exer 1_2` (com espaço) criava dois alvos e passava `1_2` ao
  gcc como ficheiro de entrada. Agora cada pasta tem o seu Makefile.
- **`printf`**: os `uint16_t` são impressos com `%u` (antes com `%d`).

## Notas

- O Unity 2.5.4 (licença MIT) está copiado em `Unity/`. No repositório, o `Aula2/Unity`
  é um gitlink sem `.gitmodules` e aparece vazio quando se clona do GitHub. Para usar
  outra cópia: `make test UNITY_ROOT=/caminho/para/Unity`.
- Foi verificado com gcc 13 (Ubuntu no WSL) e com MinGW gcc 8.1 (Windows). Compila sem
  avisos com `-Wall -Wextra -Wpedantic` e os restantes avisos dos Makefiles. O
  AddressSanitizer e o UBSan não detetaram fugas de memória nem comportamento
  indefinido.
