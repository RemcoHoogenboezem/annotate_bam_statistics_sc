//----------------------------------------------------------------
#ifndef FASTA_FILE_H
#define	FASTA_FILE_H
//----------------------------------------------------------------
#include <string>
#include <map>
//----------------------------------------------------------------
using namespace std;
//----------------------------------------------------------------
typedef struct
{
    size_t  length;
    char *  sequence;

}TARGET;
//----------------------------------------------------------------
class FastaFile
{
    private:

        char * fileBuffer;

        static char * GetLine(char ** ppFile);

    public:

        map<string,TARGET> targets;

        FastaFile(void);
        ~FastaFile(void);

        bool Open(const string & fileName);
        void Close(void);

        void Clear(void);
};
//----------------------------------------------------------------
#endif
