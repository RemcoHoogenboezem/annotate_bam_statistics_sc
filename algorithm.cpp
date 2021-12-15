//----------------------------------------------------------------
// Name        : algorithm.cpp
// Author      : Remco Hoogenboezem
// Version     :
// Copyright   :
// Description :
//----------------------------------------------------------------
#include <iostream>
#include <set>
#include <omp.h>
#include "alphabet.h"
#include "barcodes_file.h"
#include "fasta_file.h"
#include "ssw_cpp.h"
#include "algorithm.h"
//----------------------------------------------------------------
using namespace StripedSmithWaterman;
//----------------------------------------------------------------
Algorithm::Algorithm(void) : countDuplicates(true) , minAlignmentScore(40) , minBaseScore(30) , nThreads(1)
{
}
//----------------------------------------------------------------
bool Algorithm::IsAltReAlign(const bam_pileup1_t * alignment,const ANNOVAR_ENTRY & entry,const char * target)
{
    uint32_t nCigar=alignment->b->core.n_cigar;
    uint32_t * cigar=bam1_cigar(alignment->b);

    int32_t queryPos=0;
    int32_t referencePos=alignment->b->core.pos;

    string refSequence;
    int32_t indelPos=-1;

    for(uint32_t i=0;i<nCigar;i++)
    {
        int32_t opLen=bam_cigar_oplen(cigar[i]);

        switch(bam_cigar_op(cigar[i]))
        {
            //Consumes query only

            case BAM_CINS:

                queryPos+=opLen;
                break;

            case BAM_CSOFT_CLIP:

                if(i==0)
                {
                    //refSequence+=string(&target[referencePos-opLen],opLen);   //First element in CIGAR
                    queryPos+=opLen;
                    break;
                }

                if(i==nCigar-1)
                {
                    //refSequence+=string(&target[referencePos],opLen);   //Last element in CIGAR
                    queryPos+=opLen;
                    break;
                }

                cerr << "Error: Soft clipping detected not at the ends of the read"     << endl;
                return false;

            //Consumes reference only

            case BAM_CDEL:

                if(entry.pos>=referencePos && entry.pos<referencePos+opLen) indelPos=int32_t(refSequence.size())+1+entry.pos-referencePos;

                refSequence+=string(&target[referencePos],opLen);
                referencePos+=opLen;
                break;

            case BAM_CREF_SKIP:

                referencePos+=opLen;
                break;

            //Consumes query and reference

            case BAM_CMATCH:
            case BAM_CEQUAL:
            case BAM_CDIFF:

                if(entry.pos>=referencePos && entry.pos<referencePos+opLen) indelPos=int32_t(refSequence.size())+1+entry.pos-referencePos;

                refSequence+=string(&target[referencePos],opLen);
                queryPos+=opLen;
                referencePos+=opLen;
                break;
        }
    }

    if(indelPos<0)
    {
        cout << "Error: could not find indel position!" << endl;
        return false;
    }

    string altSequence=refSequence;

    if(entry.type==INS)
    {
        altSequence.insert(size_t(indelPos),entry.alt);
    }
    else
    {
        altSequence.erase(size_t(indelPos),entry.ref.size());
    }

    Aligner refAligner,altAligner;
    refAligner.SetReferenceSequence(refSequence.c_str(),int(refSequence.length()));
    altAligner.SetReferenceSequence(altSequence.c_str(),int(altSequence.length()));

    Filter filter;
    Alignment refAlignment,altAlignment;

    int32_t queryLen=alignment->b->core.l_qseq;

    string bases=string(size_t(queryLen),'\0');

    for(int32_t i=0;i<queryLen;i++) bases[size_t(i)]=bamSeq2Ascii[bam1_seqi(bam1_seq(alignment->b),i)];

    refAligner.Align(bases.c_str(),filter,&refAlignment);
    altAligner.Align(bases.c_str(),filter,&altAlignment);

    return (altAlignment.sw_score>refAlignment.sw_score);
}
//----------------------------------------------------------------
bool Algorithm::IsAlt(const bam_pileup1_t * alignment,const ANNOVAR_ENTRY & entry,const char * target)
{
    //----------------------------------------------------------------
    //Insertion
    //----------------------------------------------------------------

    if(entry.type==INS)
    {
        if(alignment->indel==int(entry.alt.length())) return true;
        return IsAltReAlign(alignment,entry,target);

    }

    //----------------------------------------------------------------
    //Deletion
    //----------------------------------------------------------------

    if(entry.type==DEL)
    {
        if(alignment->indel==-int(entry.ref.length())) return true;
        return IsAltReAlign(alignment,entry,target);
    }

    //----------------------------------------------------------------
    //Point mutation
    //----------------------------------------------------------------

    return bam1_seqi(bam1_seq(alignment->b),alignment->qpos)==entry.type && bam1_qual(alignment->b)[alignment->qpos]>=minBaseScore;
}
//----------------------------------------------------------------
bool Algorithm::Run(void)
{
    //----------------------------------------------------------------
    //Check input arguments
    //----------------------------------------------------------------

    cerr << "Info: Check input arguments" << endl;

    if(bamFileName.empty())
    {
        cerr << "Error: Please specify a bam file! (-h for help)" << endl;
        return false;
    }

    if(annovarFileName.empty())
    {
        cerr << "Error: Please specify an annovar file! (-h for help)" << endl;
        return false;
    }

    if(barcodesFileName.empty())
    {
        cerr << "Error: Please specify a barcodes file! (-h for help)" << endl;
        return false;
    }

    if(fastaFileName.empty())
    {
        cerr << "Error: Please specify a fasta file! (-h for help)" << endl;
        return false;
    }

    if(nThreads<1) nThreads=1;

    //----------------------------------------------------------------
    //Open bamFle (test if we can open the bam file)
    //----------------------------------------------------------------

    cerr << "Info: Test bam file (if present)" << endl;

    {
        BamFile bamFile;

        if(bamFile.Open(bamFileName,minAlignmentScore,countDuplicates)==false)
        {
            return false;
        }

        bamFile.Close();
    }

    //----------------------------------------------------------------
    //Open annovar file
    //----------------------------------------------------------------

    cerr << "Info: Open annovar file" << endl;

    AnnovarFile annovarFile;

    if(annovarFile.Open(annovarFileName)==false)
    {
        return false;
    }

    size_t nEntires=annovarFile.entries.size();

    //----------------------------------------------------------------
    //Open barcodes file
    //----------------------------------------------------------------

    cerr << "Info: Open barcodes file" << endl;

    BarcodesFile barcodesFile;

    if(barcodesFile.Open(barcodesFileName)==false)
    {
        return false;
    }

    //----------------------------------------------------------------
    //Open fasta file
    //----------------------------------------------------------------

    cerr << "Info: Open fasta file" << endl;

    FastaFile fastaFile;

    if(fastaFile.Open(fastaFileName)==false)
    {
        return false;
    }

    //----------------------------------------------------------------
    //Pileup regions
    //----------------------------------------------------------------

    cerr << "Info: Pileup regions" << endl;

    cout << annovarFile.header << "\tcellBarcodes\trefCount\taltCount\ttotalRefCount\ttotalAltCount\tVAF" << endl;

    omp_set_num_threads(nThreads);

    #pragma omp parallel
    {
        //----------------------------------------------------------------
        //Open per thread bam file
        //----------------------------------------------------------------

        BamFile bamFile;

        if(bamFile.Open(bamFileName,minAlignmentScore,countDuplicates)==false)
        {
            goto END;
        }

        //----------------------------------------------------------------
        //Iterate over annovar file entries
        //----------------------------------------------------------------

        #pragma omp for schedule(dynamic)
        for(size_t i=0;i<nEntires;i++)
        {
            auto entry=annovarFile.entries[i];

            //----------------------------------------------------------------
            //Set region
            //----------------------------------------------------------------

            string region=entry.chr+string(":")+to_string(max(entry.pos-10,1))+string("-")+to_string(entry.pos+10);

            if(bamFile.SetRegion(region)==false)
            {
                #pragma omp critical
                cerr << "Warning: Could not set region: " << region << " (skipping)" << endl;

                continue;
            }

            if(fastaFile.targets.count(entry.chr)==0)
            {
                #pragma omp critical
                cerr << "Warning: Could not fetch target from fasta file: " << entry.chr << " (skipping)" << endl;

                continue;
            }

            const char * target=fastaFile.targets.at(entry.chr).sequence;

            auto barcodes=barcodesFile.entries;

            //----------------------------------------------------------------
            //Pileup region
            //----------------------------------------------------------------

            PLP_DATA * pileupData=bamFile.GetPileupData();

            while(bamFile.PileupRegion())
            {
                if(pileupData->targetPos!=entry.pos) continue;

                int nAlignments=pileupData->nAlignments;
                const bam_pileup1_t * alignments=pileupData->alignments;

                for(int i=0;i<nAlignments;i++)
                {
                    if(alignments[i].is_del || alignments[i].is_refskip) continue;

                    char * cellBarcode=reinterpret_cast<char*>(bam_aux_get(alignments[i].b,"CB")); // CB = chromium cellular barcode sequence that is error-corrected and confirmed against a list of known-good barcode sequences.

                    if(cellBarcode++==nullptr) continue;

                    if(barcodes.count(cellBarcode)==0) continue;

                    char * umiBarcode=reinterpret_cast<char*>(bam_aux_get(alignments[i].b,"UB")); // UB = Chromium molecular barcode sequence that is error-corrected among other molecular barcodes with the same cellular barcode and gene alignment.

                    if(umiBarcode++==nullptr) continue;

                    barcodes[cellBarcode][umiBarcode]|=IsAlt(alignments+i,entry,target);
                }
            }

            //----------------------------------------------------------------
            //Output results
            //----------------------------------------------------------------

            int totalRefCount=0;
            int totalAltCount=0;

            for(auto & cellBarcode : barcodes)
            {
                int altCount=0;

                for(auto & umiBarcode : cellBarcode.second)
                {
                    altCount+=umiBarcode.second;
                }

                totalRefCount+=int(cellBarcode.second.size())-altCount;
                totalAltCount+=altCount;
            }

            for(auto & cellBarcode : barcodes)
            {
                int altCount=0;

                for(auto & umiBarcode : cellBarcode.second)
                {
                    altCount+=umiBarcode.second;
                }

                int refCount=int(cellBarcode.second.size())-altCount;

                #pragma omp critical
                cout << entry.line << '\t' << cellBarcode.first << '\t' << refCount << '\t' << altCount << '\t' << totalRefCount << '\t' << totalAltCount << '\t' << double(totalAltCount) / double(max(totalRefCount+totalAltCount,1)) << endl;
            }
        }
END: ;
    }

    //----------------------------------------------------------------
    //Done
    //----------------------------------------------------------------

    cerr << "Info: Done" << endl;

    return true;
}
//----------------------------------------------------------------

/*
                    int32_t diff=alignment->qpos-queryPos;

                    if(diff>=0 && diff<opLen)
                    {
                        insPos=int32_t(refSequence.size())+diff+1;
                    }
*/
