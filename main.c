#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>

#define BITSIZE 8

/*
    В result записывается простое число по его порядковому номеру
*/
int generateRandomPrimeNumber(mpz_t *result, mpz_t index) {
    mpz_init(*result);
    mpz_set_ui(*result, 0);

    if (mpz_cmp_ui(index, 0) <= 0) {
        return 1;
    }

    while (mpz_cmp(*result, index) <= 0) {
        mpz_nextprime(*result, *result);
    }

    return 0;
}

int main() {
    int res;
    mpz_t indexP, p, eulerP, indexQ, q, eulerQ, n, euler, e, d, i, cmpEResult, mulResult, modResult;
    mpz_inits(indexP, p, eulerP, indexQ, q, eulerQ, n, euler, e, d, i, cmpEResult, mulResult, modResult, NULL);

    // RANDOM VARS
    gmp_randstate_t rstate;
    unsigned long int seed = time(NULL);

    gmp_randinit_default(rstate);
    gmp_randseed_ui(rstate, seed);
    mpz_urandomb(indexP, rstate, BITSIZE);
    mpz_urandomb(indexQ, rstate, BITSIZE);

    // p
    res = generateRandomPrimeNumber(&p, indexP);
    if (res) {
        printf("Error: generating prime number.\n");
        exit(1);
    }

    // q
    res = generateRandomPrimeNumber(&q, indexQ);
    if (res) {
        printf("Error: generating prime number.\n");
        exit(1);
    }

    // n
    mpz_mul(n, p, q);

    // Euler Function
    mpz_sub_ui(eulerP, p, 1);
    mpz_sub_ui(eulerQ, q, 1);
    mpz_mul(euler, eulerP, eulerQ);

    // opened key "e"
    for (mpz_set_ui(i, 2); mpz_cmp(i, euler) < 0; mpz_add_ui(i, i, 1)) {
        mpz_gcd(cmpEResult, i, euler);

        if (mpz_cmp_ui(cmpEResult, 1) == 0) {
            mpz_set(e, i);
            break;
        }
    }

    // closed key "d"
    for (mpz_set_ui(i, 2); mpz_cmp(i, euler) < 0; mpz_add_ui(i, i, 1)) {
        mpz_mul(mulResult, e, i);
        mpz_mod(modResult, mulResult, euler);

        if (mpz_cmp_ui(modResult, 1) == 0) {
            mpz_set(d, i);            
            break;
        }
    }

    gmp_printf("p = %Zd\n", p);
    gmp_printf("random index = %Zd\n", indexP);

    gmp_printf("q = %Zd\n", q);
    gmp_printf("random index = %Zd\n", indexQ);

    gmp_printf("n = %Zd\n", n);
    gmp_printf("euler = %Zd\n", euler);
    gmp_printf("opened key: e = %Zd n = %Zd\n", e, n);
    gmp_printf("closed key: d = %Zd n = %Zd\n\n", d, n);

    char ch = 'A', ch_dec;
    mpz_t encryptedMessage, decryptedMessage;
    mpz_set_ui(encryptedMessage, 1);
    mpz_set_ui(decryptedMessage, 1);

    for (mpz_set_ui(i, 0); mpz_cmp(i, e) < 0; mpz_add_ui(i, i, 1)) {
        mpz_mul_ui(mulResult, encryptedMessage, ch);
        mpz_mod(modResult, mulResult, n);
        mpz_set(encryptedMessage, mulResult);
        // encryptedMessage = (encryptedMessage * ch) % n;
    }

    for (mpz_set_ui(i, 0); mpz_cmp(i, d) < 0; mpz_add_ui(i, i, 1)) {
        mpz_mul(mulResult, decryptedMessage, encryptedMessage);
        mpz_mod(modResult, mulResult, n);
        mpz_set(decryptedMessage, mulResult);
        // decryptedMessage = (decryptedMessage * encryptedMessage) % n;
    }

    ch_dec = mpz_get_ui(decryptedMessage);

    printf("encrypted message = %c ", ch);
    gmp_printf("%Zd\n", encryptedMessage);
    printf("decrypted message = %c\n", ch_dec);

    mpz_clears(indexP, p, eulerP, indexQ, q, eulerQ, n, euler, e, d, i, cmpEResult, mulResult, modResult, NULL);
    gmp_randclear(rstate);

    exit(0);
}