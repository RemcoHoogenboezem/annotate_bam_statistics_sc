//----------------------------------------------------------------
#ifndef BARCODES_FILE_H
#define BARCODES_FILE_H
//----------------------------------------------------------------
#include <string>
#include <map>
//----------------------------------------------------------------
using namespace std;
//----------------------------------------------------------------
class BarcodesFile
{
    private:
    public:

        map<string,map<string,int> > entries;
        bool Open(const string & fileName);
};
//----------------------------------------------------------------
#endif // BARCODES_FILE_H
