# ShowStopper - Anti-Debug tricks exploration tool

*Contributed by* Check Point Software Technologies LTD.<br/>
*Programmed by* Yaraslau Harakhavik

<p align="center">
    <img src="./pics/showstopper_logo.png" />
</p>

## Overview
The <b>ShowStopper</b> project is a tool to help malware researchers explore and test anti-debug techniques or verify debugger plugins or other solutions that clash with standard anti-debug methods.<br/>
With this tool, you can attach a debugger to its process and research the debugger’s behavior for the techniques you need (the virtual addresses of functions that apply to anti-debug techniques are printed to console) and compare them with their implementation. The tool includes a varied set of different techniques from multiple sources, including real-world malware and published documents and articles. The implemented techniques work for the latest Windows releases and for different modern debuggers.

## Documenattion
How to install and use the tool, and contribute your findings in the [documentation](./DOCS.md/) for the project.

## System Requirements
* Windows 7, 8, 8.1, 10 (x86/x86-64)
* 32-Bit debuggers (OllyDbg, x32dbg, WinDbg, etc.)

## References
* [P. Ferrie. The “Ultimate”Anti-Debugging Reference](http://pferrie.epizy.com/papers/antidebug.pdf)
* [N. Falliere. Windows Anti-Debug Reference](https://www.symantec.com/connect/articles/windows-anti-debug-reference)
* [J. Jackson. An Anti-Reverse Engineering Guide](https://forum.tuts4you.com/files/file/1218-anti-reverse-engineering-guide/)
* [Anti Debugging Protection Techniques with Examples](https://www.apriorit.com/dev-blog/367-anti-reverse-engineering-protection-techniques-to-use-before-releasing-software)
* [simpliFiRE.AntiRE](https://bitbucket.org/fkie_cd_dare/simplifire.antire/src/master/)
