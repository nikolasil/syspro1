#include <iostream>
#include <string>

#include "date.h"

date::date(string dd, string mm, string yy) : day(dd), month(mm), year(yy)
{
}

date::date(string d)
{
    this->setAll(d);
}

date::~date()
{
}

void date::print()
{
    cout << this->day << "-" << this->month << "-" << this->year;
}

int date::compare(date d)
{
    string a;
    a.append(this->year);
    a.append("-");
    a.append(this->month);
    a.append("-");
    a.append(this->day);

    string b;
    b.append(d.year);
    b.append("-");
    b.append(d.month);
    b.append("-");
    b.append(d.day);

    if (a.compare(b) < 0)
    {
        // cout << a << "<" << b << endl;
        return -1;
    }
    else if (a.compare(b) > 0)
    {
        // cout << a << ">" << b << endl;
        return 1;
    }
    // cout << a << "=" << b << endl;
    return 0;
}

// GETTERS
string date::getDay()
{
    return this->day;
}

string date::getMonth()
{
    return this->month;
}

string date::getYear()
{
    return this->year;
}

// SETTERS
void date::setDay(string d)
{
    this->day = d;
}

void date::setMonth(string m)
{
    this->month = m;
}

void date::setYear(string y)
{
    this->year = y;
}

void date::setAll(string dd, string mm, string yy)
{
    this->day = dd;
    this->month = mm;
    this->year = yy;
}

void date::setAll(string d)
{
    if (d.compare("") == 0)
    {
        this->day = "";
        this->month = "";
        this->year = "";
        return;
    }
    string delimiter = "-";
    d.append(delimiter);
    size_t pos = 0;
    string token;
    int i = 0;
    string arr[3];
    while ((pos = d.find(delimiter)) != string::npos)
    {
        token = d.substr(0, pos);
        arr[i++] = token;
        d.erase(0, pos + delimiter.length());
    }
    this->day = arr[0];
    this->month = arr[1];
    this->year = arr[2];
}