# skerl
<h1 align="center">
	<img
		width="500"
		alt="skerl"
		src="https://github.com/KrishnanSG/skerl/blob/master/logo.svg">
</h1>

In this project we have developed a basic, yet powerful shell --**skerl** for the unix/linux systems.

## Getting Started

These instructions will get you a copy of the project and ready for use on your local unix/linux machine.

### Prerequisites
The shell developed is target for unix/linux systems only.

  #### Quick Access
  - Click on the link to download skerl - [skerl.zip](https://github.com/KrishnanSG/skerl/files/4505232/skerl.zip)
  
  - Extract the zip folder
  
  #### Release
  - You can download the latest release of skerl from [here](https://github.com/KrishnanSG/skerl/releases).

  #### Developer Style
  - GNU GCC complier

  - Clone this repository using the command:

    ```
      git clone https://github.com/KrishnanSG/skerl.git
      cd skerl
    ```
    
  - Complie and create executable

    ```
      gcc skerl.c -o skerl.out
    ```

### How to Use

You're almost there. 
The following steps will guide you on how to get start with skerl.

1. Open your favourite terminal
2. Starting skerl shell
     ```
          ./skerl.out
     ```
3. Enjoy skerl 😀. 

> You may use the help command to get help regarding the shell

## What can skerl do?
   - Parse and tokenize the given command
   - Execute any given program
   - Maintains history of all commands for quick access
   - Input redirection
```
eg: /home/user$ wc -l < story.txt
```
   - Output redirection
```
eg: /home/user$ ls > out.txt
/home/user$ ls >> out.txt	// append mode
```
   - Pipes
```
eg: /home/user$ cat story.txt | wc -l 
```
   - Execute process in background when suffixed with (&)
```
eg: /home/user$ program.o -arg1 -arg2 &
``` 
   - Provides stats on usage of shell. Try out the following internal commands.
```
globalusage
averageusage
```

## How does this work?
This is a really interesting section as developer you would love to visit. Keeping this in mind we have provide a pretty detailed explaination. 

The [report](https://github.com/KrishnanSG/skerl/blob/master/Report.pdf) we have created contains:
- What is a shell?
- Lifetime of a shell
- Internal vs External commands
- Implementation of skerl
- Working of skerl
- Conclusion


## Authors

* **Krishnan S G** - [@KrishnanSG](https://github.com/KrishnanSG)
* **Sadham Hussain** - [@Sadham-Hussian](https://github.com/Sadham-Hussian)

