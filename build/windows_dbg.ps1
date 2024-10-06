if (Test-Path build\output) {
  Remove-Item build\output -Recurse -Force
}
New-Item -ItemType directory -Force -Path build\output
pushd build\output
cl /Zi /Od /FS /Fepot8to_dbg_windows.exe ..\..\main_windows.cpp ..\..\platform_windows.cpp /I..\.. /Fdpot8to_dbg_windows.pdb /link User32.lib Comdlg32.lib Kernel32.lib
popd
