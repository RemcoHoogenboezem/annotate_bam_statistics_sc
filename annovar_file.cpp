//----------------------------------------------------------------
// Name        : annovar_file.cpp
// Author      : Remco Hoogenboezem
// Version     :
// Copyright   :
// Description :
//----------------------------------------------------------------
#include <iostream>
#include <cstring>
#include "alphabet.h"
#include "annovar_file.h"
//----------------------------------------------------------------
#define MAX_LINE_LEN    16384
//----------------------------------------------------------------
bool AnnovarFile::Open(const string & fileName)
{
    //----------------------------------------------------------------
    //Open annovar file
    //----------------------------------------------------------------

    FILE * annovarFile=popen((string("bzcat -f ")+fileName+string(" | zcat -f")).c_str(),"r");

    if(annovarFile==nullptr)
    {
        cerr << "Error: Could not open annovar file: " << fileName << endl;
        return false;
    }

    //----------------------------------------------------------------
    //Read and check annovar header
    //----------------------------------------------------------------

    char line[MAX_LINE_LEN];

    if(fgets(line,MAX_LINE_LEN,annovarFile)==nullptr)
    {
        cerr << "Error: Could not read header from annovar file: " << fileName << endl;
        pclose(annovarFile);
        return false;
    }

    if(strstr(line,"Chr\tStart\tEnd\tRef\tAlt")==nullptr)
    {
        cerr << "Error: Invalid annovar file header: " << fileName << endl;
        pclose(annovarFile);
        return false;
    }

    line[strcspn(line,"\n")]='\0';
    header=string(line);

    //----------------------------------------------------------------
    //Read annovar entries
    //----------------------------------------------------------------

    nEntries=0;
    entries.clear();

    ANNOVAR_ENTRY entry;

    while(fgets(line,MAX_LINE_LEN,annovarFile)!=nullptr)
    {
        line[strcspn(line,"\n")]='\0';
        entry.line=string(line);

        char * linep=line;
        char * chr=strsep(&linep,"\t");
        char * start=strsep(&linep,"\t");
        strsep(&linep,"\t");
        char * ref=strsep(&linep,"\t");
        char * alt=strsep(&linep,"\t");

        if(alt==nullptr)
        {
            cerr << "Error: Insufficient number of columns in  annovar file: " << fileName << endl;
            pclose(annovarFile);
            return false;
        }

        entry.chr=string(chr);

        //----------------------------------------------------------------
        //Insertion
        //----------------------------------------------------------------

        if(ref[0]=='-')
        {
            entry.pos=atoi(start)-1;

            entry.type=INS;
            entry.ref=string(ref);
            entry.alt=string(alt);
            entries.push_back(entry);
        }

        //----------------------------------------------------------------
        //Deletion
        //----------------------------------------------------------------

        else if(alt[0]=='-')
        {
            entry.pos=atoi(start)-2;

            entry.type=DEL;
            entry.ref=string(ref);
            entry.alt=string(alt);
            entries.push_back(entry);
        }

        //----------------------------------------------------------------
        //Substitution
        //----------------------------------------------------------------

        else
        {
            entry.pos=atoi(start)-1;

            entry.type=ascii2BamSeq[int(alt[0])];
            entry.ref=string(ref);
            entry.alt=string(alt);
            entries.push_back(entry);
        }

        nEntries++;
    }

    pclose(annovarFile);

    //----------------------------------------------------------------
    //Done
    //----------------------------------------------------------------

    return true;
}
//----------------------------------------------------------------
