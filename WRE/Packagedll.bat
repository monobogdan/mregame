"C:\MRE_SDK\tools\DllPackage.exe" "D:\UNISDK\ScrollShooter\WRE\WRE.vcproj"
if %errorlevel% == 0 (
 echo postbuild OK.
  copy WRE.vpp ..\..\..\MoDIS_VC9\WIN32FS\DRIVE_E\WRE.vpp /y
exit 0
)else (
echo postbuild error
  echo error code: %errorlevel%
  exit 1
)

