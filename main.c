/*
 * Operatsion tizimlar fani | 2-topshiriq | Variant 18
 * Mavzu : Katta sonlar (100+ xonali) yig'indisi - satr sifatida, dinamik xotirada
 *
 * Ikkita katta son satr sifatida o'qiladi (uzunligi oldindan noma'lum, realloc bilan
 * o'sadi), yig'indisi yangi dinamik satrda qaytariladi. Oxirida barcha xotira free qilinadi.
 *
 * Kompilyatsiya: gcc -Wall -Wextra -g -o main main.c
 * Tekshiruv    : valgrind --leak-check=full ./main
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOSH_HAJM 32 /* o'qish buferining boshlang'ich hajmi (keyin 2 barobar o'sadi) */

/*
 * Standart oqimdan butun qatorni o'qiydi. Xotira dinamik ajratiladi, kerak bo'lsa
 * realloc bilan kengayadi. Natija *satr ga yoziladi (ARGUMENT POINTER ORQALI O'ZGARADI).
 * Qaytish: 1 - ok, 0 - xotira yetmadi, -1 - EOF (hech narsa o'qilmadi).
 */
static int qator_oqish(char **satr)
{
    size_t sigim = BOSH_HAJM, n = 0;
    char *buf = malloc(sigim);
    int c;

    if (buf == NULL)
        return 0;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (n + 1 >= sigim) {
            char *yangi = realloc(buf, sigim * 2);
            if (yangi == NULL) {
                free(buf);
                return 0;
            }
            buf = yangi;
            sigim *= 2;
        }
        buf[n++] = (char)c;
    }
    if (c == EOF && n == 0) {
        free(buf);
        return -1;
    }
    if (n > 0 && buf[n - 1] == '\r')
        n--;
    buf[n] = '\0';
    *satr = buf;
    return 1;
}

/* Satr faqat raqamlardan iborat va bo'sh emasmi? */
static int raqamlarmi(const char *s)
{
    const char *p = s;
    if (*p == '\0')
        return 0;
    for (; *p != '\0'; p++)
        if (*p < '0' || *p > '9')
            return 0;
    return 1;
}

/* Boshidagi ortiqcha nollarni o'tkazib yuboradi ("000123" -> "123"; "000" -> "0"). */
static const char *nolsiz(const char *s)
{
    while (*s == '0' && *(s + 1) != '\0')
        s++;
    return s;
}

/*
 * Ikki katta son yig'indisi. Natija YANGI ajratilgan satrda qaytariladi
 * (chaqiruvchi free qilishi shart). Xotira yetmasa NULL.
 */
static char *katta_qosh(const char *a, const char *b)
{
    size_t la, lb, uzun, i;
    int carry = 0;
    char *tmp, *natija;
    const char *bosh;

    a = nolsiz(a);
    b = nolsiz(b);
    la = strlen(a);
    lb = strlen(b);
    uzun = (la > lb ? la : lb) + 1; /* +1: oxirgi ko'chirish uchun */

    tmp = malloc(uzun + 1); /* teskari tartibda yig'iladi */
    if (tmp == NULL)
        return NULL;

    for (i = 0; i < uzun; i++) {
        int s = carry;
        if (i < la)
            s += *(a + la - 1 - i) - '0';
        if (i < lb)
            s += *(b + lb - 1 - i) - '0';
        *(tmp + i) = (char)('0' + s % 10);
        carry = s / 10;
    }
    *(tmp + uzun) = '\0';

    /* teskari tartibni to'g'rilash va boshidagi nolni tashlash */
    natija = malloc(uzun + 1);
    if (natija == NULL) {
        free(tmp);
        return NULL;
    }
    for (i = 0; i < uzun; i++)
        *(natija + i) = *(tmp + uzun - 1 - i);
    *(natija + uzun) = '\0';
    free(tmp);

    bosh = nolsiz(natija);
    if (bosh != natija)
        memmove(natija, bosh, strlen(bosh) + 1);
    return natija;
}

/* Raqamlardan iborat son o'qiydi; noto'g'ri bo'lsa qayta so'raydi. 1 - ok, 0/-1 - xato/EOF. */
static int son_oqish(const char *taklif, char **son)
{
    for (;;) {
        int r;
        printf("%s", taklif);
        r = qator_oqish(son);
        if (r != 1)
            return r;
        if (raqamlarmi(*son))
            return 1;
        printf("Xato: faqat raqamlar (0-9) kiriting, bo'sh bo'lmasin, ishora va bo'shliqsiz.\n");
        free(*son);
        *son = NULL;
    }
}

/* Menyu tanlovi: 1 yoki 2. */
static int menyu_oqish(void)
{
    for (;;) {
        char *s = NULL;
        int r, v = 0;
        printf("\n=== KATTA SONLAR YIG'INDISI ===\n1. Ikki sonni qo'shish\n2. Chiqish\nTanlang (1-2): ");
        r = qator_oqish(&s);
        if (r != 1)
            return -1;
        if (strcmp(s, "1") == 0) v = 1;
        else if (strcmp(s, "2") == 0) v = 2;
        free(s);
        if (v != 0)
            return v;
        printf("Xato: 1 yoki 2 kiriting.\n");
    }
}

int main(void)
{
    int tanlov;

    while ((tanlov = menyu_oqish()) == 1) {
        char *a = NULL, *b = NULL, *yig = NULL;
        int r;

        r = son_oqish("1-son: ", &a);
        if (r == 1)
            r = son_oqish("2-son: ", &b);
        if (r == 0) {
            printf("Xato: xotira yetarli emas!\n");
            free(a);
            return 1;
        }
        if (r == -1) {
            free(a);
            free(b);
            break;
        }

        yig = katta_qosh(a, b);
        if (yig == NULL) {
            printf("Xato: xotira yetarli emas!\n");
            free(a);
            free(b);
            return 1;
        }
        printf("1-son uzunligi: %zu xona\n2-son uzunligi: %zu xona\n", strlen(a), strlen(b));
        printf("Yig'indi (%zu xona):\n%s\n", strlen(yig), yig);
        printf("Manzillar: 1-son = %p, 2-son = %p, yig'indi = %p\n",
               (void *)a, (void *)b, (void *)yig);

        free(a);
        free(b);
        free(yig);
        a = b = yig = NULL; /* osilib qolgan pointerdan saqlanish */
    }
    printf("Xotira bo'shatildi. Dastur tugadi.\n");
    return 0;
}
