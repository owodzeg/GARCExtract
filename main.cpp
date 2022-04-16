#include <iostream>
#include <fstream>
//#include <sys/stat.h>

//using namespace std;

int main(int argc, char *argv[])
{
	std::cout << "GARCExtract (4th January, 2019) by owocek\n";

	if (argc < 2) {
		std::cout << "Usage: input_filename";
		std::cout << "Files are extracted into the directory \"@input_filename\" next to the directory where the executable is located";
		return 0;
	}

	std::string file = argv[1];

	std::ifstream garc(file, std::ios::binary);
	if (!garc.is_open()) {
		std::cout << "Error! File " << argv[1] << " can't be opened\n";
		return 0;
	}

	std::string folder = "@" + file.substr(file.find_last_of("\\") + 1);

	system(std::string("md \"" + folder + "\"").c_str());

	//std::ifstream garc(file, std::ios::binary);

	garc.seekg(0x28);
	uint32_t file_count;
	garc.read(reinterpret_cast<char*>(&file_count), sizeof(uint32_t));

	std::cout << file_count << " files detected\n";

	for (int i = 0; i < file_count; i++)
	{
		int addr = 0x40 + (0x18 * i);

		garc.seekg(addr + 0x4);

		uint32_t file_size;
		garc.read(reinterpret_cast<char*>(&file_size), sizeof(uint32_t));

		garc.seekg(addr + 0x8);

		uint32_t data_offset;
		garc.read(reinterpret_cast<char*>(&data_offset), sizeof(uint32_t));

		garc.seekg(addr + 0xC);

		uint32_t name_offset;
		garc.read(reinterpret_cast<char*>(&name_offset), sizeof(uint32_t));

		///Getting file's name
		garc.seekg(name_offset);
		char buff = 0xFF;
		int len = 0;
		std::string file_name;

		while (buff != 0x0)
		{
			garc.get(buff);

			len++;
			file_name += buff;

			garc.seekg(name_offset + len);

			//std::cout << "buff: " << buff << endl;
		}

		std::cout << "Extracting " << file_name.c_str() << "\n";

		int filesize_left = file_size;
		const int chunk_size = 10000;
		int offset = data_offset;

		///opening new file to clear out
		std::ofstream outa(folder + "\\" + file_name, std::ios::binary | std::ios::trunc);
		outa.close();

		std::ofstream out(folder + "\\" + file_name, std::ios::binary | std::ios::app);

		while (filesize_left > chunk_size)
		{
			garc.seekg(offset);
			char fl[chunk_size];
			garc.read(fl, chunk_size);

			out.write(fl, chunk_size);

			offset += chunk_size;
			filesize_left -= chunk_size;
		}

		garc.seekg(offset);
		//char f1[filesize_left];
		char f1[chunk_size];
		garc.read(f1, filesize_left);

		out.write(f1, filesize_left);
		out.close();
	}

	//system("pause");

	return 0;
}
