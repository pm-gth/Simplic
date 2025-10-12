# What's Simplic?
Simplic is a programming language loosely based on old versions of **BASIC**.
I started this project with the goal of learning how an interpreter works. The 
Simplic interpreter is divided in different modules the main ones being:

* The lexer which tokenizes the code into more readable Tokens
* The parser which based on those tokesn creates Syntax Trees
* The interpreter which executes those trees line by line, loading and storing 
the variables inside a memory bank

## How's the syntax of a Simplic program?
Simplic's keywords must be written in caps, however, variables are case-sensitive,
here is a sample Simplic program:
	
	SET BASE = 4
	SET EXPO = 2
	SET RESULT = 1 # This is a comment

	WHILE EXPO GT 0 DO
    	SET RESULT = RESULT * BASE
    	DECR EXPO
	DONE
	PRINTLN "Result is: " + RESULT
	RETURN 0

More examples can be found in the "simplic_programs/" folder.

## How to build Simplic:
A Makefile is facilitated to build the program, some of its rules are:

* make simplic (default rule): outputs the program to the build/ directory
* make runTest : builds and runs all the unitary tests for each module

Additionally each rule can be prefixed with MODE=debug to build with debug 
symbols and memory leak detection.

## Usage:
Just run the program with a script file as argument
	
> ./simplic simplic_programs/power.sim


# To do
Simplic is still fairly limited, I want to add suppor for goto statements, arrays
and much more.

# Special thanks
To the Unity unit-testing framework, used to make this program's tests, it can 
be found here:  

https://github.com/ThrowTheSwitch/Unity
