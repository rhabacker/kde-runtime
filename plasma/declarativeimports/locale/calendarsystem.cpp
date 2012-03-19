/*
    Copyright (C) 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>
    Copyright (C) 2012 Antonis Tsiapaliokas <kok3rs@gmail.com>
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "calendarsystem.h"

#include "kdebug.h"
#include "kconfiggroup.h"
#include <KCalendarSystem>
#include <KDateTime>
#include <KGlobal>
#include <QtCore/QDateTime>

CalendarSystem::CalendarSystem(QObject* parent)
        : QObject(parent)
{
    m_calendarSystem = KCalendarSystem::create(KGlobal::locale()->calendarSystem());
}

QList<Locale::CalendarSystem> CalendarSystem::calendarSystemsList()
{
     QList<Locale::CalendarSystem> list;

     foreach(KLocale::CalendarSystem calendarSystem, KCalendarSystem::calendarSystemsList()) {
        list.append((Locale::CalendarSystem)calendarSystem);
    }

    return list;
}

QString CalendarSystem::calendarLabel(Locale::CalendarSystem calendarSystem, const KLocale *locale)
{
    return KCalendarSystem::calendarLabel((KLocale::CalendarSystem)calendarSystem, locale);
}

QString CalendarSystem::calendarType(Locale::CalendarSystem calendarSystem)
{
    return KCalendarSystem::calendarType((KLocale::CalendarSystem)calendarSystem);
}


Locale::CalendarSystem CalendarSystem::calendarSystem(const QString &calendarType )
{
    return (Locale::CalendarSystem)KCalendarSystem::calendarSystem(calendarType);
}

Locale::CalendarSystem CalendarSystem::calendarSystem() const
{
    return (Locale::CalendarSystem)m_calendarSystem->calendarSystem();
}


QString CalendarSystem::calendarLabel() const
{
    return m_calendarSystem->calendarLabel();
}

QDate CalendarSystem::epoch() const
{
    return m_calendarSystem->epoch();
}

QDate CalendarSystem::earliestValidDate() const
{
    return epoch();
}

QDate CalendarSystem::latestValidDate() const
{
    // Default to Gregorian 9999-12-31
    return m_calendarSystem->latestValidDate();
}

bool CalendarSystem::isValid(int year, int month, int day) const
{
   return m_calendarSystem->isValid(year, month, day);
}


bool CalendarSystem::isValid(int year, int dayOfYear) const
{
    return m_calendarSystem->isValid(year, dayOfYear);
}


bool CalendarSystem::isValid(const QString &eraName, int yearInEra, int month, int day) const
{
    return m_calendarSystem->isValid(eraName, yearInEra, month, day);
}

bool CalendarSystem::isValidIsoWeekDate(int year, int isoWeekNumber, int dayOfIsoWeek) const
{
    return m_calendarSystem->isValidIsoWeekDate(year, isoWeekNumber, dayOfIsoWeek);
}

bool CalendarSystem::isValid(const QDate &date) const
{
    return m_calendarSystem->isValid(date);
}

QVariantHash CalendarSystem::getDate(const QDate date) const
{
    QVariantHash hash;

    int year;
    int month;
    int day;

    m_calendarSystem->getDate(date, &year, &month, &day);

    hash["year"] = year;
    hash["month"] = month;
    hash["day"] = day;

    return hash;
}

int CalendarSystem::year(const QDate &date) const
{
    return m_calendarSystem->year(date);
}

int CalendarSystem::month(const QDate &date) const
{
    return m_calendarSystem->month(date);
}

int CalendarSystem::day(const QDate &date) const
{
    return m_calendarSystem->day(date);
}

QString CalendarSystem::eraName(const QDate &date, StringFormat format) const
{
    return m_calendarSystem->eraName(date, (KCalendarSystem::StringFormat)format);
}

QString CalendarSystem::eraYear(const QDate &date, StringFormat format) const
{
    return m_calendarSystem->eraYear(date, (KCalendarSystem::StringFormat)format);
}


int CalendarSystem::yearInEra(const QDate &date) const
{
    return m_calendarSystem->yearInEra(date);
}

QDate CalendarSystem::addYears(const QDate &date, int numYears) const
{
    return m_calendarSystem->addYears(date, numYears);
}

QDate CalendarSystem::addMonths(const QDate &date, int numMonths) const
{
    return m_calendarSystem->addMonths(date, numMonths);
}

QDate CalendarSystem::addDays(const QDate &date, int numDays) const
{
    return m_calendarSystem->addDays(date, numDays);
}

QVariantHash CalendarSystem::dateDifference(const QDate &fromDate, const QDate &toDate) const
{
    QVariantHash hash;

    int yearsDiff;
    int monthsDiff;
    int daysDiff;
    int direction;

    m_calendarSystem->dateDifference(fromDate, toDate, &yearsDiff, &monthsDiff, &daysDiff, &direction);

    hash["years"] = yearsDiff;
    hash["months"] = monthsDiff;
    hash["days"] = daysDiff;
    hash["direction"] = direction;

    return hash;
}

int CalendarSystem::yearsDifference(const QDate &fromDate, const QDate &toDate) const
{
    return m_calendarSystem->yearsDifference(fromDate, toDate);
}

int CalendarSystem::monthsDifference(const QDate &fromDate, const QDate &toDate) const
{
    return m_calendarSystem->monthsDifference(fromDate, toDate);
}

int CalendarSystem::daysDifference(const QDate &fromDate, const QDate &toDate) const
{
    return m_calendarSystem->daysDifference(fromDate, toDate);
}

int CalendarSystem::monthsInYear(const QDate &date) const
{
    return m_calendarSystem->monthsInYear(date);
}

int CalendarSystem::monthsInYear(int year) const
{
    return m_calendarSystem->monthsInYear(year);
}

int CalendarSystem::weeksInYear(const QDate &date) const
{
    return weeksInYear(date, Locale::DefaultWeekNumber);
}

int CalendarSystem::weeksInYear(int year) const
{
    return weeksInYear(year, Locale::DefaultWeekNumber);
}

int CalendarSystem::weeksInYear(const QDate &date, Locale::WeekNumberSystem weekNumberSystem) const
{
    return m_calendarSystem->weeksInYear(date, (KLocale::WeekNumberSystem)weekNumberSystem);
}

int CalendarSystem::weeksInYear(int year, Locale::WeekNumberSystem weekNumberSystem) const
{
     return m_calendarSystem->weeksInYear(year, (KLocale::WeekNumberSystem)weekNumberSystem);
}

int CalendarSystem::daysInYear(const QDate &date) const
{
    return m_calendarSystem->daysInYear(date);
}

int CalendarSystem::daysInYear(int year) const
{
    return m_calendarSystem->daysInYear(year);
}

int CalendarSystem::daysInMonth(const QDate &date) const
{
    return m_calendarSystem->daysInMonth(date);
}

int CalendarSystem::daysInMonth(int year, int month) const
{
   return m_calendarSystem->daysInMonth(year, month);
}

int CalendarSystem::daysInWeek(const QDate &date) const
{
    return m_calendarSystem->daysInWeek(date);
}

int CalendarSystem::dayOfYear(const QDate &date) const
{
    return m_calendarSystem->dayOfYear(date);
}

int CalendarSystem::dayOfWeek(const QDate &date) const
{
    return m_calendarSystem->dayOfWeek(date);
}

int CalendarSystem::week(const QDate &date, int *yearNum) const
{
    return week(date, Locale::DefaultWeekNumber, yearNum);
}

int CalendarSystem::week(const QDate &date, Locale::WeekNumberSystem weekNumberSystem, int *yearNum) const
{
    return m_calendarSystem->week(date, (KLocale::WeekNumberSystem)weekNumberSystem, yearNum);
}

bool CalendarSystem::isLeapYear(int year) const
{
    return m_calendarSystem->isLeapYear(year);
}

bool CalendarSystem::isLeapYear(const QDate &date) const
{
    return m_calendarSystem->isLeapYear(date);
}

QDate CalendarSystem::firstDayOfYear(int year) const
{
    return m_calendarSystem->firstDayOfYear(year);
}

QDate CalendarSystem::lastDayOfYear(int year) const
{
    return m_calendarSystem->lastDayOfYear(year);
}

QDate CalendarSystem::firstDayOfYear(const QDate &date) const
{
    return m_calendarSystem->firstDayOfYear(date);
}


QDate CalendarSystem::lastDayOfYear(const QDate &date) const
{
    return m_calendarSystem->lastDayOfYear(date);
}

QDate CalendarSystem::firstDayOfMonth(int year, int month) const
{
   return m_calendarSystem->firstDayOfMonth(year, month);
}

QDate CalendarSystem::lastDayOfMonth(int year, int month) const
{
    return m_calendarSystem->lastDayOfMonth(year, month);
}

QDate CalendarSystem::firstDayOfMonth(const QDate &date) const
{
   return m_calendarSystem->firstDayOfMonth(date);
}

QDate CalendarSystem::lastDayOfMonth(const QDate &date) const
{
    return m_calendarSystem->lastDayOfMonth(date);
}

QString CalendarSystem::monthName(int month, int year, CalendarSystem::MonthNameFormat format) const
{
    return m_calendarSystem->monthName(month, year, (KCalendarSystem::MonthNameFormat)format);
}

QString CalendarSystem::monthName(const QDate &date, MonthNameFormat format) const
{
    return m_calendarSystem->monthName(date, (KCalendarSystem::MonthNameFormat)format);
}

QString CalendarSystem::weekDayName(int weekDay, CalendarSystem::WeekDayNameFormat format) const
{
    return m_calendarSystem->weekDayName(weekDay, (KCalendarSystem::WeekDayNameFormat)format);
}

QString CalendarSystem::weekDayName(const QDate &date, WeekDayNameFormat format) const
{
   return m_calendarSystem->weekDayName(date, (KCalendarSystem::WeekDayNameFormat)format);
}

QString CalendarSystem::formatDate(const QDate &fromDate, Locale::DateFormat toFormat) const
{
    return m_calendarSystem->formatDate(fromDate, (KLocale::DateFormat)toFormat);
}

QString CalendarSystem::formatDate(const QDate &fromDate, const QString &toFormat,
                                    Locale::DateTimeFormatStandard standard) const
{
    return m_calendarSystem->formatDate(fromDate, toFormat, (KLocale::DateTimeFormatStandard)standard);
}


QString CalendarSystem::formatDate(const QDate &fromDate, const QString &toFormat, Locale::DigitSet digitSet,
                                    Locale::DateTimeFormatStandard formatStandard) const
{
    return m_calendarSystem->formatDate(fromDate, toFormat, (KLocale::DigitSet)digitSet,
                                        (KLocale::DateTimeFormatStandard)formatStandard);
}

QString CalendarSystem::formatDate(const QDate &date, Locale::DateTimeComponent component,
                                    Locale::DateTimeComponentFormat format,
                                    Locale::WeekNumberSystem weekNumberSystem) const
{
    return m_calendarSystem->formatDate(date, (KLocale::DateTimeComponent)component,
                                        (KLocale::DateTimeComponentFormat)format,
                                        (KLocale::WeekNumberSystem)weekNumberSystem);
}

QDate CalendarSystem::readDate(const QString &str, bool *ok) const
{
    return m_calendarSystem->readDate(str, ok);
}

QDate CalendarSystem::readDate(const QString &str, Locale::ReadDateFlags flags, bool *ok) const
{
    return m_calendarSystem->readDate(str, (KLocale::ReadDateFlags)flags, ok);
}

QDate CalendarSystem::readDate(const QString &inputString, const QString &formatString, bool *ok) const
{
    return readDate(inputString, formatString, ok, Locale::KdeFormat);
}

QDate CalendarSystem::readDate(const QString &inputString, const QString &formatString, bool *ok,
                                Locale::DateTimeFormatStandard formatStandard) const
{
    return m_calendarSystem->readDate(inputString, formatString, ok,
                                      (KLocale::DateTimeFormatStandard)formatStandard);
}

int CalendarSystem::shortYearWindowStartYear() const
{
    return m_calendarSystem->shortYearWindowStartYear();
}

int CalendarSystem::applyShortYearWindow(int inputYear) const
{
    return m_calendarSystem->applyShortYearWindow(inputYear);
}

int CalendarSystem::weekStartDay() const
{
    return m_calendarSystem->weekStartDay();
}

bool CalendarSystem::isSolar() const
{
    return m_calendarSystem->isSolar();
}

bool CalendarSystem::isLunar() const
{
    return m_calendarSystem->isLunar();
}

bool CalendarSystem::isLunisolar() const
{
    return m_calendarSystem->isLunisolar();
}

bool CalendarSystem::isProleptic() const
{
    return m_calendarSystem->isProleptic();
}