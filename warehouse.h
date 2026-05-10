#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <stddef.h>
#include <string.h>
#include <time.h>

#define NAME_LEN         50
#define PRODUCT_LEN      80
#define MANUFACTURER_LEN 80
#define CURRENCY_LEN     10

typedef struct DateA {
    int day;
    int month;
    int year;
} DateA;

typedef struct WarehouseRecord {
    char     ownerName[NAME_LEN];
    char     ownerSurname[NAME_LEN];
    char     productName[PRODUCT_LEN];
    char     manufacturer[MANUFACTURER_LEN];
    DateA    contractDate;
    char     currency[CURRENCY_LEN];
    double   wholesalePrice;
    double   unitPrice;
    unsigned quantity;
} WarehouseRecord;

static inline int isWeekendDate(int day, int month, int year) {
    struct tm t = {0};
    t.tm_mday = day;
    t.tm_mon  = month - 1;
    t.tm_year = year - 1900;
    mktime(&t);
    return t.tm_wday == 0 || t.tm_wday == 6;
}

static inline int isHolidayDate(int day, int month) {
    if (month == 1  && (day == 1  || day == 7  || day == 8))  return 1;
    if (month == 3  &&  day == 8)                              return 1;
    if (month == 5  && (day == 1  || day == 9))                return 1;
    if (month == 8  && (day == 27 || day == 31))               return 1;
    if (month == 12 &&  day == 25)                             return 1;
    return 0;
}

void inputWarehouseRecord(WarehouseRecord *r);
void displayWarehouseRecord(const WarehouseRecord *r);

#endif
