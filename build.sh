#!/bin/bash

libs=-luser32
warnings=-Wno-writable-strings # disables c++11 string vs char* wr

clang -g src/main.cpp -o celeste.exe $libs $warnings