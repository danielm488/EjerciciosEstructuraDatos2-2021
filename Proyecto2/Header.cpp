#include "Header.h"
#include "metaData.h"
#include <iostream>
#include <ctime>;
#include <sstream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <string>
#include "bitMap.h"
#include "fileEntry.h"
#include "bloquesDato.h"
#include <vector>

metaData info;
bitMap bitm;
fileEntry entry;
std::vector<fileEntry> entries; 

void header::crearDisco()
{

	std::cout << "Ingrese nombre disco: ";
	std::cin >> info.nombreDisco;

	std::cout << "\nIngrese propietario disco: ";
	std::cin >> info.propietario;

	std::cout << "\nIngrese cantidad de Bloques: ";
	std::cin >> info.cantBloques;

	std::cout << "\nIngrese cantidad de File Entries: ";
	std::cin >> info.cantFileEntries;

	info.sizeMapaB = ceil(info.cantBloques / 8);

	std::fstream archivoDisco("hola2.bin", std::ios::in | std::ios::binary | std::ios::app);

	if (!archivoDisco)
	{
		std::cout << "\nError al crear Disco";
		throw 1;
	}

	archivoDisco.write(reinterpret_cast<const char*>(&info), sizeof(metaData));
	archivoDisco.close();

	initBitMap(info.nombreDisco, info.cantBloques);
	initFileEntries(info.nombreDisco, info.cantFileEntries);
	initDataBlocks();
	
}

//std::string header::get_time()
//{
//	auto t = std::time(nullptr);
//	auto tm = *std::localtime(&t);
//
//	std::ostringstream oss;
//	oss << std::put_time(&tm, "%d/%m/%Y \t %H:%M:%S");
//	std::string str = oss.str();
//	return str;
//}

void header::initBitMap(char* nombreDisco, int cantEntradas)
{
	
	std::fstream archivoDisco("hola2.bin", std::ios::in | std::ios::binary | std::ios::app);

	if (!archivoDisco)
	{
		std::cout << "Error";
		throw 1;
	}

	bitm.bitMap = new char[ceil(cantEntradas / 8)];

	for ( int i = 0; i < ceil(cantEntradas / 8) ; i++)
	{
		bitm.bitMap[i] = 0;
	}


	archivoDisco.write(reinterpret_cast<const char*>(&bitm), sizeof(bitMap));

	archivoDisco.close();
}

void header::initFileEntries(char* nombreDisco, int cantEntradas)
{
	
	std::fstream archivoDisco("hola2.bin", std::ios::in | std::ios::binary | std::ios::app);

	if (!archivoDisco)
	{
		std::cout << "Error";
		throw 1;
	}

	for (int i = 0; i < 30; i++)
	{
		entry.name[i] = -1;
	}

	entry.type = 'F';
	entry.size = 0;
	entry.padre = -1; 
	entry.hijo = -1;
	entry.nextSibling = -1; 
	entry.primerBloq = -1;
	entry.available = true;

	for (int i = 0; i < cantEntradas; i++)
	{		
		archivoDisco.write(reinterpret_cast<const char*>(&entry), sizeof(fileEntry));
	}

	archivoDisco.close();
}

void header::initDataBlocks()
{
	std::fstream archivoDisco("hola2.bin", std::ios::in | std::ios::binary | std::ios::app);

	if (!archivoDisco)
	{
		std::cout << "Error";
		throw 1;
	}

	bloquesDato bloques;
	bloques.sig = -1;

	for (int i = 0; i < info.cantBloques; i++)
	{
		archivoDisco.write(reinterpret_cast<const char*>(&bloques), sizeof(bloquesDato));
		std::cout << i << "\n";
	}

	archivoDisco.close();

}

void header::cargarDisco()
{
	char nombreDisco[30];
	std::cout << "\nIngrese nombre de Disco: ";
	std::cin >> nombreDisco;

	std::fstream archivoDisco("hola2.bin", std::ios::in | std::ios::binary);

	if (!archivoDisco)
	{
		std::cout << "Error al abrir Disco"; 
		throw 1;
	}

	archivoDisco.seekg(0, std::ios::beg);

	archivoDisco.read(reinterpret_cast<char*>(&info), sizeof(metaData));

	archivoDisco.close();
	 
}

void header::cargarBitMap()
{
	bitm.bitMap = new char[ceil(info.cantBloques / 8)];

	std::fstream archivoDisco("hola2.bin", std::ios::in | std::ios::binary);

	if (!archivoDisco)
	{
		std::cout << "Error al abrir Disco";
		throw 1;
	}

	archivoDisco.seekg(0, std::ios::beg);
	archivoDisco.read(reinterpret_cast<char*>(&info), sizeof(metaData));
	archivoDisco.read(reinterpret_cast<char*>(&bitm), sizeof(ceil(info.cantBloques / 8)));

	archivoDisco.close();

}

void header::cargarFileEntries()
{
	std::fstream archivoDisco("hola2.bin", std::ios::in | std::ios::binary);

	if (!archivoDisco)
	{
		std::cout << "Error al abrir Disco";
		throw 1;
	}

	archivoDisco.seekg(0, std::ios::beg);
	archivoDisco.read(reinterpret_cast<char*>(&info), sizeof(metaData));
	archivoDisco.read(reinterpret_cast<char*>(&bitm), sizeof(bitMap));

	for (int i = 0; i < info.cantFileEntries; i++)
	{
		archivoDisco.read(reinterpret_cast<char*>(&entry), sizeof(fileEntry));
		entries.push_back(entry);

	}

	archivoDisco.close();

}

void header::crearDirectorio()
{
	char nombreDir[30];

	std::cout << "\nIngrese nombre directorio nuevo: ";
	std::cin >> nombreDir;

	std::fstream archivoDisco(info.nombreDisco, std::ios::in | std::ios::binary);

	if (!archivoDisco)
	{
		std::cout << "Error";
		throw 2;
	}

	archivoDisco.seekg(0, std::ios::beg);
	archivoDisco.read(reinterpret_cast<char*>(&info), sizeof(metaData));
	archivoDisco.read(reinterpret_cast<char*>(&bitm), sizeof(bitMap));

	long pos = archivoDisco.tellg();
	archivoDisco.read(reinterpret_cast<char*>(&entry), sizeof(fileEntry));

	for (int i = 0; i < info.cantFileEntries; i++)
	{
		if (entry.available == true)
		{
			strcpy_s(entry.name, nombreDir);
			entry.type = 'D';
			entry.size = sizeof(fileEntry);
			entry.padre = 0;
			entry.nextSibling = -1;
			entry.hijo = -1;
			entry.primerBloq = -1;
			entry.available = false;

			archivoDisco.seekp(pos, std::ios::beg);
			archivoDisco.write(reinterpret_cast<char*>(&entry), sizeof(fileEntry));
			archivoDisco.close();

			std::cout << "Directorio Creado";
			return;
			
		} 
		pos = archivoDisco.tellg();
		archivoDisco.read(reinterpret_cast<char*>(&entry), sizeof(fileEntry));	
	}
	archivoDisco.close();
	std::cout << "\nCantidad Maxima de File Entries, no se ha creado \n";
}

void header::elimDirectorio()
{

}

void header::listarDirectorio()
{

}

void header::importFile()
{
	char nombreArchivo[30]; 
	std::cout << "\nIngrese nombre de archivo a importar: ";
	std::cin >> nombreArchivo;

	std::fstream archivoDisco(info.nombreDisco, std::ios::in | std::ios::binary);

	if (!archivoDisco)
	{
		std::cout << "Error";
		throw 3;
	}

	archivoDisco.seekg(0, std::ios::beg);
	archivoDisco.read(reinterpret_cast<char*>(&info), sizeof(metaData));
	archivoDisco.read(reinterpret_cast<char*>(&bitm), sizeof(bitMap));

	long pos = archivoDisco.tellg();
	archivoDisco.read(reinterpret_cast<char*>(&entry), sizeof(fileEntry));

	for (int i = 0; i < info.cantFileEntries; i++)
	{
		if (entry.available == true)
		{
			strcpy_s(entry.name, nombreArchivo);
			entry.type = 'A';
			//entry.size = sizeof(fileEntry);
			entry.padre = 0;
			entry.nextSibling = -1;
			entry.hijo = -1;
			entry.primerBloq = -1;
			entry.available = false;

			archivoDisco.seekp(pos, std::ios::beg);
			archivoDisco.write(reinterpret_cast<char*>(&entry), sizeof(fileEntry));
			archivoDisco.close();

			std::cout << "Directorio Creado";
			return;

		}
		pos = archivoDisco.tellg();
		archivoDisco.read(reinterpret_cast<char*>(&entry), sizeof(fileEntry));
	}



	//reservar file entry
	//ver tam archivo 
 // reservar datablocks 
	//encontrar primerdatblock vacio en bitmap 


}

void header::exportFile()
{

}