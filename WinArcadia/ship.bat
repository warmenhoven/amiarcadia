@ECHO OFF
del /Q release
rd release
del *.ncb
del *.aps
del *.opt
del *.plg
del *.ini
del *.co?
del *.jbf
del *.nvr
del *.sym
del *.wav
del *.8svx
del *.aiff
del WinArcadia.log
del ..\RAPrefs_WinArcadia.cfg
del _UAEFSDB.___
del /AH Thumbs.db
del ..\WinArcadia.log
del ..\catalogs\_UAEFSDB.___
del /Q ..\Configs\*.*
del /Q /AH ..\Configs\Desktop.ini
del /Q ..\Disks\*.*
del /Q /AH ..\Disks\Desktop.ini
del /Q ..\Games\*.*
del /Q /AH ..\Games\Desktop.ini
del /Q ..\Projects\*.*
del /Q /AH ..\Projects\Desktop.ini
del /Q ..\RACache\Badge\*.*
rd ..\RACache\Badge
del /Q ..\RACache\Bookmarks\*.*
rd ..\RACache\Bookmarks
del /Q ..\RACache\Data\*.*
rd ..\RACache\Data
del /Q ..\RACache\UserPic\*.*
rd ..\RACache\UserPic
del /Q ..\RACache\*.*
rd ..\RACache
del /AH ..\Screenshots\Thumbs.db
del /Q ..\Screenshots\*.*
del /Q /AH ..\Screenshots\Desktop.ini
del /Q ..\Tapes\*.*
del /Q /AH ..\Tapes\Desktop.ini
del /AH ..\Backgrounds\Thumbs.db
del ..\*.co?
del ..\*.pgm
del ..\*.pro
del ..\*.bin
del ..\_UAEFSDB.___
del ..\*.nvr
del ..\*.sym
del ..\*.wav
del ..\*.mid
del ..\*.8svx
del ..\*.aiff
del ..\*.smus
del /Q /AH ..\Desktop.ini

rem cd ..
rem WinArcadia UPGRADE
rem cd source
