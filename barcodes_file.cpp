//----------------------------------------------------------------
// Name        : barcode_file.cpp
// Author      : Remco Hoogenboezem
// Version     :
// Copyright   :
// Description :
//----------------------------------------------------------------
#include <iostream>
#include <cstring>
#include "barcodes_file.h"
//----------------------------------------------------------------
#define MAX_LINE_LEN 4096
//----------------------------------------------------------------
bool BarcodesFile::Open(const string & fileName)
{
    FILE * barcodesFile=popen((string("bzcat -f ")+fileName+string(" | zcat -f")).c_str(),"r");

    if(barcodesFile==nullptr)
    {
        cerr << "Error: Could not open barcode file: " << fileName << endl;
        return false;
    }

    char line[MAX_LINE_LEN];

    while(fgets(line,MAX_LINE_LEN,barcodesFile)!=nullptr)
    {
        line[strcspn(line,"\n")]='\0';
        entries[string(line)];
    }

    pclose(barcodesFile);

    return true;
}
//----------------------------------------------------------------
