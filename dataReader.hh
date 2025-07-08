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
	using vector_data = std::vector< std::vector<std::string>>;

	vector_data data_info;
	std::vector<int> data_size;
	size_t total_register_size, register_count;
	std::string table_name;

	DataReader():
		data_info(), data_size(), table_name(), total_register_size(0), register_count(0) { }

	void get_format(std::string table_data_path)
	{
		std::ifstream file(table_data_path);
		std::string line;


		while (std::getline(file, line))
		{
			std::vector<std::string> row = get_line_info(line);
			std::string data_type;

			if (row[0] == "CREATE" && row[1] == "TABLE")
			{
				std::string temp_table_name = row[2];

				if (temp_table_name.back() == '(')
					temp_table_name.pop_back();

				table_name = temp_table_name;
			}

			if (is_a_type(row, data_type))
			{
				std::string name = row[0];

				if (data_type == "BOOLEAN" || data_type == "DATE")
				{
					std::string length;

					if (data_type == "BOOLEAN")
						length = '1';
					else
						length = "10";

					data_info.push_back({ name, data_type, length});
					data_size.push_back(std::stoi(length));
				}
				else
				{
					// Combinacion
					std::string length = row[1];

					if (data_type == "BOOLEAN")
						length = "1";
					else if (data_type == "DATE")
						length = "10";
					else
					{
						size_t start = length.find('(');
						size_t end = length.find(')');
						length = length.substr(start + 1, end - start - 1);
					}
					
					data_info.push_back({ name, data_type, length });

					// Comma pos
					size_t comma_pos = length.find(',');
					if (comma_pos == std::string::npos) // NO ES DECIMAL
						data_size.push_back(std::stoi(length));
					else								// ES DECIMAL
					{
						data_size.push_back(std::stoi(length.substr(0, comma_pos)));
						data_size.back()++;
					}
				}
			}
		}


		for (int i = 0; i < data_size.size(); i++)
			total_register_size += data_size[i] + 1;

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

			if (line == "\n" || line == "\0")
				continue;

			std::vector<std::string> row = get_table_info(line);



			std::string merged_row = merge_data_row(row, data_info, data_size);
			
			data_line += merged_row + '/';

			line_counter++;
			register_count++;
		}

		file.close();
		return data_line;
	}

	void write_data_on_txt(std::string disk_path, std::string& data, Disk &disk)
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

		Disk& disk = iterator.get_disk();
		int sector_size{ disk.get_sector_size() };
		int maximum_registers = disk.get_remnant_space() / total_register_size;

		//debug();
    
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

			std::string sub_str = data.substr(lower, upper - lower);

			int register_id = std::stoi(sub_str.substr(0, sub_str.find(',')));

			std::vector<size_t> initial_pos, final_pos;

			//std::cout << i << " : ";

			file << register_id << " ";
			for (int j = 0; j < data_size.size(); j++)
			{
				initial_pos = iterator.get_position();

				for (int k = 0; k < data_size[j]; ++k)
				{
					iterator.set_data(data[counter]);
					std::cout << iterator.current_char();
					iterator.next();
					counter++;
				}

				final_pos = iterator.get_position();

				
				for (auto pos : initial_pos)
					file << pos << " ";

				for (auto pos : final_pos)
					file << pos << " ";


				counter++;
				iterator.next();
			}
			std::cout << "\n";

			file << "\n";
		}
	}
	
	// extract data from metadata file
	std::vector<std::vector<std::string>> read_meta_data(std::string meta_data_path, std::vector<int> in_ids)
	{
		std::ifstream file(meta_data_path);
		std::string line;
		std::vector<std::vector<std::string>> results;
		while (std::getline(file, line))
		{
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
				while (iss >> token)
					result.push_back(token);

				results.push_back(result);
			}
		}
		return results;
	}

	std::vector<std::vector<std::string>> read_all_meta_data(std::string meta_data_path)
	{
		std::ifstream file(meta_data_path);
		std::string line;
		std::vector<std::vector<std::string>> results;
		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string id_str;
			if (!(iss >> id_str))
				continue;
			int id_int = std::stoi(id_str);

			
			std::vector<std::string> result;
			result.push_back(id_str); // first

			std::string token;
			while (iss >> token)
				result.push_back(token);

			results.push_back(result);
			
		}
		return results;
	}

	std::vector<int> get_field_size() { return data_size; }

	void debug()
	{
		show_data_info();
		//std::cout << "\n\n";
		show_data_size();
		//std::cout << "\n\n";
		std::cout << "REGISTROS: " << register_count << "\n";
	}

	size_t get_register_size() { return total_register_size; }

private:
	std::vector<std::string> get_table_info(std::string& line)
	{
		std::vector<std::string> data;
		std::string palabra;
		bool comillas_abierta{ false };

		char comilla_symbol{ '\0' };
		for (char c : line)
		{

			if (((c == '"') || (c == '\'')) && !comillas_abierta)
			{
				if (c == '"')
					comilla_symbol = '"';
				else
					comilla_symbol = '\'';

				comillas_abierta = true;
				continue;
			}

			if (c == comilla_symbol && comillas_abierta)
			{
				comillas_abierta = false;
				continue;
			}


			if (c == ',' && !comillas_abierta) // Push palabra
			{
				data.push_back(palabra);
				palabra.clear();
				continue;
			}

			if (comillas_abierta)
				palabra += c;
			else if (c != ' ' && c!= ',' && !comillas_abierta) //NO ESTA ENTRE COMILLAS
				palabra += c;
		}
		if (!palabra.empty())
			data.push_back(palabra);


		
		return data;
	}

	std::vector<std::string> get_line_info(std::string& line)
	{
		std::vector<std::string> row;
		std::string palabra;
		bool dentro_parentesis{ false };

		for (char c : line) {
			if (std::isalnum(c) || c == '_')
				palabra += c;

			else if (c == '(')
			{
				dentro_parentesis = true;
				palabra += c;
			}
			else if (c == ')')
			{
				palabra += c;
				dentro_parentesis = false;
			}
			else if (c == ',' && dentro_parentesis) {
				palabra += c;  
			}
			else if (!dentro_parentesis)
			{
				if (!palabra.empty())
				{
					row.push_back(palabra);
					palabra.clear();
				}
			}
		}

		if (!palabra.empty())
			row.push_back(palabra);

		return row;
	}

	bool is_a_type(std::vector<std::string>& vector, std::string& data_type)
	{
		std::vector<std::string> TYPES({ "VARCHAR", "INTEGER", "DECIMAL", "BOOLEAN", "DATE" });

		for (auto string : vector)
			for (auto type : TYPES)
				if (string.find(type) != std::string::npos) //ENCONTRADO
				{
					data_type = type;
					return true;
				}


		return false;
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

	std::string encode_value(std::string& type, std::string& number, int& limit, std::string& value)
	{
		std::string temp;

		if (type == "INTEGER")
		{
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
			temp.resize(limit, '0');
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
			temp.resize(limit, '\0');

			for (int j = 0; j < limit && j < value.length(); j++)
				temp[j] = value[j];
		}
		else if (type == "DATE") // item copy
		{
			temp.resize(limit);

			for (int j = 0; j < limit && j < value.length(); j++)
				temp[j] = value[j];
		}
		else if (type == "BOOLEAN") // item copy
		{
			if (value == "TRUE")
				temp = "1";
			else
				temp = "0";
		}

		return temp;
	}
	
	std::string merge_data_row(std::vector<std::string>& row, vector_data& data_info, std::vector<int>& data_size)
	{
		std::string data_line;

		for (int i = 0; i < row.size(); i++)
		{
			std::string& type = data_info[i][1];
			std::string& number = data_info[i][2];
			std::string& value = row[i];

			int limit = data_size[i];

			std::string encoded = encode_value(type, number, limit, value);
			data_line += encoded;

			if (i != row.size() - 1)
				data_line += ',';
		}

		return data_line;
	}
};

#endif