/*
 * Shamir secret sharing application
 *
 * Program that divides the secret into shares based on input threshold.
 * If enough shares put in, it reconstructs secret.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include <limits.h>

#define GLOBAL_THRESHOLD gThreshold
#define GLOBAL_NUM_SHARES gNumShares
#define GLOBAL_SECRET_LENGTH gSecretLen
#define GLOBAL_SHARES_LENGTH gSharesLen
#define GLOBAL_SECRET gSecret
#define GLOBAL_SHARES gShares

/* Defines */
#define PRIME_NUMBER 257

/* Global variables */
int GLOBAL_THRESHOLD;
int GLOBAL_NUM_SHARES;
int GLOBAL_SECRET_LENGTH;
int GLOBAL_SHARES_LENGTH;
char* GLOBAL_SECRET;
char* GLOBAL_SHARES;
char *end;
char buffer[3*1024 + 7];

/* Function prototypes */
char DecodeFromInt(int intToDecode);
void ClearConsole();
void PrintCLIHeader();
void PrintCLIMainMenu();
void PrintCLIGetInput();
void GetCLIInput();
void StartCLI();
int LoadShares();
int MathMod(int number, int prime);
int* GCD(int a, int b);
int ModInverse(int k, int prime);
int LargInterProduct(int i, int k, int* x, int* fx, int fieldChar);
int LagrInter(int* x, int* fx, int threshold, int fieldChar);
int IntPow(int base, int exp);
int StringCharToInt(char* string, int strLen);
int ReconstructSecret();
int EncodeToInt(char charToEncode);
int FinitieFieldPow(int base, int exponent, int fieldChar);
int EvalPolyAt(int x, int degree, int* coeffs);
int GenerateShares();
void LoadDataForGenerating();
void TerminateApplication();
void CheckDataAndGenerate(int shouldRedirect);
void ErrFileInput(char* message);
void ErrCLI(char* message);
void PresKeyToContinue();

/**
 * DecodeFromInt()
 *
 * Decodes character from integer.
 *
 * @param   int    Character to be uniquely encoded into integer.
 * @return  char     Encoded char as integer.
 */
char DecodeFromInt(int intToDecode)
{
    char c = intToDecode;
    return c;
}


/**
 * ClearConsole()
 *
 * Clears console screen
 * ---
 * Pointless now, but ready for extension for nCurses-like CLI environment.
 *
 * @param   void
 * @return  void
 */
void ClearConsole()
{
    system("cls");
    return;
}


/**
 * PrintCLIHeader()
 *
 * Prints CLI screen header.
 *
 * @param   void
 * @return  void
 */
void PrintCLIHeader()
{
    printf("\n\n\t-----------------------------\n");
    printf("\t=== Shamir secret Sharing ===\n");
    printf("\t-----------------------------\n");
    printf("\n\n");
    return;
}

/**
 * PrintCLIMainMenu()
 *
 * Prints CLI screen for main menu.
 *
 * @param   void
 * @return  void
 */
void PrintCLIMainMenu()
{
    printf("\t\t== MENU ==\n\n");
    printf("\t1) Reconstruct secret from shares\n");
    printf("\t2) Generate shares\n");
    printf("\t  ---\n");
    printf("\t3) Tell me joke!\n");
    printf("\t  ---\n");
    printf("\tQ) Exit\n");
    printf("\n\n\t$>");
    return;
}

/**
 * PrintCLIGetInput()
 *
 * Prints CLI screen for getting shares.
 *
 * @param   void
 * @return  void
 */
void PrintCLIGetInput()
{
    printf("Input your shares, one per line and then send in EOF (CTRL + Z and ENTER):\n");
    return;
}

/**
 * GetCLIInput()
 *
 * Reads user input from console and starts particular functions.
 *
 * @param   void
 * @return  void
 */
void GetCLIInput()
{
    char input;

    scanf(" %c", &input);
    getc(stdin); /* Remove extra newline char */

    switch ((int)input)
    {
    case '1':
        ClearConsole();
        PrintCLIGetInput();
        if (LoadShares())
        {
            ReconstructSecret();
        }
        break;

    case '2':
        LoadDataForGenerating();
        break;

    case '3':
        printf("\n\n\tMan asks friend: \"Usually in programming, 1 stands fo true and 0 for false, right?\"\n\tFriend replies: \"1.\"\n\n\n");
        break;

    case 'Q':
    case 'q':
        TerminateApplication();
        break;

    }

    return;
}


/**
 * StartCLI()
 *
 * Start application Comand Line Interface
 *
 * @param   GLOBALS: GLOBAL_SECRET, GLOBAL_NUM_SHARES, GLOBAL_THRESHOLD
 * @param   void
 * @return  void
 */
void StartCLI()
{
    while (1) {
        ClearConsole();
        PrintCLIHeader();
        PrintCLIMainMenu();
        GetCLIInput();

        PresKeyToContinue();
    }
    return;
}


/**
 * LoadShares()
 *
 * Loads shares from stdin to global variable.
 *
 * @param   void
 * @return  int      End state of function.
 *                      1   - Load successful.
 *                      -1  - Memory allocation failed.
 */
int LoadShares()
{
    fgets(buffer, 3*1024 + 7, (FILE*)stdin);
    buffer[strcspn(buffer, "\r\n")] = 0; /* Replaces newline chars with null byte */

    /* Use first share for getting info about threshold and length of secret */
    GLOBAL_SHARES_LENGTH = strlen(buffer) + 1; /* +1 for null byte */
    GLOBAL_THRESHOLD = (((buffer[3] - '0') * 100) + ((buffer[4] - '0') * 10) + ((buffer[5] - '0') * 1));
    if (GLOBAL_THRESHOLD < 1)
    {
        fprintf(stdout, "Wrong input!\n");
        StartCLI();
    }
    /* Allocate memory for needed number of shares */
    GLOBAL_SHARES = malloc((GLOBAL_SHARES_LENGTH)* GLOBAL_THRESHOLD);

    memcpy((GLOBAL_SHARES), &buffer, GLOBAL_SHARES_LENGTH);

    if (GLOBAL_SHARES == 0)
    {
        return -1;
    }

    /* Load shares into memory */
    int i = 1;
    while(fgets(buffer, 3*1024 + 7, (FILE*)stdin))
    {
        if (GLOBAL_THRESHOLD > i) {
            /* Substitute ending newlines with null bytes */
            buffer[strcspn(buffer, "\r\n")] = 0;

            /* Copy shares to allocated memory */
            memcpy((GLOBAL_SHARES + (i * GLOBAL_SHARES_LENGTH)), &buffer, GLOBAL_SHARES_LENGTH);
        }
        ++i;
    }
    return 1;
}

/**
 * MathMod()
 *
 * Does standard modulo operation (not remainder after division as % does)
 * ---
 * Just handles negative remainders by adding it and doing %.
 *
 * @param   int      Number to be modulated.
 * @param   int      Prime number to be modulated with.
 * @return  int      Result of modulo.
 */
int MathMod(int number, int prime)
{
    return  (number + prime) % prime;
}


/**
 * GCD()
 *
 * Finds greatest common divider of two numbers.
 *
 * @param   int     First number.
 * @param   int     Second number.
 * @return  int*    Returns pointer to array of meta results.
 */
int* GCD(int a, int b)
{
	int* xyz = malloc(sizeof(int) * 3);
	if (b == 0) {
		xyz[0] = a;
		xyz[1] = 1;
		xyz[2] = 0;
	} else {
		int n = floor(a / b);
		int c = a % b;
		int* r = GCD(b, c);

		xyz[0] = r[0];
		xyz[1] = r[2];
		xyz[2] = r[1] - r[2] * n;

		free(r);
	}
	return xyz;
}


/**
 * ModInverse()
 *
 * Finds modular inverse for provided number to do finite field division with multiplication.
 *
 * @param   int     Number we want to find modular inverse for.
 * @param   int     Characteristics of finite field.
 * @return  int     Returns modular inverse for number k in finite field with characteristics prime.
 */
int ModInverse(int k, int prime)
{
	int r;
	int* xyz;

	if (k < 0) {
		xyz = GCD(prime, -k);
		r = -xyz[2];
	} else {
		xyz = GCD(prime, k);
		r = xyz[2];
	}
	free(xyz);
	return (prime + r) % prime;
}

/**
 * LargInterProduct()
 *
 * Subfunction for Lagrange's interpolation. Implementation of product
 * in L(0) formula.
 * ---
 * Inspired by (https://en.wikipedia.org/wiki/Shamir%27s_Secret_Sharing)
 *
 * @param   int     i           Index sent from above sum.
 * @param   int     k           Threshold value.
 * @param   int*    x           Array of function parameters (same order as fx[])
 * @param   int*    xy          Array of function values (same order as x[])
 * @param   int     fieldChar   Characteristics of the finite field
 * @return  int               Result of product.
 */
int LargInterProduct(int i, int k, int* x, int* fx, int fieldChar)
{
    int  p = 1, q = 1;
    int j;
    for (j = 0; j < k; ++j)
    {
        if (j == i)
            continue;

        p = (p *  x[j] ) % fieldChar;
        q = (q * (x[j] - x[i]) ) % fieldChar;

    }
    /* Behaves like p/q but in finite field, so we can mod denominator and numerator  */
    return MathMod((p * ModInverse(q, fieldChar)), fieldChar);
}


/**
 * LagrInter()
 * (Lagrange's Inpterpolation)
 *
 * Finds 0th coefficient (secret) in Lagrange's polynomial.
 * ---
 * Uses reduced formula for computing just 0th coefficient using
 * Lagrange's polynomial interpolation (that 'k' values define
 * unambiguously Lagrange's polynomial of 'k - 1' degree).
 * Inspired by (https://en.wikipedia.org/wiki/Shamir%27s_Secret_Sharing)
 *
 * @param   int*    x           Array of function parameters (same order as fx[])
 * @param   int*    xy          Array of function values (same order as x[])
 * @param   int     threshold   Number of shares needed to reconstruct (will be constructing polynomial of threshold - 1 degree)
 * @param   int     fieldChar   Characteristics of the finite field
 * @return  int                 Returns secret as integer.
 */
int LagrInter(int* x, int* fx, int threshold, int fieldChar)
{
    int result = 0;
    int lal;
    int i;
    for (i = 0; i < threshold; ++i)
    {
        lal = LargInterProduct(i, threshold, x, fx, fieldChar);
        result = MathMod((result + (fx[i] * lal)), fieldChar);
    }
    return result;
}


/**
 * IntPow()
 *
 * Integer exponentiation.
 * ---
 * Implemented as squaring method.
 *
 * @param   int     Base value.
 * @param   int     Exponent value.
 * @return  int     Result of exponentiation.
 */
int IntPow(int base, int exp)
{
    int result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp =  exp >> 1; /* Integer division by 2 */
        base = base * base;
    }
    return result;
}

/**
 * StringCharToInt()
 *
 * Converts character represented as string into its numeral ASCII value.
 *
 * @param   char*   String to be converted.
 * @param   int     Length of string.
 * @return  int     Returns string as integer.
 */
int StringCharToInt(char* string, int strLen)
{
    int result = 0;
    int i;

    for (i = 0; i < strLen; ++i)
    {
        result = result + (string[i]  - '0') * (IntPow(10, (strLen - (i + 1))));
    }
    return result;
}

/**
 * ReconstructSecret()
 *
 * Reconstructs secret from provided shares.
 *
 * @param   GLOBALS: GLOBAL_SECRET, GLOBAL_NUM_SHARES, GLOBAL_THRESHOLD
 * @param   void
 * @return  int         Returns reconstruction function end state.
 */
int ReconstructSecret()
{
    GLOBAL_SECRET_LENGTH = GLOBAL_SHARES_LENGTH / 3 - 2;
    char secret[GLOBAL_SECRET_LENGTH];

    int params[GLOBAL_THRESHOLD];
    int values[GLOBAL_THRESHOLD];

    char node[4];
    char c;

    int i, j;
    int secretInt;

    /* Load params from shares */
    for (i = 0; i < GLOBAL_THRESHOLD; ++i)
    {
        memcpy(&node, GLOBAL_SHARES + (i * GLOBAL_SHARES_LENGTH), 3);
        node[3] = 0; /* Add null byte */
        params[i] = StringCharToInt(node, 3);
    }

    /* Load values of first char */
    for (j = 0; j < (GLOBAL_SECRET_LENGTH); ++j)
    {
        for (i = 0; i < GLOBAL_THRESHOLD; ++i)
        {
            memcpy(&node, GLOBAL_SHARES + (i * GLOBAL_SHARES_LENGTH) + ((j * 3) + 6), 3);
            node[3] = 0; /* Add null byte */
            values[i] = StringCharToInt(node, 3);
        }

        /* Decode secret character */
        secretInt = LagrInter(params, values, GLOBAL_THRESHOLD, PRIME_NUMBER);

        c = DecodeFromInt((int)secretInt);
        secret[j] = c;
    }
    /* Add propper null byte */
    secret[GLOBAL_SECRET_LENGTH] = '\0';

    fprintf(stdout, "%s\n", secret);

    /* Deallocate memory */
    free(GLOBAL_SECRET);
    return 1;
}

/**
 * EncodeToInt()
 *
 * Encodes character into unambigous integer.
 * ---
 * Just represents every ASCII character as its ascii code
 *
 * @param   char    Character to be uniquely encoded into integer.
 * @return  int     Encoded char as integer.
 */
int EncodeToInt(char charToEncode)
{
    return (int)charToEncode;
}

/**
 * FinitieFieldPow()
 *
 * Does power number to exponent in finite field.
 * ---
 * To allow compute shares with larger amount of shares needed without overflowing
 * integer (or even long long) ranges. Implementation of Fermat's little theorem.
 *
 * @param   int     Base for exponentiation.
 * @param   int     Exponent.
 * @param   int     Characteristics of finite field.
 * @return  int     Result of exponentiation.
 */
int FinitieFieldPow(int base, int exponent, int fieldChar)
{
    int x = 1, y = base;
    while (exponent > 0)
    {
        if (exponent % 2 == 1)
        {
            x = (x * y);
            if (x > fieldChar)
                x %= fieldChar;
        }
        y = (y * y);
        if (y > fieldChar)
            y %= fieldChar;
        exponent = exponent / 2;
    }
    return x;
}


/**
 * EvalPolyAt()
 *
 * Evalues polynomial at x using provided coefficients.
 * ---
 * Implemented using Horner's scheme.
 *
 *
 * @param   int     Parameter of poly.
 * @param   int     Poly degree.
 * @param   int*    Array of coefficients.
 * @return  int     Value of poly.
 */
int EvalPolyAt(int x, int degree, int* coeffs)
{
    unsigned long long int sum = coeffs[degree];
    int i;
    for(i = degree - 1; i >= 0; --i) {
        sum *= x;
        sum = (sum + coeffs[i]);

        if (sum > (unsigned long) (long) LONG_MAX)
        {
            sum = sum % PRIME_NUMBER;
        }
    }
    return sum;
}

/**
 * GenerateShares()
 *
 * Generates shares for secret stored in global variable.
 * ---
 * Shares are computed and stored in preallocated array of integers in heap memory.
 * (dev note: it is pointless step now, but for the sake of future extensibility of
 * program it makes sense, cause you'd want some better encoding for e.g. Unicode character table)
 *
 * @param   GLOBALS: GLOBAL_SECRET, GLOBAL_NUM_SHARES, GLOBAL_THRESHOLD, GLOBAL_SECRET_LENGTH
 * @param   void
 * @return  int     End state of function:
 *                      1   Success
 *                      -1  Memory allocation failed
 */
int GenerateShares()
{
    srand(time(0));

    /* Compute length of every share array
    ** xxx|ttt|c_1: f(x)|c_2: f(x)| ... |\0|
    ** xxx - argument of plynomial
    ** ttt - threshold
    ** c_1: f(x) - function value at x of particular secret letter */
    int shareLen =  3 + 3 + ((3 * GLOBAL_SECRET_LENGTH) + 1);

    int ptrSize = sizeof(char*);
    int padding = (ptrSize  - shareLen % ptrSize ) % ptrSize;

    shareLen = shareLen + padding;

    /* Allocate space for strings with shares
     * (pointer to array of strings) */
    char* pSharesArr;
    pSharesArr = malloc(GLOBAL_NUM_SHARES * shareLen);
    if (pSharesArr == 0) {
        /* Allocation failed */
        return -1;
    }

    /* Create polynomial of degree GLOBAL_THRESHOLD - 1 coeficients */
    int coefArr[GLOBAL_THRESHOLD];

    int i;
        /* Generate coeficients for polynomial except 0th one  */
    for (i = 1; i < GLOBAL_THRESHOLD; ++i)
    {
        coefArr[i] = rand() % PRIME_NUMBER;
    }

    /* Write x argument to all shares */
    for (i = 0; i < GLOBAL_NUM_SHARES; ++i) {
        sprintf(pSharesArr + (i * shareLen), "%03d", i + 1);

    }

    /* Write threshold number to all shares */
    for (i = 0; i < GLOBAL_NUM_SHARES; ++i) {
        sprintf(pSharesArr + 3 + (i * shareLen), "%03d", GLOBAL_THRESHOLD);
    }

    /* Loop through all secret letters */
    for (i = 0; i < GLOBAL_SECRET_LENGTH; ++i)
    {

        /* Set 0th coeficient */
        coefArr[0] = EncodeToInt(GLOBAL_SECRET[i]);

        int j;
        /* Compute share value for this letter for every share piece and write it as string */
        for (j = 0; j < GLOBAL_NUM_SHARES; ++j) {
            sprintf(pSharesArr +((i + 2) * 3) + (j * shareLen), "%03d", EvalPolyAt(j + 1, GLOBAL_THRESHOLD - 1, coefArr) % PRIME_NUMBER);
        }
        /* INFO: sprintf puts null byte at the end, so we don't have to put it there */
    }

    /* Output result */
    for (i = 0; i < GLOBAL_NUM_SHARES; ++i)
    {
        fprintf(stdout, "%s\n", pSharesArr + (i * shareLen));
    }

    /* Deallocate shares memory location */
    free(pSharesArr);
    return 1;
}


/**
 * LoadDataForGenerating()
 *
 * Loads data for generating shares from user, because thew were not
 * sent in as args. Then it Checks data and starts generating.
 *
 * @param   void
 * @return  void
 */
void LoadDataForGenerating()
{
    char* sBuffer = malloc(1024 * sizeof(char));
    ClearConsole();

    printf("Enter secret: \n");
    fgets(sBuffer, 1024, stdin);

    GLOBAL_SECRET = sBuffer;
    GLOBAL_SECRET_LENGTH = strlen(GLOBAL_SECRET);

    printf("Enter number of shares: \n");
    scanf("%20d", &GLOBAL_NUM_SHARES);

    printf("Enter threshold: \n");
    scanf("%20d", &GLOBAL_THRESHOLD);

    CheckDataAndGenerate(0);

    free(sBuffer);
    return;
}

/**
 * PresKeyToContinue()
 *
 * Does reliable pause of the application for user to read output.
 *
 * @param   void
 * @return  void
 */
void PresKeyToContinue()
{
    system ("pause");
    return;
}

/**
 * TerminateApplication()
 *
 * Safely terminates application.
 *
 * @param   void
 * @return  void
 */
void TerminateApplication()
{
    system ("pause");

    exit(0);
    return;
}

/**
 * ErrFileInput()
 *
 * Handles error when input was from file.
 * ---
 * Redirects stdout back to console and prints message.
 *
 * @param   char*   Message to be printed out.
 * @return  void
 */
void ErrFileInput(char* message)
{
    freopen("CON","w",stdout);
    fprintf(stdout, message);
    TerminateApplication();
}

/**
 * ErrCLI()
 *
 * Handles error when input was from console.
 *
 * @param   char*   Message to be printed out.
 * @return  void
 */
void ErrCLI(char* message)
{
    fprintf(stdout, message);
    PresKeyToContinue();
    StartCLI();
    return;
}

/**
 * CheckDataAndGenerate()
 *
 * Checks validity of input and starts generating.
 *
 * @param   GLOBALS: GLOBAL_SECRET, GLOBAL_NUM_SHARES, GLOBAL_THRESHOLD
 * @param   int     If needed forced redirection back to stdout.
 * @return  void
 */
void CheckDataAndGenerate(int shouldRedirect)
{

    if (GLOBAL_SECRET_LENGTH > 1023) {
        if (shouldRedirect) {
            ErrFileInput("Secret can be 1024 characters maximum!\n");
        }
        else
        {
            ErrCLI("Secret can be 1024 characters maximum!\n");
        }
    }

    if (GLOBAL_NUM_SHARES == 0 || GLOBAL_THRESHOLD == 0)
    {
        if (shouldRedirect) {
            ErrFileInput("Wrong input!\n");
        }
        else
        {
            ErrCLI("Wrong input!\n");
        }
    }

    if (GLOBAL_NUM_SHARES > 999)
    {
        if (shouldRedirect) {
            ErrFileInput("Number of shares can maximally 999!\n");
        }
        else
        {
            ErrCLI("Number of shares can maximally 999!\n");
        }
    }

    if (GLOBAL_THRESHOLD > 256)
    {
        if (shouldRedirect) {
            ErrFileInput("Threshold must be less than 257!\n");
        }
        else
        {
            ErrCLI("Threshold must be less than 257!\n");
        }
    }

    if (GLOBAL_NUM_SHARES < GLOBAL_THRESHOLD)
    {
        if (shouldRedirect) {
            ErrFileInput("Number of shares must be at least as threshold.\n");
        }
        else
        {
            ErrCLI("Number of shares must be at least as threshold.\n");
        }
    }

    /* Input is correct, generate shares */
    if (GenerateShares() == 1)
    {
        if (shouldRedirect) {
            ErrFileInput("Shares successfully generated!\n");
        }
        else
        {
            ErrCLI("Shares successfully generated!\n");
        }
    }
    else
    {
        if (shouldRedirect) {
            ErrFileInput("Generating shares failed!\n");
        }
        else
        {
            ErrCLI("Generating shares failed!\n");
        }
    }
    return;
}

int main(int argc, char* argv[])
{
    /* If number of args sent is 4,
     * user probably wants to create shares from secret */
    if (argc == 4)
    {
        /* Load data into global variables */
        GLOBAL_SECRET = argv[1];
        GLOBAL_SECRET_LENGTH = strlen(GLOBAL_SECRET);

        GLOBAL_NUM_SHARES = strtol(argv[2], &end, 10);
        GLOBAL_THRESHOLD = strtol(argv[3], &end, 10);

        CheckDataAndGenerate(1);
    }
    /* User wants to reconstruct secret or start program CLI */
    else
    {
        /* Detect if stdin was redirected or
         * application opened without input redirection,
         * if no input file start CLI version */
        if (isatty(STDIN_FILENO)) {
            StartCLI();
        } else {
            if (LoadShares())
            {
                ReconstructSecret();
            }
            else
            {
                /* Force redirect stdout to console */
                freopen("CON","w",stdout);
                fprintf(stdout, "Loading shares failed!\n");
            }

        }
    }
    TerminateApplication();
    return 0;
}
