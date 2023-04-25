#	Build MSI packages

MSB=%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\amd64\msbuild.exe

WIX_LINK = light.exe -ext WixUIExtension -ext WixUtilExtension -dWixUILicenseRtf=doc\groestlcoin_lic.rtf -dWixUIDialogBmp=share\pixmaps\wix-banner.bmp -dWixUIBannerBmp=share\pixmaps\wix-topbanner.bmp


all : groestlcoin-25.0.0-win32.msi groestlcoin-25.0.0-win64.msi

src\qt\res_bitcoin.cpp : src\qt\moc.proj
	cd src\qt
	msbuild moc.proj
	cd ..\..


x86_R_St\groestlcoin-qt.exe : src\groestlcoin.cpp src\qt\res_bitcoin.cpp
	""$(MSB)"" groestlcoin.sln /p:Configuration=R_St,Platform=x86 /v:n

x64_R_St\groestlcoin-qt.exe : src\groestlcoin.cpp src\qt\res_bitcoin.cpp
	""$(MSB)"" groestlcoin.sln /p:Configuration=R_St,Platform=x64 /v:n


groestlcoin-25.0.0-win32.msi : groestlcoin.wxs x86_R_St\groestlcoin-qt.exe
	candle.exe -o groestlcoin-x86.wixobj groestlcoin.wxs
	$(WIX_LINK)  -out $@ groestlcoin-x86.wixobj

groestlcoin-25.0.0-win64.msi : groestlcoin.wxs x64_R_St\groestlcoin-qt.exe
	candle.exe -arch x64 -o groestlcoin-x64.wixobj groestlcoin.wxs
	$(WIX_LINK) -out $@ groestlcoin-x64.wixobj
