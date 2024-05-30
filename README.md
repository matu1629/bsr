# bsr
Binary Search and Replace

# detail
this tool search or replace data for files by specified bin or hex string.

# usage
bsr -p \<path\> -c \<condition\>

## options
### -h
help
### -p \<path\>
folder/directory path
### -c \<condition\>
string condition to search
### -r \<replacement\>
string to replace
### -f \<filecondition\>
file name conditon to search default:.*

you can use regular expression(case insensitive) of ECMAScript.
### -m \<mode\>
bin | hex | dbin | dhex default:hex

if you specify bin or hex mode, you can use regular expression(case insensitive) of ECMAScript for \<conditon\> and \<replacement\>.

if you specify dbin mode, you can use 0, 1 or ? for \<condition\> and \<replacement\>.

"?" is any one charactor(0 or 1).

if you specify dhex mode, you can use 0 ~ 9, A ~ F, a ~ f or ? for \<condition\> and \<replacement\>.

"?" is any one charactor(0 ~ 9, A ~ F, a ~ f).
### -n \<threadno\>
number of threads default:1

## example
### case 1 search (hex mode)
bsr -p directory -c ab01
### case 2 replace (hex mode)
bsr -p directory -c ab01 -r cd23
### case 3 specified file name
bsr -p directory -c ab01 -f .*.txt
### case 4 specified mode
bsr -p directory -c ab01 -m dhex
### case 5 execute by 2 threads
bsr -p directory -c ab01 -n 2
### case 6 use pipe
bsr -p directory -c ab01 | bsr -c cd23

# mode difference of bin and dbin or hex and dhex
if file data is 0b0000111101010101.

    bsr -p directory -c 00001111 -m bin
        0000111101010101
        ~~~~~~~~ hit
    bsr -p directory -c 00001111 -m dbin
        0000111101010101
        ~~~~~~~~ hit
    bsr -p directory -c 0011 -m bin
        0000111101010101
          ~~~~ hit
    bsr -p directory -c 0011 -m dbin
        0000111101010101
        not hit

if file data is 0xab01cd23.

    bsr -p directory -c ab01 -m hex
        ab01cd23
        ~~~~ hit
    bsr -p directory -c ab01 -m dhex
        ab01cd23
        ~~~~ hit
    bsr -p directory -c b0 -m bin
        ab01cd23
         ~~ hit
    bsr -p directory -c b0 -m dbin
        ab01cd23
        not hit

# condition
## c++ compiler
c++ standard is c++17.

because this program use std::filesystem and some c++17 functions.

## cmake
3.1 or later

if you use visual studio 2017 or later, use cmake from developper command prompt.

# build
now, if current directory is bsr.
1. mkdir build
2. cd build
3. cmake ..
4. cmake --build . --config Release

if you use debug mode, please change "Release" to "Debug".

# test
after build step 4.

ctest -C Release

if it success, it output "100% tests passed...".

if you use debug mode, please change "Release" to "Debug".
