@echo off

echo Run Program
C:\Users\User\source\repos\automata\lab5\lab5\x64\Debug\lab5.exe
if %errorlevel% neq 0 goto err

echo Run Python Script to minimize graph
python C:\Users\User\source\repos\automata\lab2\main.py
if %errorlevel% neq 0 goto err

echo Run Python Script to Visualize Graph
python C:\Users\User\source\repos\automata\lab5\graphVisualizator\main.py output.txt
if %errorlevel% neq 0 goto err

echo Opening graph.html in browser
start "" "graph.html"

echo Run Python Script to Visualize Graph
python C:\Users\User\source\repos\automata\lab5\graphVisualizator\main.py minimize.txt
if %errorlevel% neq 0 goto err	

echo Opening graph.html in browser
start "" "graph.html"

goto end

:err
echo An error occurred.
pause

:end
pause
