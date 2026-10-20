# Aula 6 — Processos, sinais e threads em Linux (versão Claude)

Resolução completa do guião *Laboratory Module: Linux Processes, Signals and Threads*
(SETR 2026/27). Os exercícios da Parte 1 já tinham uma primeira versão em
[`../Aula5`](../Aula5); essa pasta não foi alterada.

## Compilar e testar

Tudo usa `fork()`, `exec`, sinais e pthreads, por isso tem de correr em **Linux ou WSL**
(não funciona com o MinGW do Windows).

```bash
make          # compila todos os exercícios
make check    # corre todos os programas e verifica o que imprimem (72 verificações, ~1,5 min)
make clean    # apaga as pastas build/
```

Cada pasta tem o seu Makefile, por isso cada exercício compila e corre sozinho:
`make -C ex4_2 run`, `make -C ex5_2 run N_ITERATIONS=10`, etc. Os executáveis ficam em
`exN_M/build/`.

O `check.sh` (usado pelo `make check`) lança os programas dos sinais em segundo plano e
envia-lhes os sinais com `kill`, tal como o guião sugere, e confirma a ordem das mensagens.

## Exercícios

| Exercício | Pasta | Conteúdo |
|---|---|---|
| 1.1 myPS | `ex1_1/` | `system("ps aux")` |
| 1.2 wc | `ex1_2/` | `mywc <ficheiro>`: número de caracteres e de palavras, com `wc -m` e `wc -w` via `system()` |
| 1.3 createFold | `ex1_3/` | `createFold Group 10` cria `Group1` … `Group10` com `mkdir -p` |
| 1.4 deleteFold | `ex1_4/` | `deleteFold [-i] Group 10` apaga as pastas (mesmo não vazias) com `rm -rf`; `-i` pergunta antes de cada uma |
| 2.1 myPS (exec) | `ex2_1/` | `execlp("ps", "ps", "aux", NULL)` |
| 2.2 wc (exec) | `ex2_2/` | `fork` + `execlp("wc", ...)`; o pai lê os números por um `pipe` e imprime-os como em 1.2 |
| 2.3 create/deleteFold (exec) | `ex2_3/` | 1.3 e 1.4 com `fork` + `execvp` + `waitpid` (`run_command()` em `common/utils.c`) |
| 3.1 fork | `ex3_1/` | Pai e filho mostram o seu PID e o do outro, e avisam que vão terminar |
| 3.2 fork + wait | `ex3_2/` | O filho trabalha 3 s; o pai espera com `waitpid`. `--no-wait` mostra o que acontece sem isso |
| 4.1 sigaction | `ex4_1/` | Apanha SIGINT, SIGHUP, SIGUSR1 e SIGCONT e diz qual chegou |
| 4.2 Timer | `ex4_2/` | Conta segundos; SIGINT = reset, SIGHUP = suspende, SIGCONT = retoma |
| 4.3 Timer que ignora | `ex4_3/` | O mesmo, mas todos os outros sinais são ignorados (`SIG_IGN`) e os três sinais do timer estão no `sa_mask` |
| 4.4 kill | `ex4_4/` | O pai envia 10 × SIGUSR1 ao filho, um por segundo; o filho termina ao receber o 10.º |
| 4.5 sa_mask | `ex4_5/` | SIGUSR2 fica bloqueado enquanto o handler de SIGUSR1 corre (3 s). `--no-mask` para comparar |
| 5.1 pthreads | `ex5_1/` | Duas threads (2 s e 3 s) e `pthread_join` |
| 5.2 Variável partilhada | `ex5_2/` | Uma thread incrementa um contador por segundo, a outra deteta a mudança por flag + polling; `N_ITERATIONS` definido na compilação |
| 5.3 Sinais em threads | `ex5_3/` | O main envia SIGUSR1 a cada thread com `pthread_kill`, com 1 s de intervalo, e espera que terminem |

`common/` tem o código partilhado das Partes 1 e 2 (`utils.c/.h`) e as opções de compilação
comuns a todos os Makefiles (`common.mk`).

## Exemplos

```text
$ make -C ex3_2 run ARGS=--no-wait
[ 0.0 s] Parent (PID 1203) starting
[ 0.0 s] Parent (PID 1203) created child PID 1204
[ 0.0 s] Process PID 1203 is finishing now
[ 0.0 s] Child (PID 1204, parent 1) starting, working for 3 s
[ 1.0 s] Child working... 1/3 (my parent is now PID 1)
...
```

Sem `--no-wait` o pai fica em `waitpid()` e só termina depois do filho. Com `--no-wait` o pai
termina logo e o filho fica órfão: passa a ser filho do `init` (PID 1) ou de outro processo
"subreaper".

```text
$ ./ex4_2/build/timer &            # e depois kill -HUP, kill -CONT, kill -INT
0 s
1 s
2 s
SIGHUP: counting suspended at 2 s
SIGCONT: counting resumed at 2 s
3 s
SIGINT: reset, counting again from 0
1 s
```

```text
$ make -C ex4_5 run                 # SIGUSR1 e, 1 s depois, SIGUSR2
SIGUSR1 handler: start, counter = 1
SIGUSR1 handler: end
SIGUSR2 handler: runs now (counter = 1)

$ make -C ex4_5 run ARGS=--no-mask
SIGUSR1 handler: start, counter = 1
SIGUSR2 handler: runs now (counter = 1)
SIGUSR1 handler: end
```

## Notas sobre as soluções

- **`system()` e nomes com espaços.** O `system()` passa o texto a `/bin/sh`, por isso um nome
  como `it's a $file; rm -rf ~` seria interpretado pela shell. Os programas da Parte 1 põem os
  nomes entre plicas (`shell_quote()`). Com `exec` (Parte 2) isto não é preciso, porque cada
  argumento chega ao programa tal como está, sem shell no meio.
- **`rmdir` vs `rm -r`.** O `rmdir` só apaga pastas vazias e não tem opção para apagar o
  conteúdo (`-p` apaga as pastas-pai e `--ignore-fail-on-non-empty` só esconde o erro). Para
  pastas com ficheiros é preciso `rm -r`.
- **`fflush(stdout)` antes de `fork()`.** O `fork()` copia o buffer do `stdout`: se ainda lá
  estiver texto, é impresso duas vezes, uma por cada processo.
- **Handlers de sinais.** Dentro de um handler só se podem chamar funções
  *async-signal-safe*, e o `printf` não é uma delas. Nos timers o handler só muda flags
  (`volatile sig_atomic_t`) e quem imprime é o ciclo principal. No 4.1 e no 4.5 o handler
  escreve com `write()`.
- **`pause()` tem uma corrida.** Em `while (!flag) pause();`, se o sinal chegar entre o teste
  e o `pause()`, o programa fica parado para sempre. Por isso, nos programas que esperam por
  um sinal específico (4.2, 4.3, 4.4 e 5.3), o sinal é bloqueado e a espera é feita com
  `sigsuspend()`, que desbloqueia e espera numa só operação atómica. O 4.1 e o 4.5 usam
  `pause()` diretamente, porque aí não há nada a testar antes de esperar.
- **4.3: o que é ignorado.** Todos os sinais recebem `SIG_IGN`, exceto SIGKILL e SIGSTOP (o
  kernel não deixa) e os que o próprio CPU gera quando o programa falha (SIGSEGV, SIGFPE,
  SIGILL, SIGBUS…). Ignorar estes faria com que um erro de memória ficasse em ciclo infinito.
  Por isso, o timer do 4.3 só termina com `kill -KILL <PID>`.
- **4.4: handler antes do `fork()`.** O handler é instalado e o SIGUSR1 é bloqueado antes do
  `fork()`. Assim, um sinal que chegue antes de o filho estar pronto fica pendente, em vez de
  matar o filho com a ação por omissão.
- **5.2: mutex.** O guião pede uma flag com polling. A flag e o contador são acedidos com um
  mutex, porque sem ele há uma *data race* (comportamento indefinido em C): a thread que
  imprime podia ver a flag a 1 antes do novo valor do contador. Entre consultas, a thread
  dorme 10 ms, para o polling não gastar 100 % de CPU.
- **5.3: sinais e threads.** Os handlers são do processo inteiro, mas o `pthread_kill()`
  entrega o sinal a uma thread específica e o handler corre nessa thread. A flag que o handler
  muda é `_Thread_local`, por isso cada thread só vê os sinais que lhe foram enviados a ela.
