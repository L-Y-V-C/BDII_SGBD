#ifndef DATAREADER_HH
#define DATAREADER_HH

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "diskManager.hh"


class DataReader
{
public:

	using vector_data = std::vector< std::vector<std::string> >;

	DataReader():
		data_info(), data_size(), total_register_size(0), register_count(0) { }

	void get_format(std::string table_data_path)
	{
		std::ifstream file(table_data_path);
		std::string line;

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
				length = length.substr(start + 1, end - start - 1);

				data_info.push_back({ name, type, length });

				// Comma pos
				size_t comma_pos = length.find(',');

				if (comma_pos == std::string::npos) //NO ENCONTRADO
					data_size.push_back(std::stoi(length));
				else
					data_size.push_back(std::stoi(length.substr(0, comma_pos)));

			}
		}

		
		for (int i = 0; i < data_info.size(); i++)
		{
			total_register_size += data_size[i] + 1;

			if (data_info[i][1] == "DECIMAL") // Because of the extra point, decimal requires
				total_register_size++;

		}	

		file.close();
	}

	std::string read_data(std::string data_path, std::string table_data_path)
	{
		std::ifstream file(data_path);
		std::string line, data_line;

		get_format(table_data_path); // Update data_info

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


			for (int i = 0; i < row.size(); i++)
			{

				std::string& name{ data_info[i][0] },
					& type{ data_info[i][1] },
					& number{ data_info[i][2] },
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

				data_line += temp;
				if (i != (row.size() - 1))
					data_line += ',';
					
			}
			line_counter++;
			register_count++;
			data_line += '/';
		}

		file.close();
		return data_line;
	}

	void write_data_on_disk(std::string disk_path, std::string& data, Disk &disk)
	{
		std::ofstream file(disk_path);

		if (!file.is_open())
		{
			std::cerr << "Error al abrir el archivo " << disk_path << '\n';
			return;
		}


		int sector_size{ disk.get_sector_size() };
		int maximum_registers = disk.get_remnant_space() / total_register_size;
		
		int current_registers{ 0 };
		for (int i = 0; i < data.size(); i++)
		{
			if (current_registers >= maximum_registers)
			{
				std::cerr << "No se pudieron escribir todos los registros!, escrito hasta el registro: " << current_registers << "\n";

				return;
			}
			if ((i % total_register_size == 0) && i > 0)
				current_registers++;
			if ((i % sector_size == 0) && i > 0)
				file << "\n";
			file << data[i];
		}

		file.close();
	}

	void write_data(DiskManager& iterator, std::string &data, std::string meta_data_path)
	{
		std::ofstream file(meta_data_path);

		if (!file.is_open())
		{
			std::cerr << "Error al abrir el archivo " << meta_data_path << '\n';
			return;
		}

		Disk& disk = iterator.disk;
		int sector_size{ disk.get_sector_size() };
		int maximum_registers = disk.get_remnant_space() / total_register_size;



		int counter{ 0 };
		for (int i = 0; i < register_count; i++)
		{
			if (i >= maximum_registers)
			{
				std::cerr << "No se pudieron escribir todos los registros!, escrito hasta el registro: " << i << "\n";
				return;
			}

			int lower = i * total_register_size,
				upper = (i+1) * total_register_size;

			std::string sub_str = data.substr(lower, upper);

			int register_id = std::stoi(sub_str.substr(0, sub_str.find(',')));


			// Save positions, before and after inserting a register
			std::vector<size_t> initial_pos = iterator.get_position();

			for (int j = lower; j < upper; j++, iterator.next(), counter++)
				iterator.set_data(data[j]);

			std::vector<size_t> final_pos = iterator.get_position();


			// Save meta data
			file << register_id << " ";
			for (auto i : initial_pos)
				file << i << " ";
			
			for (int i = 0; i < final_pos.size(); i++)
			{
				file << final_pos[i];
				if (i == final_pos.size() - 1)
					file << "\n";
				else
					file << " ";
			}

		}
	}
	// extract data from metadata file
	std::vector<std::vector<std::string>> read_meta_data(std::string meta_data_path, std::vector<int> in_ids)
	{
		std::ifstream file(meta_data_path);
		std::string line;
		std::vector<std::vector<std::string>> results;
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			std::string id_str;
			if (!(iss >> id_str))
				continue;
			int id_int = std::stoi(id_str);

			if (std::find(in_ids.begin(), in_ids.end(), id_int) != in_ids.end())
			{
				std::vector<std::string> result;
				result.push_back(id_str); // first

				std::string token;
				while (iss >> token) {
					result.push_back(token);
				}
				results.push_back(result);
			}
		}
		return results;
	}

	std::vector<int> extractSizes()
	{
		std::vector<int> sizes;
		std::string tmp;
		int sizeInt;
		for (auto x : data_info) {
			tmp = x[2];
			if (x[1] == "DECIMAL") {
				size_t pos = tmp.find(',');
				tmp = (pos != std::string::npos) ? tmp.substr(0, pos) : tmp;
				sizeInt = std::stoi(tmp) + 1;
			}
			else {
				sizeInt = std::stoi(tmp);
			}
			sizes.push_back(sizeInt);
		}
		return sizes;
	}

	void debug()
	{
		show_data_info();
		//std::cout << "\n\n";
		show_data_size();
		//std::cout << "\n\n";
		std::cout << "REGISTROS: " << register_count << "\n";
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

	void show_data_info()
	{
		for (auto i : data_info)
		{
			for (auto j : i)
				std::cout << j << "   ";
			std::cout << "\n";
		}
	}

	void show_data_size()
	{
		std::cout << "TOTAL: " << total_register_size << " - " << data_size.size() << " campos\n";
		for (auto i : data_size)
			std::cout << i << "   ";
		std::cout << "\n";

	}
public:
	vector_data data_info;
	std::vector<int> data_size;
	size_t total_register_size, register_count;
};

#endif