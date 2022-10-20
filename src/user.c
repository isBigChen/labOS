#include <osdefs.h>
#include <user_lib.h>

char inputbuf[256] = {0};

void clear_inputbuf() {
    for (int i = 0; i < 256; i++) {
        inputbuf[i] = 0;
    }
}

void task1() {
    int i = 0;
    while (1) {
        printf("Task 1: %d\n", i++);
        sleep(100);
    }
}

void task2() {
    int i = 0;
    while (1) {
        printf("Task 2: %d\n", i++);
        sleep(100);
    }
}

void task3() {
    int i = 0;
    while (1) {
        printf("Task 3: %d\n", i++);
        sleep(100);
    }
}


/* ---------- */

// <<K&R C>> 2.7节 的伪随机数函数.
uint32_t next = 1;
/* rand: return pseudo-random integer on 0..32767 */
int rand(void) {
    next = next * 1103515245 + 12345;
    return (uint32_t)(next/65536) % 32768;
}
/* srand: set seed for rand() */
void srand(uint32_t seed) {
    next = seed;
}

void myfunc(){
    puts("Please input something");
    gets(inputbuf); 
    puts(inputbuf);
}

void banner() { 
    printf("Welcome to ");
    puts_color("labOS :)", Cyan, Black);
    puts("");  
    puts("Type \"help\" for more infomation.");
}

int run_flag = 0;

void TestosMain() {
    if (!run_flag)
        banner();
    run_flag = 1;
    add_task(task1);
    add_task(task2);
    add_task(task3);
shell:
    while (1) {
        puts_color("shell> ", Light_Blue, Black);
        gets(inputbuf);
        if (strcmp(inputbuf, "func") == 0) {
            goto func;
        } else if (strcmp(inputbuf, "switch") == 0) {
            goto switch_test;
        } else if (strcmp(inputbuf, "help") == 0) {
            puts("Help list for labOS:");
            puts("  help - Display this message.");
            puts("  cls - Clear the screen.");
            puts("  banner - Display a beautiful banner.");
            puts("  func - Call myfunc.");
            puts("  switch - Show task switch test in 5 second.");
        } else if (strcmp(inputbuf, "banner") == 0) {
            banner();
        } else if (strcmp(inputbuf, "cls") == 0) {
            clsscr();
            moveto(0, 0);
        }
        else {
            printf("! Unknown command: %s\nType \"help\" for more infomation.\n", inputbuf);
        }
        clear_inputbuf();
    }
func:
    myfunc();
    clear_inputbuf();
    goto shell;
switch_test:
    // 交互运行task1, task2, task3大概5秒后返回到TestosMain开始
    // 具体见int_handlers.c时钟中断处理
    switch_enable(TRUE);
    goto shell;
}
