
mkdir output
copy /Y "..\build-tanca\release\Tanca.exe" output
cd output
windeployqt Tanca.exe
cd ..
