#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <gmp.h>
#include <math.h>
#include <time.h>

#define SIZE 8192

void generateRandomPrimeNumber(mpz_t result, int bitsAmount) {
    gmp_randstate_t rstate;
    unsigned long int seed = clock();

    gmp_randinit_default(rstate);
    gmp_randseed_ui(rstate, seed);

    mpz_urandomb(result, rstate, bitsAmount);
    mpz_nextprime(result, result);

    gmp_randclear(rstate);
}

void encrypt(mpz_t e, mpz_t n, char message[SIZE], FILE *stream) {
    int r[SIZE];
    mpz_t messageToInt, encryptedMessage;
    mpz_inits(messageToInt, encryptedMessage, NULL);

    for (int i = 0; message[i] != '\0'; i++) {
        r[i] = (int)message[i];
        mpz_set_ui(messageToInt, r[i]);
        mpz_powm(encryptedMessage, messageToInt, e, n);
        gmp_fprintf(stream, "%Zd\n", encryptedMessage);
    }

    mpz_clears(messageToInt, encryptedMessage, NULL);
}

void decrypt(mpz_t d, mpz_t n, char (*resultPtr)[SIZE], FILE *stream) {
    char *res = *resultPtr;
    char buffer[SIZE];
    mpz_t encryptedMessage, decryptedMessage;
    mpz_inits(encryptedMessage, decryptedMessage, NULL);

    for (int i = 0; !feof(stream); i++) {
        if (fgets(buffer, SIZE, stream) == NULL) break;
        
        mpz_set_str(encryptedMessage, buffer, 10);

        mpz_powm(decryptedMessage, encryptedMessage, d, n);
        res[i] = (char)mpz_get_ui(decryptedMessage);
    }

    mpz_clears(encryptedMessage, decryptedMessage, NULL);
}

int main() {
    int bitsAmount, power;
    char message[SIZE], decryptedMessage[SIZE];
    mpz_t p, eulerP, q, eulerQ, n, euler, e, d, i;
    mpz_t messageToInt, encryptedMessage;
    FILE *encryption = fopen("result.txt", "w"), *decryption = fopen("result.txt", "r");

    printf("Введите степень 2 (от 10 до 16): ");
    scanf("%d", &power);
    if (power > 16 || power < 10) exit(1);
    
    mpz_inits(p, eulerP, q, eulerQ, n, euler, e, d, i, NULL);
    mpz_inits(messageToInt, encryptedMessage, decryptedMessage, NULL);

    bitsAmount = pow(2, power);

    generateRandomPrimeNumber(p, bitsAmount / 2);
    generateRandomPrimeNumber(q, bitsAmount / 2);

    // n
    mpz_mul(n, p, q);

    // Euler Function
    mpz_sub_ui(eulerP, p, 1);
    mpz_sub_ui(eulerQ, q, 1);
    mpz_mul(euler, eulerP, eulerQ);

    //generating opened key
    generateRandomPrimeNumber(e, bitsAmount);

    //generating closed key
    mpz_invert(d, e, euler);

    gmp_printf("p = %Zd\n", p);
    gmp_printf("q = %Zd\n", q);
    gmp_printf("n = %Zd\n", n);
    gmp_printf("euler = %Zd\n", euler);
    gmp_printf("открытый ключ:\ne = %Zd\nn = %Zd\n", e, n);
    gmp_printf("закрытый ключ:\nd = %Zd\nn = %Zd\n", d, n);

    //RSA algorithm
    printf("\nВведите сообщение: ");
    scanf("%s", message);

    encrypt(e, n, message, encryption);
    fclose(encryption);

    decrypt(d, n, &decryptedMessage, decryption);
    fclose(decryption);
    
    printf("Расшифрованное сообщение: %s\n", decryptedMessage);

    mpz_clears(p, eulerP, q, eulerQ, n, euler, e, d, i, NULL);

    return 0;
}