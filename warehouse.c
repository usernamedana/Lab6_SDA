#include "warehouse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void clearBuf(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

static void readLine(char *buf, size_t size) {
    if (fgets(buf, (int)size, stdin) != NULL) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        else clearBuf();
    }
}

static int readInt(const char *prompt) {
    int v;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &v) == 1) { clearBuf(); return v; }
        printf("Invalid integer. Try again.\n");
        clearBuf();
    }
}

static unsigned readUnsigned(const char *prompt) {
    unsigned v;
    while (1) {
        printf("%s", prompt);
        if (scanf("%u", &v) == 1) { clearBuf(); return v; }
        printf("Invalid number. Try again.\n");
        clearBuf();
    }
}

static double readDouble(const char *prompt) {
    double v;
    while (1) {
        printf("%s", prompt);
        if (scanf("%lf", &v) == 1) { clearBuf(); return v; }
        printf("Invalid real number. Try again.\n");
        clearBuf();
    }
}

static int isLeapYear(int y) {
    return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
}

static int isValidDate(int d, int m, int y) {
    int dim[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (y < 1900 || y > 2100) return 0;
    if (m < 1 || m > 12) return 0;
    if (isLeapYear(y)) dim[1] = 29;
    return d >= 1 && d <= dim[m - 1];
}

static int isFutureDate(int d, int m, int y) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int cy = t->tm_year + 1900, cm = t->tm_mon + 1, cd = t->tm_mday;
    if (y > cy) return 1;
    if (y == cy && m > cm) return 1;
    if (y == cy && m == cm && d > cd) return 1;
    return 0;
}

static void readValidDate(DateA *date) {
    int d, m, y, ok;
    do {
        d = readInt("Contract day  : ");
        m = readInt("Contract month: ");
        y = readInt("Contract year : ");
        ok = 1;
        if (!isValidDate(d, m, y)) {
            printf("Invalid calendar date.\n"); ok = 0;
        } else if (isFutureDate(d, m, y)) {
            printf("Contract date cannot be in the future.\n"); ok = 0;
        } else if (isWeekendDate(d, m, y)) {
            printf("Contracts cannot be signed on Saturdays or Sundays.\n"); ok = 0;
        } else if (isHolidayDate(d, m)) {
            printf("Contracts cannot be signed on public holidays.\n"); ok = 0;
        }
    } while (!ok);
    date->day = d; date->month = m; date->year = y;
}

static void readCurrency(char *buf, size_t size) {
    while (1) {
        printf("Currency (MDL/EUR/USD/RON): ");
        readLine(buf, size);
        if (strcmp(buf,"MDL")==0 || strcmp(buf,"EUR")==0 ||
            strcmp(buf,"USD")==0 || strcmp(buf,"RON")==0) return;
        printf("Invalid currency. Allowed: MDL, EUR, USD, RON.\n");
    }
}

void inputWarehouseRecord(WarehouseRecord *r) {
    printf("Owner name      : "); readLine(r->ownerName,    NAME_LEN);
    printf("Owner surname   : "); readLine(r->ownerSurname, NAME_LEN);
    printf("Product name    : "); readLine(r->productName,  PRODUCT_LEN);
    printf("Manufacturer    : "); readLine(r->manufacturer, MANUFACTURER_LEN);
    readValidDate(&r->contractDate);
    readCurrency(r->currency, CURRENCY_LEN);

    r->wholesalePrice = readDouble("Wholesale price : ");
    while (r->wholesalePrice < 0) {
        printf("Price cannot be negative.\n");
        r->wholesalePrice = readDouble("Wholesale price : ");
    }
    r->unitPrice = readDouble("Unit price      : ");
    while (r->unitPrice < 0 || r->unitPrice < r->wholesalePrice) {
        if (r->unitPrice < 0)
            printf("Price cannot be negative.\n");
        else
            printf("Unit price must be >= wholesale price (no loss-making contracts).\n");
        r->unitPrice = readDouble("Unit price      : ");
    }
    r->quantity = readUnsigned("Quantity        : ");
}

void displayWarehouseRecord(const WarehouseRecord *r) {
    printf("  %s %s | %s (%s) | %02d.%02d.%04d | %.2f/%.2f %s | qty=%u\n",
           r->ownerName, r->ownerSurname,
           r->productName, r->manufacturer,
           r->contractDate.day, r->contractDate.month, r->contractDate.year,
           r->wholesalePrice, r->unitPrice, r->currency,
           r->quantity);
}
