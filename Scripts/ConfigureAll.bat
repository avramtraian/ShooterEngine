::
:: Copyright (c) 2024-2025 Traian Avram. All rights reserved.
:: SPDX-License-Identifier: Apache-2.0.
::

@ECHO OFF
PUSHD "%~dp0..\"

where cl >nul 2>&1
IF ERRORLEVEL 1 (
    ECHO Skipping configuration that use the MSVC toolchain...
) ELSE (
    :: MSVC toolchain with Visual Studio as the build generator. Generates solution files that can also be opened.
    cmake -S . -B Build/Debug-MSVC-VS          -DCMAKE_BUILD_TYPE=Debug       -G "Visual Studio 17 2022"
    cmake -S . -B Build/Development-MSVC-VS    -DCMAKE_BUILD_TYPE=Development -G "Visual Studio 17 2022"
    cmake -S . -B Build/Shipping-MSVC-VS       -DCMAKE_BUILD_TYPE=Shipping    -G "Visual Studio 17 2022"

    :: Clang toolchain with Ninja as the build generator.
    cmake -S . -B Build/Debug-MSVC-Ninja       -DCMAKE_BUILD_TYPE=Debug       -G "Ninja" -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
    cmake -S . -B Build/Development-MSVC-Ninja -DCMAKE_BUILD_TYPE=Development -G "Ninja" -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
    cmake -S . -B Build/Shipping-MSVC-Ninja    -DCMAKE_BUILD_TYPE=Shipping    -G "Ninja" -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
)

:: Clang toolchain with Ninja as the build generator.
cmake -S . -B Build/Debug-Clang-Ninja       -DCMAKE_BUILD_TYPE=Debug       -G "Ninja" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake -S . -B Build/Development-Clang-Ninja -DCMAKE_BUILD_TYPE=Development -G "Ninja" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake -S . -B Build/Shipping-Clang-Ninja    -DCMAKE_BUILD_TYPE=Shipping    -G "Ninja" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

POPD
