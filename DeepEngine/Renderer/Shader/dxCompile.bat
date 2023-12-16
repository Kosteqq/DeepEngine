
set /p var0=[VERTEX]Enter input name : 
set /p var1=[FRAGMENT]Enter input name : 

dxc.exe -spirv -T vs_6_0 -E main .\%var0%.hlsl -Fo .\vert.spv
dxc.exe -spirv -T ps_6_0 -E main .\%var1%.hlsl -Fo .\frag.spv
pause