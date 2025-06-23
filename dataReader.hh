#ifndef DATAREADER_HH
#define DATAREADER_HH

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>


class DataReader
{
public:
	using data_line = std::vector<std::vector<std::string>>;

	// OLD
	data_line read_data_arr_arr_str(std::string data_path)
	{
		data_line data_rows;

		std::ifstream file(data_path);

		std::string line;
		std::stringstream ss(line); 

		while (std::getline(file, line))
		{
			std::stringstream ss(line);
			std::string field;

			std::vector<std::string> row;
			while (std::getline(ss, field, ','))
				row.push_back(field);
			
			data_rows.push_back(row);
		}		
		
		file.close();
		return data_rows;
	}

	std::vector<int> get_meta_data_size(std::string meta_data_path)
	{
		std::vector<int> data_size;
		std::ifstream file(meta_data_path);

		std::string line;
		while (std::getline(file, line))
		{
			std::vector<std::string> row = get_row(line, ',');
			data_size.push_back(std::stoi(row[1]));
		}

		
		file.close();
		return data_size;
	}

	std::string read_data(std::string data_path, std::string meta_data_path)
	{
		std::vector<int> data_size{ get_meta_data_size(meta_data_path) };

		std::ifstream file(data_path);
		std::string line, data_line;

		while (std::getline(file, line))
		{
			std::vector<std::string> row = get_row(line, ',');

			for (int i = 0; i < data_size.size(); i++)
			{
				if (data_size[i] != -1) // Es string
					data_line += format_string(data_size[i], row[i], '-');
				else					// No es string
					data_line += row[i];

				data_line += ',';       // Field separator
			}
			data_line += '/';			// Register separator
		}

		file.close();
		return data_line;
	}

	void write_data(int in_sector_size)
	{

	}

private:
	std::string format_string(int new_str_size, std::string& str_to_format, char in_char)
	{
		std::string to_return(new_str_size, in_char);

		for (int i = 0; i < str_to_format.length() && i < new_str_size; i++)
			to_return[i] = str_to_format[i];

		return to_return;
	}

	std::vector<std::string> get_row(std::string& line, char delimiter = ',')
	{
		std::vector<std::string> row;
		std::stringstream ss(line);

		std::string field;
		while (std::getline(ss, field, delimiter))
			row.push_back(field);

		return row;
	}
};

#endif