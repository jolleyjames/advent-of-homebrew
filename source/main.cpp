#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdint>
#include <memory>
#include <chrono>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>

#include "menu.hpp"
#include "solution.hpp"
#include "audio.hpp"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

//---------------------------------------------------------------------------------
int main(int argc, char **argv)
{
	//---------------------------------------------------------------------------------

	std::cout << "entering main()..." << std::endl;

	// Initialise the video system
	VIDEO_Init();

	// This function initialises the attached controllers
	WPAD_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	console_init(xfb, 20, 20, rmode->fbWidth - 20, rmode->xfbHeight - 20, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Clear the framebuffer
	VIDEO_ClearFrameBuffer(rmode, xfb, COLOR_BLACK);

	// Make the display visible
	VIDEO_SetBlack(false);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if (rmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();

	advhb::Menu &menu = advhb::Menu::getInstance();
	bool errors = false;

	// Initialize FAT32 filesystem on SD card
	if (!fatInitDefault())
	{
		menu.setErrorMessage("Could not initialize SD card");
		errors = true;
	}

	if (!errors)
	{
		menu.setSolutions(advhb::Solution::getSolutions());
		// mute if user holds B
		auto start = std::chrono::system_clock::now();
		bool mute = false;
		while (std::chrono::system_clock::now() - start <= std::chrono::milliseconds(1000))
		{

			WPAD_ScanPads();
			u32 pressed = WPAD_ButtonsDown(0);
			if (pressed & WPAD_BUTTON_B)
			{
				mute = true;
				std::cout << "\033[2J";
				std::cout << "\x1b[2;0H";
				std::cout << "Background music muted, press A to continue" << std::endl;
				while (true)
				{
					WPAD_ScanPads();
					pressed = WPAD_ButtonsDown(0);
					if (pressed & WPAD_BUTTON_A)
						break;
					VIDEO_WaitVSync();
				}
			}
			VIDEO_WaitVSync();
		}
		if (!mute)
		{
			// time for some music
			advhb::Audio::getInstance().playBgmMP3(std::filesystem::path("/apps/advhb/media/bgm.mp3"));
		}

		while (1)
		{
			// invoke next line of the menu
			menu.next();
			// app exit handled in menu
		}
	}
}
