#pragma once
#include <string>
#include <vector>
#include <utility>

class TimeSeriesTransformations
{

private:
    // initialize vector of pairs in which to store the data.
    std::vector<std::pair<int, double>> _data{}; 

    // initialize name of TST object.
    std::string _name{}; 

    // initialize separator.
    char _separator{};

    // set desired precision.
    const int decimalPlaces = 5;
 
    // below are functions to be used within the class;

    // gets the mean of a vector.
    static double getMean(const std::vector<double>& vector);
  
    // gets the standard deviation of a vector.
    static double getSD(const std::vector<double>& vector);

    // truncates a datetime Y-M-D Hr:Min:Sec to only the day Y-M-D.
    static int truncDate(std::string& date);

    // same as truncDate but in terms of unix.
    static int truncUnix(int& unix);

    // rounds a gived double to 5 decimal places.
    double roundTo(double& value_to_round);

    // function to check tricky dates.
    bool trickyDate(std::string date) const;
 
public:
    

    // default constructor.
    TimeSeriesTransformations()
    {
        _data = {};
        _name = {};
    };

    // constructor that creates a TimeSeriesTransformations object from a given external file.
    explicit TimeSeriesTransformations(const std::string& filenameandpath);

    // second constructor for data given from two vectors and a name rather than from an external file. 
    TimeSeriesTransformations(const std::vector<int>& time, const std::vector<double>& price, std::string name = "");

    // Copy Constructor.
    TimeSeriesTransformations(const TimeSeriesTransformations& t);

    // Destructor
    virtual ~TimeSeriesTransformations();

    // overloaded = operator.
    TimeSeriesTransformations& operator=(const TimeSeriesTransformations& t);

    // a function which returns the price vector.
    std::vector<double> getPrice() const;

    // a function which returns the time vector.
    std::vector<int> getTime() const;

    // equality operator.
    bool operator== (const TimeSeriesTransformations& t) const;

    // a fucntion which returns the name of the asset (header of the price vector).
    std::string getName() const;

    // a function which changes the name of the vector of prices (name of the asset).
    void setName(const std::string& newName);

    // a function that counts the number of data points of a TimeSeriesTransformations object.
    int count() const;

    // function which converts a given UNIX timestamp to human readable date -> Year-Month-Date Hour-Minute-Second.
    static std::string convertToDate(const time_t& unix);

    // this is the inverse of the above and converts a given human readable date into a UNIX timestamp.
    static int convertToUnix(const std::string& date);

    // function that displays the vector of pairs time-price (here time is displayed in human readable dates).
    void displayData() const;

    // function that displays the price vector.
    void displayPrice() const;

    // function to compute the mean of the price vector.
    bool mean(double* meanValue) const;

    // fucntion that computes the standard deviation of prices.
    bool standardDeviation(double* standardDeviationValue) const;

    // function that computed the increments of the price vector.
    std::vector<double> computeIncrements() const;

    // function that displays increments.
    void displayIncrements() const;

    // function that computes the mean of the increments.
    bool computeIncrementMean(double* meanValue) const;

    // function that computes the standard deviation of the increments.
    bool computeIncrementStandardDeviation(double* standardDeviationValue) const;

    // function to add a share price given a date and a time.
    void addASharePrice(std::string datetime, double price);

    // function that removes an entry at a given time;
    bool removeEntryAtTime(std::string time);

    // function that removes all prices that are greater than a given double.
    bool removePricesGreaterThan(double price);

    // function that removes all prices that are lower than a given double.
    bool removePricesLowerThan(double price);
    
    // function that removes all prices before a given date.
    bool removePricesBefore(std::string date);

    // function that removes all prices after a given date.
    bool removePricesAfter(std::string date);

    // function that prints the share price on a given date.
    std::string printSharePricesOnDate(std::string date) const;

    // function that print the increments on a given date.
    std::string printIncrementsOnDate(std::string date) const;

    // function that finds the greatest incerement in the series of prices on a given date.
    bool findGreatestIncrements(std::string* date, double* price_increment) const;

    // function that returns the price at a given date (in case of multiple prices on a date, it returns the price of the last time in that day).
    bool getPriceAtDate(const std::string date, double* value) const;

    // function that saves the data.
    void saveData(std::string filename) const;

    // function that saves the data but instead of unix uses human readable date.
    void saveDataHumanDate(std::string filename) const;

    // gets the separator of the file loaded.
    char getSeparator() const;

};