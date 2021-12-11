@echo on
cd /d "%~dp0"
set project_path=%~dp0..\projects

rem remove cache [.vs]
for /f  %%p in ('dir "%project_path%" /b /ad') do call:remove_cache %project_path%\%%p

goto:eof
:remove_cache
set remove_cache_path=%1
set cache_path=
set cache_path=%cache_path% "%remove_cache_path%\.vs"
set cache_path=%cache_path% "%remove_cache_path%\out"
rd /s /q %cache_path%
goto:eof