@echo off
svn update
if %errorlevel% equ 0 (
echo SVN更新结束!
) else (
echo SVN更新文件失败
pause
exit
)

msdev argo_cli.dsp /make "All - Win32 Releas" /rebuild
pause