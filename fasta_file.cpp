//----------------------------------------------------------------
// Name        : FastaFile.cpp
// Author      : Remco Hoogenboezem
// Version     :
// Copyright   :
// Description : Open a single fasta file can now also be compressed
//----------------------------------------------------------------
#include <iostream>
#include <cstdio>
#include <cstring>
#include "fasta_file.h"
//----------------------------------------------------------------
FastaFile::FastaFile(void) : fileBuffer(nullptr)
{
}
//----------------------------------------------------------------
FastaFile::~FastaFile(void)
{
    Clear();
}
//----------------------------------------------------------------
bool FastaFile::Open(const string & fileName)
{
    //----------------------------------------------------------------
    //Clear previous sequences
    //----------------------------------------------------------------

    Clear();

    //----------------------------------------------------------------
    //Open fasta file
    //----------------------------------------------------------------

    FILE * fastaFile=popen((string("bzcat -f ")+fileName+string(" | zcat -f")).c_str(),"r");

    if(fastaFile==nullptr)
    {
        cerr << "Error: Could not open fasta file: " << fileName << endl;
        return false;
    }

    //----------------------------------------------------------------
    //Determine file size (file can be compressed -> read file in 4KB blocks)
    //----------------------------------------------------------------

    #define BLOCK_SIZE 4096

    size_t fileSize=0;
    size_t readSize;

    do
    {
        uint8_t buffer[BLOCK_SIZE];
        readSize=fread(buffer,1,BLOCK_SIZE,fastaFile);
        fileSize+=readSize;
    }
    while(readSize==BLOCK_SIZE);

    if(ferror(fastaFile))
    {
        cerr << "Error: Could not read fasta file: " << fileName << endl;
        pclose(fastaFile);
        return false;
    }

    pclose(fastaFile);

    //----------------------------------------------------------------
    //Allocate memory to hold entire file
    //----------------------------------------------------------------

    char * fileBuffer=(char*)malloc(fileSize);

    if(fileBuffer==nullptr)
    {
        cerr << "Error: Could not allocate memory for fasta file" << endl;
        return false;
    }

    //----------------------------------------------------------------
    //Open fasta file
    //----------------------------------------------------------------

    fastaFile=popen((string("bzcat -f ")+fileName+string(" | zcat -f")).c_str(),"r");

    if(fastaFile==nullptr)
    {
        cerr << "Error: Could not open fasta file: " << fileName << endl;
        free(fileBuffer);
        return false;
    }

    //----------------------------------------------------------------
    //Read the entire file into memory
    //----------------------------------------------------------------

    char * sequence=fileBuffer;

    #define MAX_LINE 4096
    char line[MAX_LINE];

    map<string,TARGET> targets;

    for(bool good=fgets(line,MAX_LINE,fastaFile)!=nullptr ; good ; )
    {
        if(line[0]=='>')
        {
            *strpbrk(line,"\n ")='\0';

            string targetName=string(line+1);
            TARGET target; target.sequence=sequence;

            for(good=fgets(line,MAX_LINE,fastaFile)!=nullptr ; good ; good=fgets(line,MAX_LINE,fastaFile)!=nullptr)
            {
                if(line[0]=='>' || line[0]==';' || line[0]=='\0')
                {
                    break;
                }

                for(char * pLine=line;*pLine!='\n' && *pLine!='\0';)
                {
                    *sequence++=*pLine++;
                }
            }

            target.length=sequence-target.sequence;
            targets[targetName]=target;
        }

        else
        {
            good=fgets(line,MAX_LINE,fastaFile)!=NULL;
        }
    }

    //----------------------------------------------------------------
    //Check if all was oke
    //----------------------------------------------------------------

    if(ferror(fastaFile))
    {
        cerr << "Error: Could not read fasta file: " << fileName << endl;
        pclose(fastaFile);
        free(fileBuffer);
        return false;
    }

    //----------------------------------------------------------------
    //Keep results
    //----------------------------------------------------------------

    this->fileBuffer=fileBuffer;
    this->targets=targets;

    //----------------------------------------------------------------
    //Done
    //----------------------------------------------------------------

    return true;
}
//----------------------------------------------------------------
void FastaFile::Clear(void)
{
    targets.clear();

    if(fileBuffer)
    {
        free(fileBuffer);
        fileBuffer=nullptr;
    }
}
//----------------------------------------------------------------



