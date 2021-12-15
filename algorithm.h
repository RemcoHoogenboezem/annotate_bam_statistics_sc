//----------------------------------------------------------------
#ifndef ALGORITHM_H
#define ALGORITHM_H
//----------------------------------------------------------------
#include <string>
#include "bam_file.h"
#include "annovar_file.h"
//----------------------------------------------------------------
using namespace std;
//----------------------------------------------------------------
class Algorithm
{
    private:

        bool IsAltReAlign(const bam_pileup1_t * alignment,const ANNOVAR_ENTRY & entry,const char * target);
        bool IsAlt(const bam_pileup1_t * alignment,const ANNOVAR_ENTRY & entry,const char * refSequence);

    public:

        bool countDuplicates;

        uint32_t minAlignmentScore;
        uint8_t minBaseScore;

        int nThreads;

        string bamFileName;
        string annovarFileName;
        string barcodesFileName;
        string fastaFileName;

        Algorithm(void);
        bool Run(void);
};
//----------------------------------------------------------------
#endif // ALGORITHM_H
