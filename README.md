<Qxlsx> install
  1. install Perl(State Tool): https://docs.activestate.com/platform/state/install/
    1_1 cmd: powershell -Command "& $([scriptblock]::Create((New-Object Net.WebClient).DownloadString('https://platform.activestate.com/dl/cli/install.ps1')))"
    1_2 cmd: state checkout tangxing806/ActivePerl-5.28 .
    1_3 cmd: state use ActivePerl-5.28 (state shell ActivePerl-5.28)
  2. https://blog.csdn.net/u014779536/article/details/111769792
  3. 下载QtXlsx源码: https://github.com/dbzhang800/QtXlsxWriter
