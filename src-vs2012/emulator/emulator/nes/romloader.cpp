#include "../stdafx.h"

// local header files
#include "../macros.h"
#include "../types/types.h"
#include "../unittest/framework.h"

#include "internals.h"
#include "debug.h"
#include "rom.h"

static MIRRORING mirroring;
static uint8_t mapper;
static uint8_t prgCount, chrCount;
static flag_set<uint8_t,ROMCONTROL1> romCtrl;
static flag_set<uint8_t,ROMCONTROL2> romCtrl2;
static char *trainerData;
static size_t trainerSize;
static char *imageData;
static size_t imageSize;
static char *vromData;
static size_t vromSize;

namespace rom
{
	bool load( const _TCHAR *romFile )
	{
		FILE *fp=NULL;
		uint8_t nesMagic[4]={0};
		uint8_t reserved[8]={0};

		// open rom file
		_tfopen_s(&fp, romFile, _T("rb"));
		if (fp==NULL)
		{
			_tprintf(_T("Couldn't open %s (error code %d)\n"), romFile, errno);
			return false;
		}

		// check signature
		fread(nesMagic,4,1,fp);
		if (memcmp(nesMagic,"NES",3))
		{
			ERROR(INVALID_ROM, INVALID_FILE_SIGNATURE);
			goto onError;
		}

		puts("[-] loading...");

		fread(&prgCount,1,1,fp);
		fread(&chrCount,1,1,fp);
		fread(&romCtrl,1,1,fp);
		fread(&romCtrl2,1,1,fp);
		fread(&reserved,8,1,fp);

		printf("[ ] %u * 16K ROM Banks\n", prgCount);
		printf("[ ] %u * 8K CHR Banks\n", chrCount);
		printf("[ ] ROM Control Byte #1 : %u\n", valueOf(romCtrl));
		printf("[ ] ROM Control Byte #2 : %u\n", valueOf(romCtrl2));

		mapper=romCtrl(ROMCONTROL1::MAPPERLOW);
		mapper|=romCtrl2(ROMCONTROL2::MAPPERHIGH)<<4;
		printf("[ ] Mapper : #%u\n",mapper);

		mirroring=romCtrl[ROMCONTROL1::VERTICALM]?MIRRORING::VERTICAL:MIRRORING::HORIZONTAL;
		if (romCtrl[ROMCONTROL1::FOURSCREEN]) mirroring=MIRRORING::FOURSCREEN;

		printf("[ ] Mirroring type : %u\n", mirroring);
		printf("[ ] Fourscreen mode : %u\n", romCtrl[ROMCONTROL1::FOURSCREEN]);
		printf("[ ] Trainer data present : %u\n", romCtrl[ROMCONTROL1::TRAINER]);
		printf("[ ] SRAM present : %u\n", romCtrl[ROMCONTROL1::BATTERYPACK]);
    
		// read trainer data (if present)
		if (romCtrl[ROMCONTROL1::TRAINER])
		{
			trainerSize = 512;
			trainerData = new char[trainerSize];
			assert(trainerData != NULL);
			if (1 != fread(trainerData, 512, 1, fp)) goto incomplete;
		}

		puts("[ ] reading ROM image...");
		// read rom image
		imageSize = prgCount*0x4000;
		imageData = new char[imageSize];
		assert(imageData != NULL);
		if (prgCount != fread(imageData, 0x4000, prgCount, fp))
		{
	incomplete:
			ERROR(INVALID_ROM, UNEXPECTED_END_OF_FILE);
	onError:
			fclose(fp);
			return 0;
		}

		puts("[ ] reading VROM data...");
		// read VROM
		vromSize = chrCount*0x2000;
		vromData = new char[vromSize];
		assert(vromData != NULL);
		if (chrCount != fread(vromData, 0x2000, chrCount, fp)) goto incomplete;

		// done. close file
		fclose(fp);
		puts("[-] loaded!");
		return true;
	}

	void unload()
	{
		SAFE_DELETE(trainerData);
		SAFE_DELETE(imageData);
		SAFE_DELETE(vromData);
	}

	int mapperType()
	{
		return mapper;
	}

	MIRRORING mirrorMode()
	{
		return mirroring;
	}

	void setMirrorMode(MIRRORING newMode)
	{
		mirroring = newMode;
	}

	const char* getImage()
	{
		return imageData;
	}

	const char* getVROM()
	{
		return vromData;
	}

	size_t sizeOfImage()
	{
		return imageSize;
	}

	int count16KPRG()
	{
		return prgCount;
	}

	int count8KPRG()
	{
		return prgCount*2;
	}

	size_t sizeOfVROM()
	{
		return vromSize;
	}

	int count8KCHR()
	{
		return chrCount;
	}

	int count4KCHR()
	{
		return chrCount*2;
	}
}