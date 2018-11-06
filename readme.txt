Name:
Amit Hampal

Student #:

Description:
The following is a library that implements the RFC 6350 vCard standard. This library makes use of
a linked list api to organize and store vCards in memory. There are several functions:
-createCard to load a saved .vcf or .vcard into memory
-deleteCard to delete card object
-printCard to print card object to terminal
-printError to print an expanded message given a certain error code
-writeCard to save card object to .vcf file
-validateCard to check if card object complies with vCard specifications and Struct Specifications
-JSONtoStruct type functions to parse json strings into memory
-StructtoJSON type functions to create usable json strings from structs

Compilation:
-cd terminal to directory structure
-execute makefile by typing 'make all' into terminal

Assumptions and limitations:
-No known limitations
-assumed no major error checking of json strings
