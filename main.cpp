#include <iostream>
#include <fstream>
#include <sys/stat.h>

using namespace std;

int main(int argc, char *argv[])
{
    cout << "GARCExtract (4th January, 2019) by owocek" << endl;

    if(argc < 2)
    return 0;

    string file = argv[1];
    string folder = "@"+file.substr(file.find_last_of("\\")+1);

    system(string("md \""+folder+"\"").c_str());

    ifstream garc(file, ios::binary);

    garc.seekg(0x28);
    uint32_t file_amount;
    garc.read(reinterpret_cast<char*>(&file_amount), sizeof(uint32_t));

    cout << file_amount << " files detected" << endl;

    for(int i=0; i<file_amount; i++)
    {
        int addr = 0x40 + (0x18 * i);

        garc.seekg(addr+0x4);

        uint32_t file_size;
        garc.read(reinterpret_cast<char*>(&file_size), sizeof(uint32_t));

        garc.seekg(addr+0x8);

        uint32_t data_offset;
        garc.read(reinterpret_cast<char*>(&data_offset), sizeof(uint32_t));

        garc.seekg(addr+0xC);

        uint32_t name_offset;
        garc.read(reinterpret_cast<char*>(&name_offset), sizeof(uint32_t));

        ///Getting file's name
        garc.seekg(name_offset);
        char buff = 0xFF;
        int len = 0;
        string file_name;

        while(buff != 0x0)
        {
            garc.get(buff);

            len++;
            file_name += buff;

            garc.seekg(name_offset+len);

            //cout << "buff: " << buff << endl;
        }

        cout << "Extracting " << file_name << endl;

        int filesize_left = file_size;
        int chunk_size = 10000;
        int offset = data_offset;

        ///opening new file to clear out
        ofstream outa(folder+"\\"+file_name, ios::binary | ios::trunc);
        outa.close();

        ofstream out(folder+"\\"+file_name, ios::binary | ios::app);

        while(filesize_left > chunk_size)
        {
            garc.seekg(offset);
            char fl[chunk_size];
            garc.read(fl,chunk_size);

            out.write(fl,chunk_size);

            offset += chunk_size;
            filesize_left -= chunk_size;
        }

        garc.seekg(offset);
        char f1[filesize_left];
        garc.read(f1,filesize_left);

        out.write(f1,filesize_left);
        out.close();
    }

    system("pause");

    return 0;
}
