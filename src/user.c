#include <osdefs.h>
#include <user_lib.h>

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

void TestosMain() {
    const char *s = "Hello. I am a user program in Ring3.\n";
    moveto(10, 20);
    while (*s) {
        putchar(*s++);
        sleep(100);
    }
    moveto(0, 0);
    printf("%d %c %% %x %b\n", 1, 'x', 0x233, 8);
    uint8_t c;
    srand(_sys_clock());
    puts("Now let's generate some random integers.Press Q to exit.");
    while (1) {
        c = getch();
        if (c == 'q') break;
        printf("%d\n", rand());
        sleep(1000);
    }
    puts("Exited");
    while (1) {}
}
