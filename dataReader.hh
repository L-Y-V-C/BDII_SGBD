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

	using vector_data = std::vector< std::vector <std::string>>;

	vector_data get_format(std::string meta_data_path)
	{
		std::ifstream file(meta_data_path);
		std::string line;
		vector_data vector;

		while (std::getline(file, line))
		{
			std::vector<std::string> row = get_row(line, ' ');

			if (row.size() > 3)
			{
				std::string name = row[0];
				std::string type = row[1].substr(0, row[1].find('('));

				// Combinacion
				std::string length = row[1] + row[2];
				size_t start = length.find('(');
				size_t end = length.find(')');
				length = length.substr(start + 1, end - start);

				vector.push_back({ name, type, length });
			}
		}

		file.close();
		return vector;
	}

	std::string read_data(std::string data_path, std::string meta_data_path)
	{
		std::ifstream file(data_path);
		std::string line, data_line;
		vector_data format_vector{ get_format(meta_data_path) };

		int line_counter{ 0 };

		while (std::getline(file, line))
		{
			if (line_counter++ == 0)
				continue;

			// Get item name without quotation marks
			std::string item;
			size_t first_quote = line.find_first_of('"');
			size_t second_quote = line.find_last_of('"');
			item = line.substr(first_quote + 1, second_quote - first_quote - 1);



			// Fill empty item name with "WORD"
			line = line.substr(0, line.find_first_of(',')) + ",WORD" + line.substr(line.find_last_of('"') + 1, line.size() - (line.find_last_of('"') + 1));
			// Erase spaces
			line.erase(std::remove(line.begin(), line.end(), ' '), line.end());


			std::vector<std::string> row = get_row(line, ',');

			int counter{ 0 };
			for (int i = 0; i < row.size(); i++)
			{

				std::string& name{ format_vector[i][0] },
					& type{ format_vector[i][1] },
					& number{ format_vector[i][2] },


					& value{ row[i] };



				std::string temp;

				if (type == "INTEGER")
				{
					int limit{ std::stoi(number) };
					temp.resize(limit, '0');

					for (int i = 0; i < limit && i < value.size(); i++)
						temp[limit - 1 - i] = value[value.size() - 1 - i];
				}
				else if (type == "DECIMAL")
				{
					size_t comma_pos = number.find(',');

					int num_size{ std::stoi(number.substr(0, comma_pos)) };

					int float_size{ std::stoi(number.substr(comma_pos + 1)) },
						int_size{ num_size - float_size };


					int dot_pos = int_size;
					temp.resize(num_size + 1, '0');
					temp[dot_pos] = '.';


					// String values of int part and float part
					size_t dot_index = value.find('.');
					std::string int_part{ value.substr(0, dot_index) },
						float_part{ value.substr(dot_index + 1, value.size() - dot_index) };

					// Int
					for (int i = 0; i < int_size && i < int_part.size(); i++)
						temp[dot_pos - 1 - i] = int_part[int_part.size() - 1 - i];

					// Float
					for (int i = 0; i < float_size && i < float_part.size(); i++)
						temp[dot_pos + 1 + i] = float_part[i];

				}
				else if (type == "VARCHAR") // item copy
				{
					int limit{ std::stoi(number) };
					temp.resize(limit, '\0');

					for (int i = 0; i < limit && i < item.size(); i++)
						temp[i] = item[i];
				}

				data_line += temp + ',';
			}

			data_line += '/';
		}

		file.close();
		return data_line;
	}

	void write_data(int in_sector_size)
	{

	}

private:
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