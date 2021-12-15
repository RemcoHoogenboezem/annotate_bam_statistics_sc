//----------------------------------------------------------------
#ifndef BAM_FILE_H
#define	BAM_FILE_H
//----------------------------------------------------------------
#include <string>
#include "bam.h"
//----------------------------------------------------------------
using namespace std;
//----------------------------------------------------------------
typedef struct
{
    int                     targetId;
    int                     targetPos;

    int                     nAlignments;
    const bam_pileup1_t *   alignments;

}PLP_DATA;
//----------------------------------------------------------------
class BamFile
{
    private:

        bool            initialized;

        uint32_t        minQual;

        string          fileName;

        bamFile         fileHandle;
        bam_header_t *  bamHeader;
        bam_index_t *   bamIndex;
        bam_plp_t       pileupObject;
        bam_iter_t      bamRegion;

        PLP_DATA        pileupData;

        static int PileupCallback(void * userData,bam1_t * alignment);

    public:

        BamFile(void);
        ~BamFile(void);

        bool            Open(const string & fileName,uint32_t minQual,bool countDuplicates);
        void            Close(void);

        const char *    GetFileName(void);

        int             GetTargetCount(void);
        int             GetTargetLen(int targetId);
        const char *    GetTargetName(int targetId);
        PLP_DATA *      GetPileupData(void);

        bool            SetRegion(const string & region);
        bool            PileupRegion(void);

        int             Read(bam1_t * alignment);
        int             ReadRegion(bam1_t * alignment);
};
//----------------------------------------------------------------
#endif
