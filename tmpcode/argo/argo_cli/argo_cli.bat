@echo off
svn update
if %errorlevel% equ 0 (
echo SVN���½���!
) else (
echo SVN�����ļ�ʧ��
pause
exit
)

msdev argo_cli.dsp /make "All - Win32 Releas" /rebuild
pause