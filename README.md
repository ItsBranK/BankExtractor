# Wwise *.BNK File Extractor

This is a modern C++ rewrite and extension of **bnkextr** originally written by CTPAX-X in Delphi.
It extracts `WEM` files from the ever more popular Wwise `BNK` format.

Use [ww2ogg](https://github.com/hcs64/ww2ogg) to convert `WEM` files to the `OGG` format.

## Usage

```
Usage: bankextractor.exe filename.bnk [/swap] [/nodir] [/index] [/info]
	/swap - Swap byte order (use it for unpacking "Army of Two").
	/nodir - Create no additional directory for the *.wem files.
	/index - Writes the internal bnk id for the extracted *.wem file names.
	/info - Prints extra/detailed info to the console while extracting files.
```

## Screenshots

![](https://i.imgur.com/YO3Rh3j.png)
![](https://i.imgur.com/1t8CvQ6.png/)