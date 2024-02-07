
UNI_COMPILER = D:\SKYSDK\compiler\mrpbuilder.NET1.exe
ADS_COMPILER = C:\ARM\ADSv1_2\Bin\
ADS_CC = $(ADS_COMPILER)armcc
ADS_LINK = $(ADS_COMPILER)armlink
ADS_FLAGS = -O2 -g -apcs /ropi/rwpi -cpu ARM7EJ-S

MRE_SDK_PATH = C:\MRE_SDK\lib\MRE10\src\

clean:
	del ./bin/*
	
mtk:
	"VXP/BuildApp.bat"
	
win32:
	echo Use Visual Studio (msbuild not supported yet)
	
mrp:
	$(UNI_COMPILER) game.mpr