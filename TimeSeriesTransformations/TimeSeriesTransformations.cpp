// TimeSeriesTransformations.cpp : Defines the functions for the static library.
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include<cmath>
#include<chrono>
#include<ctype.h>
#include "TimeSeriesTransformations.h"


TimeSeriesTransformations::TimeSeriesTransformations(const std::string& filenameandpath)
{
	// we create an ifstream object in order to open, read and act upon the file of interest.
	std::ifstream user_file;

	// we open the file.
	user_file.open(filenameandpath);

	// check if the file is open. If it is not, throw a runtime error: "Could not open file".
	if (!user_file.is_open())
	{
		throw std::runtime_error("Could not open file.");
	}

	// if the file is good (has not raised any errors) then proceed.
	if (user_file.good())
	{
		// HEADER CASE
		// if the first element is a letter, we know there is a header.
		if (isalpha(user_file.peek())) 
		{
			// get the name of the asset
			std::string header;
			getline(user_file, header);
			std::stringstream ss(header);
			while (getline(ss, header, ',')) // parse the string: get the word in the header after the comma (asset name).
			{
				ss >> _name; // store the name in the _name string.
			}
			// get time and price
			std::string line;
			while (getline(user_file, line)) // we get each non-empty line of the file.
			{
				std::stringstream ss(line);

				// we create two variables to store the time and price of each line.
				int time; double price;

				ss >> time; // we select the first term in the line and store it in the variable time.
				_separator = ss.get(); // we skip the separator and store it;
				ss >> price; // we select the second term in the line and store it in the variable price.

				// we store each time-price map into the vector of pairs "data".
				_data.push_back({ time , roundTo(price) }); // we also round the price to 5 decimal places for accuracy.
			}
			
		}
		// EMPTY FILE CASE
		// if the first character is empty, we are dealing with an empty file.
		else if (user_file.peek()) 
		{
			// throw an error in this case.
			throw std::runtime_error("File is empty."); 
		}
		// NO HEADER BUT DATA CASE
		else
		{
			// no header is provided so we set the _name to the following:
			_name = "No name provided";
			std::string line;
			// we now only concern ourselves only with getting the numerical data using the same procedure as in the HEADER CASE:
			while (getline(user_file, line)) 
			{
				std::stringstream ss(line);

				int time; double price;

				ss >> time; 
				_separator = ss.get(); 
				ss >> price; 

				_data.push_back({ time , roundTo(price) });
			}
		}
	}
	// finally, we sort the data in the case that the file received is not ordered.
	std::sort(_data.begin(), _data.end()); 

	// close the file.
    user_file.close(); 
}

// this function rounds a double into 5 decimal places.
double TimeSeriesTransformations::roundTo(double& value_to_round) 
{
	// using a mathematical trick and std::round to do so: e.g. for 2 d.m. -> std::round( x * 10^2 / 10^2 ).
	return std::round(value_to_round * std::pow(10, decimalPlaces)) / std::pow(10, decimalPlaces);
}

// we now create the second constructor which, unlike the first one, does not feed on external file, but takes in 3 inputs from the user. The object TimeSeriesTransformations is hence 
// created by a given int vector of timestamps in UNIX, a given double vector of data points representing the price of the asset and finally a name which will be the header of the price column.
TimeSeriesTransformations::TimeSeriesTransformations(const std::vector<int>& time, const std::vector<double>& price, std::string name)
{
	// if the time and price vector fed do not match, throw an error.
	if (size(time) != size(price)) 
	{
		throw std::runtime_error("Time and price vectors must be of the same size.");
	}

	// set the name.
	_name = name;

	// push the data back into the vector of pairs.
	for (int i = 0; i < time.size(); i++)
	{
		_data.push_back({ time[i], price[i] });
	}

	// sort the data in case the latter is unsorted.
	std::sort(_data.begin(), _data.end());

	// set the separator.
	_separator = ',';
}

// we create the copy constructor.
TimeSeriesTransformations::TimeSeriesTransformations(const TimeSeriesTransformations& t)
{
	_data = t._data;
	_name = t._name;
}

// overload the = operator.
TimeSeriesTransformations& TimeSeriesTransformations::operator=(const TimeSeriesTransformations& t)
{
	_data = t._data;
	_name = t._name;
	return *this;
}

// function taht gets the price comlumn of the vector of pairs.
std::vector<double> TimeSeriesTransformations::getPrice() const
{
	std::vector<double> price;
	for (int i = 0; i < _data.size(); i++)
	{
		price.push_back(_data[i].second);
	}
	return price;
}

// function that gets the time column of the vector of pairs.
std::vector<int> TimeSeriesTransformations::getTime() const
{
	std::vector<int> time;
	for (int i = 0; i < _data.size(); i++)
	{
		time.push_back(_data[i].first);
	}
	return time;
}

// overload the == operator.
bool TimeSeriesTransformations::operator==(const TimeSeriesTransformations& t) const
{
	try
	{
		// if the two sets of data mismatch, throw an error.
		if (getPrice().size() != t.getPrice().size())
		{

			throw std::runtime_error("The sets of data to be compared must be of the same size.");
		}
	}
	// catch the error and print the message to the user.
	catch (const std::runtime_error& error)
	{
		std::cerr << error.what() << '\n';
	}

	for (int i = 0; i < _data.size(); i++)
	{
		// we compare both data sets time and price vectors and return true if they match and false if they do not.
		return (_data[i].first == t._data[i].first && _data[i].second == t._data[i].second ? 1 : 0);
	}
}

// function that gets the name of the data set.
std::string TimeSeriesTransformations::getName() const
{
	return _name;
}

// function taht changes the name of the data set.
void TimeSeriesTransformations::setName(const std::string& newName)
{
	_name = newName; 
}

// function that counts the observations of the data set.
int TimeSeriesTransformations::count() const
{
	return (int)_data.size(); // we use (int) as the .size() getter returns a type size_t. The conversion from size_t to int might cause a loss of accuracy.
}

// function that converts a UNIX timestamp into human readable date (HRD).
std::string TimeSeriesTransformations::convertToDate(const time_t& unix)
{
	// convert time_t type (UNIX) to time structure (tm) using gmtime which returns GMT/UTC time.
	tm* timeStruct = gmtime(&unix); 
	
	// create int year and get the year from the timeStrcut variable (adjust by adding 1900). 
	int year = 1900 + timeStruct->tm_year; 
	// convert int year to string.
	std::string sYear = std::to_string(year);

	// we now repeat the same procedure for the remaining variables (adjusting where necessary).
	int month = 1 + timeStruct->tm_mon; std::string sMonth = std::to_string(month);
	int day = timeStruct->tm_mday; std::string sDay = std::to_string(day);
	int hour = timeStruct->tm_hour; std::string sHour = std::to_string(hour);
	int min = timeStruct->tm_min; std::string sMin = std::to_string(min);
	int sec = timeStruct->tm_sec; std::string sSec = std::to_string(sec);

	// now, for the sake of aesthetics, if the month is single valued, we add a zero before it such that instead of displaying, for example, 1, we display 01.
	(month < 10 ? sMonth = "0" + sMonth : sMonth);
	// we do the same with the rest of the variables.
	(day < 10 ? sDay = "0" + sDay : sDay);
	(hour < 10 ? sHour = "0" + sHour : sHour);
	(min < 10 ? sMin = "0" + sMin : sMin);
	(sec < 10 ? sSec = "0" + sSec : sSec);

	// we use string arithmetic to return a string of the converted UNIX to HRD, using the appropriate separators.
	return sYear + "-" + sMonth + "-" + sDay + " " + sHour + ":" + sMin + ":" + sSec;
}

// function that converts a HRD into a UNIX timestamp.
int TimeSeriesTransformations::convertToUnix(const std::string& date)
{
	// we initialize the int variables in which we will store our time.
	int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0; 

	// we create a stringstream object to parse the given string (date).
	std::stringstream ss(date);

	// we parse each variable into the integrals created above.
	ss >> year;

	// we skip the separator.
	ss.get(); 

	//do the same as above.
	ss >> month;
	ss.get();
	ss >> day;
	ss.get();
	ss >> hour;
	ss.get();
	ss >> min;
	ss.get();
	ss >> sec;

	// here we are basically inverting the convertToDate function.
	time_t unix; // we create a type time_t variable which represents the unix which we will soon fill.
	tm* timeStruct; // we create a time structure pointer.
	time(&unix); // we get the time of the unix.
	timeStruct = gmtime(&unix); // convert time_t type to time structure.
	timeStruct->tm_year = year - 1900; // get the year (after adjustment) from the time structure.
	// proceed in the same way for the rest of the variables.
	timeStruct->tm_mon = month - 1; 
	timeStruct->tm_mday = day;
	timeStruct->tm_hour = hour;
	timeStruct->tm_min = min;
	timeStruct->tm_sec = sec;

	// return the conversion from time structure to time_t type but first changing it to int to avoid loss of accuracy.
	return (int) mktime(timeStruct);
}

// function that prints the data to the console.
void TimeSeriesTransformations::displayData() const
{
	// we print the header using the string "Date" and the name of the TST object.
	std::cout << "       Date       , " << _name << '\n'; 

	// print the numerical data.
	for (int i = 0; i < _data.size(); i++)
	{
		// for each data point, we convert from UNIX to date and print. We print each associated price point.
		std::cout << convertToDate(_data[i].first) << ", " << _data[i].second << '\n';
	};

	return;
}

// I also wrote a function to only display the price in case needed by the user.
void TimeSeriesTransformations::displayPrice() const
{
	for (int i = 0; i < _data.size(); i++)
	{
		std::cout << _data[i].second << '\n'; // we print each price point on a new line.
	};
	return;
}

// function that computes the mean of a double vector.
double TimeSeriesTransformations::getMean(const std::vector<double>& vector)
{
	// we compute the cumulative sum making use of the accumulate function.
	// takes in the range (first to last data point) and the initial value of the sum (0).
	double cumSum = std::accumulate(vector.begin(), vector.end(), 0.0); 
	
	// divide the sum by the size of the vector.
	return cumSum / vector.size(); 
}

// function that computes the standard deviation of a double vector.
double TimeSeriesTransformations::getSD(const std::vector<double>& vector)
{
	// create value to store the mean of vector.
	double mean = getMean(vector);

	// create a helper vector of doubles of the same size as the given vector in which to store the squared differences.
	std::vector<double> helper(vector.size()); 

	// we make use of the transform function. Over the whole range of data points (begin to end), we use a double value (point) to store each data point, and the mean computed above,
	// to return at each iteration the square of the difference between the data point and the mean. We store the squared differences into the helper vector.
	std::transform(vector.begin(), vector.end(), helper.begin(), [mean](double point) {return (point - mean) * (point - mean); });

	// we use the accumulate function to calculate the cumulative sum of the squared differences.
	double cumSum = std::accumulate(helper.begin(), helper.end(), 0.0);

	// finally we divide the cumulative sum by the size of the vector minus one (sample SD).
	return sqrt(cumSum / (vector.size() - 1));
}

// function that computes the mean of the TST object.
bool TimeSeriesTransformations::mean(double* meanValue) const
{

	try
	{
		// if the price vector of the TST has at least one element, then we compute the mean and return true.
		if (getPrice().size() != 0)
		{
			*meanValue = getMean(getPrice()); 
			return true;
		}
		else
		{
			// otherwise we throw an error.
			throw std::runtime_error("Empty vector.");
		}
	}
	catch (const std::runtime_error& error)
	{
		// we catch the error and print he error message.
		std::cerr << error.what() << '\n';

		// in case of error, the mean value is set to nan.
		*meanValue = std::numeric_limits<double>::quiet_NaN();

		return false;
	}
}

// function that computes the standard deviaation of teh TST object. The procedure is the same as in the mean function.
bool TimeSeriesTransformations::standardDeviation(double* standardDeviationValue) const
{
	try
	{
		if (getPrice().size() != 0)
		{
			*standardDeviationValue = getSD(getPrice());
			return true;
		}
		else
		{
			throw std::runtime_error("Empty vector.");
		}
	}
	catch (const std::runtime_error& error)
	{
		std::cerr << error.what() << '\n';
		*standardDeviationValue = std::numeric_limits<double>::quiet_NaN();
		return false;
	}
}

// I create a function to compute the increments of a double vector.
std::vector<double> TimeSeriesTransformations::computeIncrements() const
{
	// we create a vector to store the prices.
	std::vector<double> increments = getPrice();

	// we make use of the adjacent_difference function to calculate the differences between prices over the whole range.
	std::adjacent_difference(increments.begin(), increments.end(), increments.begin());

	// we erase the first observation for obvious reasons.
	increments.erase(increments.begin());

	return increments;
}

// function to print the incrememts to the console.
void TimeSeriesTransformations::displayIncrements() const
{
	// add the header.
	std::cout << "    Date     , " << _name << '\n';

	// print converting each UNIX to HRD.
	for (int i = 0; i < computeIncrements().size(); i++)
	{
		// as advised by James, we set the date of the increments price[i+1] - price[i] to be i.
		std::cout << convertToDate(_data[i].first) << ", " << computeIncrements()[i] << '\n';
	}
}

// function that computes the mean of the increments. Same procedure as in the mean function.
bool TimeSeriesTransformations::computeIncrementMean(double* meanValue) const
{
	try
	{
		if (_data.size() != 0)
		{
			*meanValue = getMean(computeIncrements());
			return true;
		}
		else
		{
			throw std::runtime_error("Empty vector.");
		}
	}
	catch (const std::runtime_error& error)
	{
		std::cerr << error.what() << '\n';
		*meanValue = std::numeric_limits<double>::quiet_NaN();
		return false;
	}	
}

// function that computes the standard deviation of the increments. Same procedure as in the standard deviation function.
bool TimeSeriesTransformations::computeIncrementStandardDeviation(double* standardDeviationValue) const
{
	try
	{
		if (_data.size() != 0)
		{
			*standardDeviationValue = getSD(computeIncrements());
			return true;
		}
		else
		{
			throw std::runtime_error("Empty vector.");
		}
	}
	catch (const std::runtime_error& error)
	{
		std::cerr << error.what() << '\n';
		*standardDeviationValue = std::numeric_limits<double>::quiet_NaN();
		return false;
	}
}

// function that truncates a given date to only its Y-M-D component (i.e. from datetime to day).
int TimeSeriesTransformations::truncDate(std::string& date)
{
	std::stringstream ss(date);

	// parse the string such that the date Y-M-D H-m-S is broken by the space.
	getline(ss, date, ' ');

	// we return the first part of the broken string by first converting it to UNIX.
	return convertToUnix(date);
}

// function that truncate a UNIX timestamp.
int TimeSeriesTransformations::truncUnix(int& unix)
{
	// we first convert the UNIX to HRD.
	std::string date = convertToDate(unix);

	// we then simply apply truncDate.
	return truncDate(date);
}

// function that returns true if the given date is not tricky.
bool TimeSeriesTransformations::trickyDate(std::string date) const
{
	// parse the date and keep only the day.
	std::stringstream ss(date);
	getline(ss, date, ' ');
	std::string day = date;

	// convert the day to unix. If the day is invalid (such as 30th Feb, or 31st April), then convertToUnix will return a different day (such as 2nd March or 1st May).
	int unix = convertToUnix(day);
	
	// we convert it back to day in string and parse it so that we are only left with the day part.
	std::string helper = convertToDate(unix);
	std::stringstream ss1(helper);
	getline(ss1, helper, ' ');

	// we store it as newDate.
	std::string newDay = helper;

	// if the input day is equal to the new day, then the input is a valid date. Return true.
	if (newDay == date)
	{
		return true;
	}
	// otherwise throw an error.
	else
	{
		throw std::runtime_error("Tricky date! The given date is invalid.");
		return false;
	}
}

// function that adds a given share price at a given date.
void TimeSeriesTransformations::addASharePrice(std::string datetime, double price)
{
	// check if the date is not a tricky date. If true, proceed.
	if (trickyDate(datetime))
	{
		// convert HRD to UNIX.
		int unix = convertToUnix(datetime);

		// pushback the pair in the TST data.
		_data.push_back({ unix , price });

		// sort the data so that the pair ends up at the correct place in the series.
		std::sort(_data.begin(), _data.end());
	}
}

// funciton that removes a pair date-price at a given time.
bool TimeSeriesTransformations::removeEntryAtTime(std::string time)
{
	if (trickyDate(time))
	{
		// convert HRD to UNIX.
		int unix = convertToUnix(time);

		// store the size of the data now for future comparison.
		size_t size_before = _data.size();

		// over the whole data range, check which time is equal to the given UNIX and erase it.
		_data.erase(std::remove_if(_data.begin(), _data.end(), [unix](std::pair<int, double> pair) { return pair.first == unix; }), _data.end());

		try
		{
			// if the size of the data is changed, that means that an element was erased, which means that the given time was found within the time of the data.
			if (size_before != _data.size())
			{
				return true;
			}
			else
			{
				// otherwise throw an error.
				throw std::runtime_error("The given time is missing.");
			}
		}
		catch (const std::runtime_error& error)
		{
			// catch the error and print the message to the console.
			std::cerr << error.what() << '\n';
			return false;
		}
	}
}

// function to remove all prices greater than a given double (exclusive). The procedure is the same as in remove entry at time, but with a change in the conditional statement (line 546).
bool TimeSeriesTransformations::removePricesGreaterThan(double price)
{
	size_t size_before = _data.size();

	_data.erase(std::remove_if(_data.begin(), _data.end(), [price](std::pair<int, double> pair) { return pair.second > price; }), _data.end());

	try
	{
		if (size_before != _data.size())
		{
			return true;
		}
		else
		{
			throw std::runtime_error("The given price is lower than all prices in the series.");
		}
	}
	catch (const std::runtime_error& error)
	{
		std::cerr << error.what() << '\n';
		return false;
	}
}

// same as the previous function but for prices lower than a given double.
bool TimeSeriesTransformations::removePricesLowerThan(double price)
{
	size_t size_before = _data.size();
	_data.erase(std::remove_if(_data.begin(), _data.end(), [price](std::pair<int, double> pair) { return pair.second < price; }), _data.end());

	try
	{
		if (size_before != _data.size())
		{
			return true;
		}
		else
		{
			throw std::invalid_argument("The given price is greater than all prices in the series.");
		}
	}
	catch (const std::invalid_argument& error)
	{
		std::cerr << error.what() << '\n';
		return false;
	}
}

// function to remove prices before a given string date. Same procedure as in the previous functions, but this time we are comparing the data time vector to the given date (converted to UNIX).
bool TimeSeriesTransformations::removePricesBefore(std::string date)
{
	if (trickyDate(date))
	{
		int unix = convertToUnix(date);

		size_t size_before = _data.size();
		_data.erase(std::remove_if(_data.begin(), _data.end(), [unix](std::pair<int, double> pair) { return pair.first < unix; }), _data.end());

		try
		{
			if (size_before != _data.size())
			{
				return true;
			}
			else
			{
				throw std::runtime_error("The given date is smaller than all dates in the series.");
			}
		}
		catch (const std::runtime_error& error)
		{
			std::cerr << error.what() << '\n';
			return false;
		}
	}
}

bool TimeSeriesTransformations::removePricesAfter(std::string date)
{
	if (trickyDate(date))
	{
		int unix = convertToUnix(date);
		size_t size_before = _data.size();
		_data.erase(std::remove_if(_data.begin(), _data.end(), [unix](std::pair<int, double> pair) { return pair.first > unix; }), _data.end());


		try
		{
			if (size_before != _data.size())
			{
				return true;
			}
			else
			{
				throw std::runtime_error("The given date is larger than all dates in the series.");
			}
		}
		catch (const std::runtime_error& error)
		{
			std::cerr << error.what() << '\n';
			return false;
		}
	}
}

// function that creates a string out of the prices observed in a given date.
std::string TimeSeriesTransformations::printSharePricesOnDate(std::string date) const
{
	if (trickyDate(date))
	{
		// we truncate the date so as to return all prices in a given day.
		int unix = truncDate(date);

		// create the string to store the prices.
		std::string prices = "";

		for (int i = 0; i < _data.size(); i++)
		{
			// we select the prices for the truncated times equal to the given date truncated.
			if (truncUnix(getTime()[i]) == unix)
			{
				// perform string arithemtic to concatenate the prices on each new line.
				prices += std::to_string(getPrice()[i]) + '\n';
			}
		}

		// print the prices on new lines.
		std::cout << prices << '\n';

		// we store the string.
		return prices;
	}
}

// same as PrintPricesOnDate but fro increments.
std::string TimeSeriesTransformations::printIncrementsOnDate(std::string date) const
{
	if (trickyDate(date))
	{
		int unix = truncDate(date);

		std::string incerements = "";

		// we set the index i to run while it is less then the data size minus 1, so that if the date corresponds to the last datapoint, we do not go out of range.
		// this is necessary since, in line 510, we compute the increments using the price at i + 1.
		for (int i = 0; i < _data.size() - 1; i++)
		{
			if (truncUnix(getTime()[i]) == unix)
			{
				incerements += std::to_string(getPrice()[i + 1] - getPrice()[i]) + '\n';
			}
		}
		std::cout << incerements << '\n';
		return incerements;
	}
}

// function that finds the greatest price increment in the whole series.
bool TimeSeriesTransformations::findGreatestIncrements(std::string* date, double* price_increment) const
{
	// compute and store the increments.
	std::vector<double> increments = computeIncrements();

	// find and store the greatest element over the whole increments.
	auto greatestElement = std::max_element(increments.begin(), increments.end());
	

	try
	{
		// return true only if the price vector is greater than 1 (which implies that the increments vector size will be greater than 0).
		if (getPrice().size() > 1)
		{
			// find the index of the greatest element.
			auto index = std::distance(increments.begin(), greatestElement);

			*date = convertToDate(_data[index].first);
			*price_increment = *greatestElement;
			return true;
		}
		else
		{    
			// throw an error if increments size is 0.
			throw std::runtime_error("The vector of prices must be greater than 1.");
		}
	}
	catch (const std::runtime_error& error)
	{
		std::cerr << error.what() << '\n';

		// set *price_incerement to nan in this case.
		*price_increment = std::numeric_limits<double>::quiet_NaN();
		return false;
	}
}

// function that gets a price given a date.
bool TimeSeriesTransformations::getPriceAtDate(const std::string date, double* value) const
{
	if (trickyDate(date))
	{
		std::string date1 = date;
		int unix = convertToUnix(date1);

		// find the element matching the unix.
		auto element = std::find_if(_data.begin(), _data.end(), [unix](std::pair<int, double> pair) {return pair.first == unix; });
		try
		{
			// if the iterator is equal to _data.end(), then no element matching the unix was found.
			if (element == _data.end())
			{
				//throw error.
				throw std::runtime_error("The given date is missing.");
			}
			else
			{
				// otheriwise get the index of the element and return true.
				auto index = std::distance(_data.begin(), element);
				*value = getPrice()[index];
				return true;
			}
		}
		catch (const std::runtime_error& error)
		{
			std::cerr << error.what();
			// if error occurs, set *value to nan.
			*value = std::numeric_limits<double>::quiet_NaN();
			return false;
		}
	}
}

// we save the data.
void TimeSeriesTransformations::saveData(std::string filename) const
{
	// create an out fstream.
	std::ofstream _file;

	// open file.
	_file.open(filename + ".csv");

	// if the file is open correctly copy the data of teh TST object line by line into the file.
	if (_file.is_open())
	{
		_file << "Date, " << _name << '\n';
		for (int i = 0; i < _data.size(); i++)
		{
			_file << _data[i].first << getSeparator() << _data[i].second << '\n';
		}

		// close the file.
		_file.close();
	}
	return;
}


// same as above but saves the data to with timestamps in HRD.
void TimeSeriesTransformations::saveDataHumanDate(std::string filename) const
{
	std::ofstream _file;
	_file.open(filename + ".csv");

	if (_file.is_open())
	{
		_file << "Date, " << _name << '\n';
		for (int i = 0; i < _data.size(); i++)
		{
			_file << convertToDate(_data[i].first) << getSeparator() << _data[i].second << '\n';
		}
		_file.close();
	}
	return;
}

// get separator function.
char TimeSeriesTransformations::getSeparator() const
{
	return _separator;
}

// destructor.
TimeSeriesTransformations::~TimeSeriesTransformations()
{
	return;
}