#include <windows.h>
#include <iostream>
#include <string>
#include "driver.h"
#include "kdmapper/kdmapper.hpp"
#include "xorstr.hpp"

#pragma comment(lib, "version.lib")

//ioctl codes for the driver
//https://media.discordapp.net/attachments/772876827562606593/773471546282082314/unknown.png
#define init CTL_CODE(FILE_DEVICE_UNKNOWN, 0x69, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define nullspoof CTL_CODE(FILE_DEVICE_UNKNOWN, 0x213, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define randomserialspoofreq CTL_CODE(FILE_DEVICE_UNKNOWN, 0x428, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define customserial CTL_CODE(FILE_DEVICE_UNKNOWN, 0x492, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define randomserial CTL_CODE(FILE_DEVICE_UNKNOWN, 0x920, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

struct CHEATING_WIN
{
	CONST CHAR* SERIAL;
};

int main()
{
	DWORD byteret = 0;
	int type = 0;
	std::string serial = "";
	CHEATING_WIN in;

	//unload and delete intel driver
	service::StopAndRemove(E("iqvw64e.sys"));
	system(E("del /f /q C:\\Users\\%username%\\AppData\\Local\\Temp\\iqvw64e.sys"));
	system(E("cls"));

	//write hwrwdrv.sys because driver hooks into its ioctl shit
	std::ofstream hwrwdrv(E("C:\\Nigger.sys"), std::ios::binary);
	hwrwdrv.write((const char*)driver_hwrwdrv, sizeof(driver_hwrwdrv) );
	hwrwdrv.close();

	//start hwrwdrv.sys
	if (!service::RegisterAndStart(E("C:\\Nigger.sys")))
	{
		std::cout << E("[-] Warning: HwRwDrv.sys was not able to load successfully either because it was already loaded or something!") << std::endl;
	}

	//start intel driver
	HANDLE iqvw64e_device_handle = intel_driver::Load();
	if (!iqvw64e_device_handle || iqvw64e_device_handle == INVALID_HANDLE_VALUE)
	{
		//error
		MessageBoxA(0, E("Intel Vulner Driver Failed To Load!"), E("Fatal Error Cheating.Win V2"), MB_ICONERROR);
		//exit
		exit(1);
	}

	//map cheating.win driver
	if (!kdmapper::MapDriver(iqvw64e_device_handle))
	{
		std::cout << E("[-] Failed to map driver") << std::endl;
		intel_driver::Unload(iqvw64e_device_handle);
		return -1;
	}

	//unload intel driver
	intel_driver::Unload(iqvw64e_device_handle);

	//create handle to disk
	HANDLE drv_handle = CreateFileW(E(L"\\\\.\\PhysicalDrive0"), GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	
	//check if handle is invalid
	if (drv_handle == INVALID_HANDLE_VALUE)
	{
		//error
		MessageBoxA(0, E("wtf? disk drive handle invalid, ur disk has some issues lmfao or u need to run this as admin!"), E("Fatal Error Cheating.Win V2"), MB_ICONERROR);
		//exit
		exit(1);
	}

	//init
	DeviceIoControl(drv_handle, init, 0, 0, 0, 0, &byteret, 0);

	A:

	std::cout << E("\n\nCHEATING.WIN V2 CRACK BY THE ONE AND ONLY ZERAX NIGGER :)\n\n[1] Serial Nulled\n[2] Random Serial Every Time Serial Is Queried\n[3] Custom Serial\n[4] Random Serial\n\n[Select]: ");

	//set the type to 0
	type = 0;

	//get user input
	std::cin >> type;

	//stop using if statements for everything retards.
	switch (type)
	{
	case 1:
		DeviceIoControl(drv_handle, nullspoof, 0, 0, 0, 0, &byteret, 0);
		break;
	case 2:
		DeviceIoControl(drv_handle, randomserialspoofreq, 0, 0, 0, 0, &byteret, 0);
		break;
	case 3:
		std::cout << E("[Custom Serial]: ");
		std::cin >> serial;
		in.SERIAL = serial.c_str();
 		DeviceIoControl(drv_handle, customserial, &in, sizeof(in), 0, 0, &byteret, 0);
		break;
	case 4:
		DeviceIoControl(drv_handle, randomserial, 0, 0, 0, 0, &byteret, 0);
		break;
	default:
		system(E("cls"));
		goto A;
		break;
	}

	std::cout << E("\n\nDone! Exiting in 5 seconds!") << std::endl;
	Sleep(5000);
	exit(0);
}
