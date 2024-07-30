## Сборка под Linux

Для запуска должен быть установлен ANTLR4:
    https://www.antlr.org/

Вот пример конфигурирования для Release и Debug:
```
# mkdir -p build-release 
# cd build-release
# cmake .. -DCMAKE_BUILD_TYPE=Release
# cmake --build .

# mkdir -p build-debug
# cd build-debug
# cmake .. -DCMAKE_BUILD_TYPE=Debug
# cmake --build .

```
## Запуск

В папке `build` выполнить команду
```
./bin/spreadsheet
```
