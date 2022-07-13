#include"pch.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include<algorithm>
#include<numeric>
#include "../TimeSeriesTransformations/TimeSeriesTransformations.h"


bool test(double in, double in2) {
    return abs((in - in2) / in) < 0.00001;
}

// we test the functions to convert from string date to unix and viceversa.
TEST(TimeSeriesTransformations, convertToFuncs)
{
    std::string date = "2020-11-09 19:35:04";
    int unix = TimeSeriesTransformations::convertToUnix(date);
    std::string getBackDate = TimeSeriesTransformations::convertToDate(unix);
    EXPECT_TRUE(unix == 1604950504);
    EXPECT_TRUE(date == getBackDate);
}

// we test that a file loads without exceptions.
TEST(TimeSeriesTransformations, loadFileExaminationFile)
{
  EXPECT_NO_THROW(TimeSeriesTransformations("headerdata.csv"));
}

// we load in a file with header and data.
TimeSeriesTransformations t("headerdata.csv");

// we test that the function mean returns true along with the correct output.
TEST(TimeSeriesTransformations, meanOfExaminationFile)
{
    double mean; 
    auto c = t.mean(&mean);
    EXPECT_TRUE(c);
    EXPECT_DOUBLE_EQ(mean, 52.851881428571438);
}

// we test that the functio standardDeviation returns true along with the correct output.
TEST(TimeSeriesTransformations, standardDeviationOfExaminationFile)
{
    double sd;
    auto c = t.standardDeviation(&sd);
    EXPECT_TRUE(c);
    EXPECT_DOUBLE_EQ(sd, 32.997831467128677);
}

// we test that the function incrementMean returns true along with the correct output.
TEST(TimeSeriesTransformations, incrementMeanOfExaminationFile)
{
    double inc_mean;
    auto c = t.computeIncrementMean(&inc_mean);
    EXPECT_TRUE(c);
    EXPECT_DOUBLE_EQ(inc_mean, -1.8160218518518523);
}

// we test that the functio incrementStandardDeviation returns true along with the correct output.
TEST(TimeSeriesTransformations, incrementStandardDeviationOfExaminationFile)
{
    double inc_sd;
    auto c = t.computeIncrementStandardDeviation(&inc_sd);
    EXPECT_TRUE(c);
    EXPECT_DOUBLE_EQ(inc_sd, 40.233938872573837);
}

// we load a file with only the header.
TimeSeriesTransformations t1("headeronly.csv");

// we test that with header only file the mean function returns true along wth mean = nan.
TEST(TimeSeriesTransformations, meanOfEmptyFileWithHeader)
{
    double mean;
    auto c = t1.mean(&mean);
    EXPECT_FALSE(c);
    EXPECT_TRUE(isnan(mean));
}

// we test that with header only file the standardDeviation function returns true along wth SD = nan.
TEST(TimeSeriesTransformations, standardDeviationOfEmptyFileWithHeader)
{
    double sd;
    auto c = t1.standardDeviation(&sd);
    EXPECT_FALSE(c);
    EXPECT_TRUE(isnan(sd));
}

// same for incrementMean.
TEST(TimeSeriesTransformations, incrementMeanOfEmptyFileWithHeader)
{
    double inc_mean;
    auto c = t1.computeIncrementMean(&inc_mean);
    EXPECT_FALSE(c);
    EXPECT_TRUE(isnan(inc_mean));
}

// same for incrementStandardDeviation.
TEST(TimeSeriesTransformations, incrementStandardDeviationOfEmptyFileWithHeader)
{
    double inc_sd;
    auto c = t1.computeIncrementStandardDeviation(&inc_sd);
    EXPECT_FALSE(c);
    EXPECT_TRUE(isnan(inc_sd));
}

// we test that the getName function returns the first row-second column string from the file with only the header.
TEST(TimeSeriesTransformations, testNameEmptyFileWithHeader)
{
    std::string header_price = t1.getName();
    EXPECT_EQ(header_price, "HEADER");
}

// we test that getting the name of an empty file returns an error.
TEST(TimeSeriesTransformations, testNameOfExaminationFile)
{
    std::string name;
    try
    {
        TimeSeriesTransformations t2("empty.csv");
        name = t2.getName();
    }
    catch(const std::runtime_error& error)
    {
        name = "cannot open file"; // if there is an error, then the file is empty and the string name will be overwritten with this message.
    }
    EXPECT_TRUE(name == "cannot open file"); // expect the file to be empty and for the string name to be overwritten.
}

// we test that the constructor succesfully returns an error if we try to open an empty file.
TEST(TimeSeriesTransformations, throwRunTimeFromFileDoesNotExist)
{
    EXPECT_THROW(TimeSeriesTransformations t2("empty.csv"), std::runtime_error);
}

// we test that building a TST object will have the appropriate configuration.
TEST(TimeSeriesTransformations, buildingTSTobject)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    EXPECT_TRUE(t.getName() == "TEST");
    EXPECT_TRUE(t.getSeparator() == ',');
    EXPECT_TRUE(t.count() == 6);
}

// we test that trying to build a TST object with time and price of unequal length will throw an error.
TEST(TimeSeriesTransformations, TSTobjectUnequalLengthInuts)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };
    EXPECT_THROW(TimeSeriesTransformations t(_time, _price, "TEST"), std::runtime_error);
}

// we test that getting the price at a date that exists will return the appropriate value.
TEST(TimeSeriesTransformations, getAPriceAtADateThatExists)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    double newPrice;
    auto c = t.getPriceAtDate("1970-01-01 00:00:06", &newPrice);
    EXPECT_TRUE(c);
    EXPECT_TRUE(newPrice == 6);
}

// we test that getting the price at a date that does not exist within the time vector, will return the newPrice as nan.
TEST(TimeSeriesTransformations, getAPriceAtADateThatDoesNotExist)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    double newPrice;
    auto c = t.getPriceAtDate("1970-01-01 00:00:07", &newPrice);
    EXPECT_FALSE(c);
    EXPECT_TRUE(isnan(newPrice));
}

// we check that the empty constructor has the appropriate configuration.
TEST(TimeSeriesTransformations, checkEmptyConstructor)
{
    TimeSeriesTransformations t({}, {}, "");
    EXPECT_TRUE(empty(t.getName()));
    EXPECT_TRUE(t.getSeparator() == ',');
    EXPECT_TRUE(t.count() == 0);
}

// we test that the = and == operators work as expected.
TEST(TimeSeriesTransformations, checkAssignmentAndEqualityOperator)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };

    // build dense TST object.
    TimeSeriesTransformations t1(_time, _price, "TEST");

    // build empty TST object.
    TimeSeriesTransformations t({}, {}, "");

    // set the equal using =.
    t = t1;

    // expect that they are equal using ==.
    EXPECT_TRUE(t == t1);
}

// checks that the save function appropriately saves a TST object.
TEST(TimeSeriesTransformations, checkSaveFunctionality)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1.12345 , 2.12345 , 3.12345 , 4.12345 , 5.12345 , 6.12345 };

    // create dense TST object.
    TimeSeriesTransformations t(_time, _price, "TEST");

    // save it.
    t.saveData("SavedData");

    // load it back.
    TimeSeriesTransformations t1("SavedData.csv");

    // expect them to be equal.
    EXPECT_TRUE(t == t1);
}

// we test that findGreatestIncrement returns true and the correct increment.
TEST(TimeSeriesTransformations, findGreatestIncrementsWithData)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 7 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    std::string _date;
    double maxInc;
    auto c = t.findGreatestIncrements(&_date, &maxInc);
    EXPECT_TRUE(c);
    EXPECT_TRUE(_date == "1970-01-01 00:00:05");
    EXPECT_TRUE(maxInc == 2);
}

// we perform the same test as above but on a TST with only one observation, which means the increments will have size zero.
// we expect the function to return false and the increment to be nan.
TEST(TimeSeriesTransformations, findGreatestIncrementsWithNoData)
{
    std::vector<int> _time = { 1 };
    std::vector<double> _price = { 1 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    std::string _date;
    double maxInc;
    auto c = t.findGreatestIncrements(&_date, &maxInc);
    EXPECT_FALSE(c);
    EXPECT_TRUE(isnan(maxInc));
}

// test the count function on a dense file.
TEST(TimeSeriesTransformations, countDataRowsLoadedInExaminationFile)
{
    int count = t.count();
    EXPECT_TRUE(count == 28);
}

// test the count function on an empty file.
TEST(TimeSeriesTransformations, countEmptyFile)
{
    int count = t1.count();
    EXPECT_TRUE(count == 0);
}

// we test that getting a price at a missing date will return false and the nePrice will be nan.
// if false, we test addShare price and we add the newPrice at the new given date.
// we get the price again. We now expect the function to return true and for the newPrice to have the correct value.
TEST(TimeSeriesTransformations, testAddASharePrice)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    double newPrice;
    std::string date = "1970-01-01 00:00:07";
    auto c = t.getPriceAtDate(date, &newPrice);
    EXPECT_FALSE(c);
    EXPECT_TRUE(isnan(newPrice));
    
    if (!c)
    {
        double newPriceAtNewDate = 7;
        t.addASharePrice(date, newPriceAtNewDate);
    }
    
    auto c1 = t.getPriceAtDate(date, &newPrice);
    EXPECT_TRUE(c1);
    EXPECT_TRUE(newPrice == 7);
}

// we test that removing an entry at a given date that exists within the data, removes it succesfully.
// This will be reflected in the size of the data which will decrease by one.
// We then try to remove the value at that same time. This should return false and the size of the data should remain unchanged.
TEST(TimeSeriesTransformations, removeEntryThatExistsAtTime)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    std::string date = "1970-01-01 00:00:06";
    auto c = t.removeEntryAtTime(date);
    EXPECT_TRUE(c);
    EXPECT_TRUE(t.count() == 5);
    auto c1 = t.removeEntryAtTime(date);
    EXPECT_FALSE(c1);
    EXPECT_TRUE(t.count() == 5);
}

// we test that removing an entry at a given date that is missing from within the file, should return false and the size of the data should remain unchanged.
TEST(TimeSeriesTransformations, removeEntryThatHasNeverExisted)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    std::string date = "1970-01-01 00:00:07";
    auto c = t.removeEntryAtTime(date);
    EXPECT_FALSE(c);
    EXPECT_TRUE(t.count() == 6);
}

// we test that removing the prices greater than a given value should return true.
// consequently, the size of the data should be less than it was before calling the function.
TEST(TimeSeriesTransformations, removePricesGreaterThan)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    int countBefore = t.count();
    double p = 5;
    auto c = t.removePricesGreaterThan(p);
    EXPECT_TRUE(c);
    EXPECT_TRUE(countBefore > t.count());
}

// same as above but for removePricesLowerThan.
TEST(TimeSeriesTransformations, removePricesLessThan)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    int countBefore = t.count();
    double p = 5;
    auto c = t.removePricesLowerThan(p);
    EXPECT_TRUE(c);
    EXPECT_TRUE(countBefore > t.count());
}

// same as bove but with the input given by a date.
TEST(TimeSeriesTransformations, removePricesBefore)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    int countBefore = t.count();
    std::string date = "1970-01-01 00:00:05";
    auto c = t.removePricesBefore(date);
    EXPECT_TRUE(c);
    EXPECT_TRUE(countBefore > t.count());
}

// same as above but for removePriceAfter.
TEST(TimeSeriesTransformations, removePricesAfter)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    int countBefore = t.count();
    std::string date = "1970-01-01 00:00:05";
    auto c = t.removePricesAfter(date);
    EXPECT_TRUE(c);
    EXPECT_TRUE(countBefore > t.count());
}

// we test that removing prices before a date returns true and decreases the count, which we store as countAfter.
// we then remove prices after the date, thius should return true and the new count should be even lower than countAfter.
TEST(TimeSeriesTransformations, removePricesAfterAndBefore)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 , 1607652062 , 1607652063 , 1607652064 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    int countBefore = t.count();
    std::string date = "2000-01-01";
    auto c = t.removePricesBefore(date);
    int countAfter = t.count();
    EXPECT_TRUE(c);
    EXPECT_TRUE(countBefore > countAfter);

    auto c1 = t.removePricesAfter(date);
    EXPECT_TRUE(c1);
    EXPECT_TRUE(countAfter > t.count());
}

// This is used to check the print methods output the correct number of 
int countNewLines(std::string s) {
    int count = 0;
    for (const auto& i : s) {
        if (i == '\n') {
            count++;
        }
    }
    return count;
}

// we test the printPricesOnDate and check that the number of lines printed is equal to the number of observations.
TEST(TimeSeriesTransformations, printPricesOnDay)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 , 1607652062 , 1607652063 , 1607652064 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    std::string date = "1970-01-01 12:25";
    std::string prices = t.printSharePricesOnDate(date);
    EXPECT_TRUE(countNewLines(prices) == 6);
}

// we do the same but for a date on the boundary.
TEST(TimeSeriesTransformations, printPricesOnDayExactDayBoundary)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 , 1607652062 , 1607652063 , 1607652064 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    std::string date = "1970-01-01 00:00";
    std::string prices = t.printSharePricesOnDate(date);
    EXPECT_TRUE(countNewLines(prices) == 6);
}

// we do the same but for a date that we know does not exist in the data. We expect the number of lines od the printed string to be zero.
TEST(TimeSeriesTransformations, printPricesOnDayThatDoesntExist)
{

    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 , 1607652062 , 1607652063 , 1607652064 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    std::string date = "1995-03-09 12:25";
    std::string prices = t.printSharePricesOnDate(date);
    EXPECT_TRUE(countNewLines(prices) == 0);
}

// same as above but with printIncrementPricesOnDay.
TEST(TimeSeriesTransformations, printIncrementPricesOnDay)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 , 1607652062 , 1607652063 , 1607652064 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    std::string date = "1970-01-01 12:25";
    std::string prices = t.printIncrementsOnDate(date);
    EXPECT_TRUE(countNewLines(prices) == 6);
}

//same as above but for a date at the boundary.
TEST(TimeSeriesTransformations, printIncrementPricesOnDayExactDayBoundary)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 , 1607652062 , 1607652063 , 1607652064 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    std::string date = "1970-01-01 00:00";
    std::string prices = t.printIncrementsOnDate(date);
    EXPECT_TRUE(countNewLines(prices) == 6);
}

// same as above but for a date we know does not exist in the data.
TEST(TimeSeriesTransformations, printPricesIncrementPricesOnDayThatDoesntExist)
{

    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 , 1607652062 , 1607652063 , 1607652064 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    std::string date = "1995-03-09 12:25";
    std::string prices = t.printIncrementsOnDate(date);
    EXPECT_TRUE(countNewLines(prices) == 0);
}

// we test that trying printing prices on a tricky date should return an error.
TEST(TimeSeriesTransformations, printPricesOnTrickyDay)
{
    std::vector<int> _time = { 1 , 2 , 3 , 4 , 5 , 6 , 1607652062 , 1607652063 , 1607652064 };
    std::vector<double> _price = { 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 };
    TimeSeriesTransformations t(_time, _price, "TEST");
    std::string date = "1970-02-30 12:25";

    EXPECT_THROW(t.printSharePricesOnDate(date), std::runtime_error);
}