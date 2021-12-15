//----------------------------------------------------------------
#ifndef ANNOVAR_FILE_H
#define ANNOVAR_FILE_H
//----------------------------------------------------------------
#include <string>
#include <vector>
//----------------------------------------------------------------
using namespace std;
//----------------------------------------------------------------
typedef struct
{
    string  chr;
    int32_t pos;

    int     type;
    string  ref;
    string  alt;
    string  line;

}ANNOVAR_ENTRY;
//----------------------------------------------------------------
class AnnovarFile
{
    private:

    public:

        int                     nEntries;
        string                  header;
        vector<ANNOVAR_ENTRY>   entries;

        bool Open(const string & fileName);
};
//----------------------------------------------------------------
#endif // ANNOVAR_FILE_H
