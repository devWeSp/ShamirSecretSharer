# Shamir Secret Sharing
Application for sharing and recovering secrets among multiple people.

## What it does
Application that can divide secret into selected number of shares with selected number of required shares to correctly reconstruct secret. 

It also can reconstruct secret, when at least number of required shares are provided. Whenever less shares or wrong shares provided, program will output "valid" output, but with wrong secret (meaning, that it will output string of characters, but it will definitely not be secret and potential attacker wouldn't know if that was secret or not).

## Usage
### Splitting secret into shares
Just navigate into folder with executable file using Command Line and run
```
>.\ShamirSecret.exe "My top secret secret" 10 5 > DesiredOutputFile.txt
```
This will divide "My top secret secret" into 10 shares with at least 5 of them needed to reconstruct correct secret. Output is redirected into DesiredOutputFile.txt so you can find your shares there. Without the last part (stdout redirection) shares will be printed into console and you can copy them.
 
### Reconstructing shares
Just navigate into folder with executable file and copy (or create) there file containing shares you want to put together, one per line like this, name is up to you, e.g. MyShares.txt

```
001005006026045031050051218043046037030044219
002005014034053039058059226051054045038052227
003005153173192178197198108190193184177191109
004005054074093079098099009091094085078092010
005005212232251237256000167249252243236250168
```
  And using Command Line run 
```
>.\ShamirSecret.exe < MyShares.txt
```
Secret will be computed and outputted into console. You can redirect stdout into whatever file you want of course.

### Starting up into CLI
If you provide wrong or no parameters, application will start into Command Line Interface. You can create shares, reconstruct secrets from there as well, just follow instructions.

## Limitations
### Input encoding 
As of now application supports only standard ASCII characters. Behavior using other characters is undefined (probably punctuation will be stripped).

### Input length
Currently application supports secret of length 1024 characters maximum.

### Threshold value
Maximum threshold is now 256. Enough for now, because generating shares is quite slow and with higher threshold it starts to take some serious time (more about that in next paragraph).

### Number of shares 
Maximum number of shares is 999.

## Parts that need improvements
### Slow running time of generating shares
This part of code could use some optimization, especially part where polynomial is evaluated. Even though I used Horner's scheme for evaluating polynomial, it is still very slow for increasing threshold because degree in polynomial is getting high and meta sum needs to be modulated more often.

### Shares in decimal base
Looking back, it wasn't wise to have shares in decimal base. Every character takes string with 3 bytes and it consumes more memory during computation and shares are very long. It would be cool to be in Base64 or something like that. But let's call decimal base feature for now.

### Support for Unicode
It would be nice to support Unicode encodings. Everything else than ASCII is out for my league for now! 

### Better encoding algorithm
Right now I just use ASCII codes as integer and compute shares using this. It would be great to have some better encoding algorithm (especially if Unicode was supported)

## Development environment
### Built with
* [CodeBlocks](http://www.codeblocks.org/) v16.01 - C/C++ IDE
* [GCC](https://gcc.gnu.org/) v2.95 19990728 - C/C++ compiler

#### Compile flags used
-ansi

### Target platform
Microsoft Windows 10 (in 32-bit mode) 

### Tested on 
* Microsoft Windows 10 [Version 10.0.16299.248]

## Authors
* **devWeSp** - [Website](http://www.devWeSp.com/) 

## License
MIT License

Copyright (c) 2018 devWeSp

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


